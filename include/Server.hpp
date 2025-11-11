#pragma once

#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>


class Server
{
private:
	/* data */
	int						_listen_fd; // This is set up as soon as the Server instance is created.
	std::vector<pollfd>		_poll_fds; // fd array waiting for the poll().
	std::map<int, Client>	_clients;
	std::string				_password;

	/* utils */
	void	setupListenSocket_(int port);
	void	acceptNewClient();
	void	receiveFromClient(int fd);




public:
	/* default */
	Server(int port, std::string &password);
	~Server();

	/* method */
	void	run();
};
