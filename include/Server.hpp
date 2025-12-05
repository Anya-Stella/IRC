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
#include <ctype.h>
#include "Client.hpp"
#include "Utils.hpp"

#define RCV_MAXBUFF 1024 // rcv_message mkuida
struct ParsedMessage;
class Channel;

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
	std::map<int, Client*>	_clients;
	std::string				_password;
	std::map<std::string, Channel*> _channels; // サーバすべてのチャンネル名のリスト
	std::string				_server_name;


	/* utils */
	void	setupListenSocket_(int port);
	void	disconnectClient(int fd);
	void	acceptNewClient();
	void	receiveFromClient(int fd);

	/* cmds */
	void	executeCmds(Client &c, const ParsedMessage &msg);
	void	handleCAP(Client &c, const std::vector<std::string> &params);
	void	handleNICK(Client &c, const std::vector<std::string> &params);
	void 	handlePASS(Client &c, const std::vector<std::string> &params);
	void 	handleUSER(Client &c, const std::vector<std::string> &params);
	void 	handlePING(Client &c, const std::vector<std::string> &params);
	void 	handlePONG(Client &c, const std::vector<std::string> &params);
	void 	handleJOIN(Client &c, const std::vector<std::string> &params);
	void 	handlePART(Client &c, const std::vector<std::string> &params);
	void 	handlePRIVMSG(Client& sender, const std::vector<std::string>& params);
	void 	handleNOTICE(Client& sender, const std::vector<std::string>& params);
	void	handleKICK(Client& sender, const std::vector<std::string>& params);
	void 	handleQUIT(Client& c, const std::vector<std::string>& params);
	void 	handleINVITE(Client& sender, const std::vector<std::string>& params);
	void 	handleTOPIC(Client& c, const std::vector<std::string>& params);
	void 	handleMODE(Client& sender, const std::vector<std::string>& params);


	/*PASS*/
	void 	sendWelcome(Client &c);
	/*USER*/
	bool 	isValidNick(const std::string& nickname);
	bool 	isUsedNick(const std::string& nickname);
	void	broadcastToAllUserChannels(Client& sender, const std::string& message);
	/*NICK*/
	std::string	toUpperCaseString(const std::string& n);

	/*JOIN*/
	void	broadcastToChannel(Channel& ch, const std::string& message);
	void	sendNamesReply(Client& c, const Channel& channel);
	/*PART*/
	std::vector<std::string> splitChannels(const std::string& param);
	std::string 	trim(const std::string& s);
	/*PRIVMSG*/
	Client*	findClientByNick(const std::string& nickname);
	void 	broadcastToChannel(Channel& ch, const std::string& message, Client* sender);




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