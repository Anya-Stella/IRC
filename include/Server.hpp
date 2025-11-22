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
#include <cerrno> 
#include <ctype.h>

struct ParsedMessage;

class Server
{
private:
	/* data */
	int						_listen_fd; // This is set up as soon as the Server instance is created.
	std::vector<pollfd>		_poll_fds; // fd array waiting for the poll().
	std::map<int, Client*>	_clients;
	std::string				_password;

	/* utils */
	void	setupListenSocket_(int port);
	void	acceptNewClient();
	void	receiveFromClient(int fd);

	/* cmds */
	void	executeCmds(Client &c, const ParsedMessage &msg);
	void	handleNICK(Client &c, const std::vector<std::string> &params);
	void 	handlePASS(Client &c, const std::vector<std::string> &params);
	void 	handleUSER(Client &c, const std::vector<std::string> &params);
	void 	handlePING(Client &c, const std::vector<std::string> &params);

	/*PASS*/
	void 	sendWelcome(Client &c);
	/*USER*/
	bool 	isValidNick(const std::string& nickname);
	bool 	isUsedNick(const std::string& nickname);
	void 	broadcastToChannels(Client& sender, const std::string& message);


	// ...


public:
	/* default */
	Server(int port, std::string &password);
	~Server();

	/* method */
	void	run();
};

struct ParsedMessage {
	std::string					command;
	std::vector<std::string>	params;
};