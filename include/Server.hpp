#pragma once

#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "Client.hpp"

#define RCV_MAXBUFF 1024 // rcv message

typedef enum
{
	save_getstr,
	save_laststr,
	close_fd
} rcv_resp;

struct ParsedMessage
{
	std::string					command;
	std::vector<std::string>	params;
};

class Server
{
private:
	/* data */
	int						_listen_fd; // This is set up as soon as the Server instance is created.
	std::vector<pollfd>		_poll_fds; // fd array waiting for the poll().
	std::map<int, Client>	_clients; // mkuida-memo int ha soketfd
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

void printPmsg(const ParsedMessage& p)	//mkuida
{
	std::cout << "command : " << p.command << std::endl;
	if(p.params.empty())
		std::cout << "params : none" << std::endl;
	else
	{
		for(int i = 0 ; i < p.params.size() ; ++i)
		{
			std::cout << "params[" << i << "] : " << p.params[i] << std::endl;
		}
	}
}
