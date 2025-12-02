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
#include <cerrno>
#include "Client.hpp"
#include "Utils.hpp"
#include <cerrno>

#define RCV_MAXBUFF 1024 // rcv_message mkuida
struct ParsedMessage;

typedef enum	// mkuida
{
	save_getstr,
	save_laststr,
	close_fd
} rcv_resp;

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
	void	disconnectClient(int fd);
	void	acceptNewClient();
	void	receiveFromClient(int fd);

	/* cmds */
	void	executeCmds(Client &c, const ParsedMessage &msg);
	void	handleNICK(Client &c, const std::vector<std::string> &params);
	// ...
	void		mkPmsg(const std::string& line, ParsedMessage &pmsg);
	rcv_resp	acceptClientMessage(int fd, std::string& cliant_buff);
	Client*		getClientPtr(int fd);
	

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