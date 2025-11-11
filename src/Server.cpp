#include "../include/Server.hpp"


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
Server::Server(int port, std::string &password) : _listen_fd(-1), _password(password)
{
	std::cout << "Generate Server ..." << std::endl;

	// set up _listen_fd
	setupListenSocket_(port);
	
	// add _listen_fd to _poll_fds
	pollfd p = { _listen_fd, POLLIN, 0 };
	_poll_fds.push_back(p);

	std::cout << "Generate Server Complete ...!" << std::endl;
}

// void	Server::run()
// {
// 	std::cout << "Server is running..." << std::endl;

// 	while (true)
// 	{
		
// 	}
// }







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
