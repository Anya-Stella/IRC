#include "../include/Server.hpp"

/* subUtils */
namespace
{
	//sub utils
	sockaddr_in	init_socket_address(const std::string &ip, int port)
	{
		sockaddr_in	addr;
		std::memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET; //IPv4
		addr.sin_port = htons(port);
		
		if (ip.empty() || ip == "0.0.0.0")
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		else
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
		return (addr);
	}
}

/* default */
Server::Server(int port, std::string &password) : _listen_fd(-1), _password(password), _channels()
{
	std::cout << "Generate Server ..." << std::endl;
	try
	{
		// set up _listen_fd
		setupListenSocket_(port);
		
		// add _listen_fd to _poll_fds
		pollfd p = { _listen_fd, POLLIN, 0 };
		_poll_fds.push_back(p);

		std::cout << "Generate Server Complete ...!" << std::endl;
	}
	catch(...)
	{
		if (_listen_fd != -1)
		{
			::close(_listen_fd);
			_listen_fd = -1;
		}
		throw;
	}
}

Server::~Server()
{
	// close client_FD
	for( std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it )
		::close(it->first);

	// close server_FD
	if (_listen_fd != -1)
		::close(_listen_fd);

	std::cout << "Server closed." << std::endl;
}

void	Server::run()
{
	std::cout << "Server is running..." << std::endl;

	while (true)
	{
		// poll()
		if (_poll_fds.empty())
			throw std::runtime_error("no fds to poll");

		std::cout << "これから待つよ\n" << std::endl; //TODO:
		std::cout << _poll_fds.size() << std::endl;//TODO:

		int	nready = ::poll(&_poll_fds[0], _poll_fds.size(), -1);
		if (nready < 0)
			throw std::runtime_error("poll failed");

		std::cout << "待つの終わり\n" << std::endl;//TODO:
		
		// scanning
		for (size_t	i = 0; i < _poll_fds.size(); ++i)	
		{
			pollfd	p = _poll_fds[i];

			// POLLIN flag？
			if (p.revents & POLLIN)
			{
				if (p.fd == _listen_fd)
					acceptNewClient();
				else
				{
					receiveFromClient(p.fd);
				}
			}

			// other flag?
			//TODO:
			// if (p.revents & POLLOUT)
			// {

			// }

			// if (p.revents & (POLLERR | POLLHUP | POLLNVAL))
			// {

			// }
		}
	}
}

/* utils */
void	Server::setupListenSocket_(int port)
{
	// generate and setup socket
	// use IPv4, TCP
	_listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw std::runtime_error("generate socket failed");

	// set SO_REUSEADDR 'ON', this allows the port to be used immediately after a reboot.
	// avoid "bind failed: Address already in use"
	int optValue = 1;
    ::setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(optValue));

	// this allows Useing 'NONBLOCK', 'non-block' is system-call fanction don't wait anything.
	// ex) accept() ,recv() returns immediatery. 
    ::fcntl(_listen_fd, F_SETFL, O_NONBLOCK);

	// set socket address.
	sockaddr_in	addr = init_socket_address("0.0.0.0", port);
	
	// bind
	if (::bind(_listen_fd, (sockaddr *) &addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind failed");

	// listen
	if (::listen(_listen_fd, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");

	/* _listen_fd does not hold Information about individual client. 
	   It is associated with the queue. */
}

// delete ClientFD, and _client.
void	Server::disconnectClient(int fd)
{
	// erase from _poll_fds
	for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_poll_fds.erase(it);
			break;
		}
	}
		
	// erase from _clients
	_clients.erase(fd);

	// close fd
	::close(fd);

	std::cout << "disconnect Client." << std::endl;
}

// Handling Multiple connection requests
void	Server::acceptNewClient()
{
	while (true)
	{
		sockaddr_in	cliaddr;
		socklen_t	len = sizeof(cliaddr);
		int			cfd = ::accept(_listen_fd, (sockaddr *) &cliaddr, &len);
		if (cfd < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			if (errno == EINTR)
				continue;
			throw std::runtime_error("accept failed.");
		}

		// NONBLOCK
		if (::fcntl(cfd, F_SETFL, O_NONBLOCK) < 0) {
			::close(cfd);
			throw std::runtime_error("fcntl() failed");
		}

		// push
		pollfd	p = { cfd, POLLIN, 0 };
		_poll_fds.push_back(p);

		// register client
		_clients[cfd] = new Client(cfd);


		std::cout << "Accepted " << cfd << " " << std::endl; 
	}
}


Client* Server::getClientPtr(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return (it->second);
	return NULL;
}

void	Server::mkPmsg(const std::string& line, ParsedMessage &pmsg)
{
	size_t i = 0;
	size_t len = line.size();

	while (i < len)
	{
		while (i < len && std::isspace(static_cast<unsigned char>(line[i])))
			++i;
		if (i >= len)
			break;

		// ':' to end 
		if (line[i] == ':' && i > 0)
		{
			++i;
			std::string trail = line.substr(i);
			pmsg.params.push_back(trail);
			return;
		}

		// normal
		size_t start = i;
		while (i < len && !std::isspace(static_cast<unsigned char>(line[i])))
			++i;

		std::string token = line.substr(start, i - start);

		if (pmsg.command.empty())
			pmsg.command = token;
		else
			pmsg.params.push_back(token);
	}
}

rcv_resp	Server::acceptClientMessage(int fd, std::string& cliant_buff)
{
	char buff[RCV_MAXBUFF];

	while (1)
	{
		ssize_t rsize = recv(fd, buff, sizeof(buff) - 1, 0);
		if(rsize > 0)
		{
			buff[rsize] = '\0';
			cliant_buff += buff;
			continue;
		}
		else if(rsize == 0)
		{
			return (save_laststr);
		}
		if (errno == EINTR)
			continue;
		else if (errno == EAGAIN || errno == EWOULDBLOCK )
			return (save_getstr);
		else if (errno == ECONNRESET)
			return (close_fd);
		else
			return (close_fd);
	}
}

void	Server::receiveFromClient(int fd)
{
	Client *client = getClientPtr(fd);
	if(client == NULL)
		return ;

	rcv_resp rtn = acceptClientMessage(fd, client->adjBuff());
	if(rtn == close_fd)
	{
		disconnectClient(fd);
		std::cout << "client message error, disconnect client." << std::endl;
		return ;
	}
	if (client->getBuff().size() == 0)
		return ;
	if (client->getBuff().size() > CLIENT_MAXBUFF)
	{
		std::cerr << "err: too large data accept and disconnect client" << "client->getNickname()" << std::endl;
		disconnectClient(fd);
		std::cout << "disconnect client." << std::endl;
		return ;
	}
	while(client->devBuff("\r\n"))
	{
		if (client->getCmd().size() > CMD_MAXBUFF)
		{
			std::cerr << "err: too large cmdline accept and disconnect client" << "client->getNickname()" << std::endl;
			disconnectClient(fd);
			std::cout << "disconnect client." << std::endl;
			return ;	
		}
		ParsedMessage pmsg;
		mkPmsg(client->getCmd(),pmsg);

		// debug
		std::cout << "\033[31m --------- Received --------- \033[m" << std::endl;
		printPmsg(pmsg);
		std::cout << "\033[31m ---------    End    --------- \033[m" << std::endl;	
	}
	return ;
}
