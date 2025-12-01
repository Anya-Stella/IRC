#include "../include/Server.hpp"

/* default */
Server::Server(int port, std::string &password) : _listen_fd(-1), _password(password)
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
	if (_listen_fd == -1)
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
		int	nready = ::poll(&_poll_fds[0], _poll_fds.size(), -1);
		if (nready < 0)
			throw std::runtime_error("poll failed");
		
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
					receiveFromClient(p.fd);
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
		_clients.insert(std::make_pair(cfd, Client(cfd)));

		std::cout << "Accepted " << cfd << " " << std::endl; 
	}
}

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

//mkuida

void mkPmsg(const std::string& line, ParsedMessage &pmsg)
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

rcv_rtn acceptCliantMessage(int fd, std::string& cliant_message)
{
	char buff[RCV_MAXBUFF];

	while (1)
	{
		sszie_t rsize = recv(fd, buf, sizeof(buff) - 1, 0);
		if(rsize > 0)
		{
			getstr = true;
			buff[rsize] = '\0';
			cliant_message += buff;
			continue;
		}
		else if(rsize == 0)
		{
			return (save_laststr);
		}
		if (errno == EINTER)
			continue;
		else if (errno == EAGAIN || errno == EWOULDBLOCK )
			return (save_getstr);
		else if (errno == ECONNRESET)
			return (close_fd);
		else
			return (close_fd) // ←huan
	}
}

void	Server::receiveFromClient(int fd)
{
	Client *client = getCliantPtr(fd);

	rcv_resp rtn = acceptCliantMessage(fd, cliant->setBuff());
	if(rtn == close_fd)
	{
		close_fd(fd);
		return ;
	}
	if (cliant->getBuff.size() == 0)
		return ;
	if (cliant->getBuff.size() > CLIENT_MAXBUF)
	{
		std::cerr << "err : too large data accept and disconect cliant" << cliant->getNickname() << std::endl;
		close_fd(fd);
		return ;
	}
	whlie(client.devBuff("\r\n"))
	{
		if (client.getCmd().size() > CMD_MAXBUFF)
		{
			std::cerr << "err : too large cmdline accept and disconect cliant" << cliant->getNickname() << std::endl;
			close_fd(fd);
			return ;	
		}
		std::cout << "\n \033[31m --- New data received --- \033[m" << std::endl;
		ParsedMessage pmsg;
		mkPmsg(client.getCmd(),pmsg);
		// consumeClientBuffLine(fd, client->setCmd());
		std::cout << "\033[31m --- Receiving ends --- \033[m" << std::endl;	
	}

	return ;
}
