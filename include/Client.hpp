#pragma once
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <cerrno>
#define CLIENT_MAXBUFF 8192	// mkuida
#define CMD_MAXBUFF 512		// mkuida

class Server;

class Client
{
private:
	/* data */
	int _fd;
	std::string					_nickname;
	std::string					_username;
    std::string					_realname;
	std::vector<std::string>	_channels;
	time_t						_lastPongTime;
	std::string					_buff;	//mkuida
	std::string					_cmd;	//mkuida

	/* already registered? */
	bool _passAccepted;
    bool _hasNick;
    bool _hasUser;
	bool _registered;

public:
	explicit	Client(int fd);
	~Client();
	int			getFd() const { return _fd; }

	/*PASS*/
	std::string	getNickname() const;
	void		sendMessage(const std::string &msg);
	bool		isPassAccepted() const;
	void		setPassAccepted(bool v);
	bool		isFullyRegistered() const;

	/*NICK*/
	bool		hasNick() const { return !_nickname.empty(); }
	void		changeHasNicktoTrue() { if (!_hasNick) _hasNick = true; }
	void		setNick(std::string n) { _nickname = n; }
	bool		tryToRegister();

	/*USER*/
	void		setUsername(const std::string &name) { _username = name; }
    void		setRealname(const std::string &name) { _realname = name; }
    void		setHasUser(bool b) { _hasUser = b; }

	/*PONG*/
	void		updatePongTime(){ _lastPongTime = time(NULL); }
	
	/*JOIN*/
	void		joinChannel(const std::string& name) { _channels.push_back(name); }
    void		partChannel(const std::string& name);
    const std::vector<std::string>& getChannels() const { return _channels; }

	/* cmd */
	const std::string& getBuff( void ) const;	//mkuida
	void setBuff( const std::string& str );		//mkuida
	const std::string& getCmd( void ) const;	//mkuida
	void setCmd( const std::string& str );		//mkuida
	bool devBuff(const char* str);				//mkuida
	std::string& adjBuff();						//mkuida
};
