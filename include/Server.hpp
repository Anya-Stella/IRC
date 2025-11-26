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
class Channel;

class Server
{
private:
	/* data */
	int						_listen_fd; // This is set up as soon as the Server instance is created.
	std::vector<pollfd>		_poll_fds; // fd array waiting for the poll().
	std::map<int, Client*>	_clients;
	std::string				_password;
	std::map<std::string, Channel*> _channels; // サーバすべてのチャンネル名のリスト


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
	void 	handlePONG(Client &c, const std::vector<std::string> &params);
	void 	handleJOIN(Client &c, const std::vector<std::string> &params);
	void 	handlePART(Client &c, const std::vector<std::string> &params);

	/*PASS*/
	void 	sendWelcome(Client &c);
	/*USER*/
	bool 	isValidNick(const std::string& nickname);
	bool 	isUsedNick(const std::string& nickname);
	void	broadcastToAllUserChannels(Client& sender, const std::string& message);
	/*JOIN*/
	void	broadcastToChannel(Channel& ch, const std::string& message);
	void	sendNamesReply(Client& c, const Channel& channel);

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