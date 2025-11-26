#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <string>

class Server;

// ここはクライアント側の状態のみを扱う
class Client
{
private:
	/* data */
	int _fd;
	bool _passAccepted;    // PASS コマンド受理済みか
    bool _hasNick;         // NICK コマンド受理済みか
    bool _hasUser;         // USER コマンド受理済みか
	bool _registered;	   // PASS,NICK,USERがすべて登録完了してるか
	std::string	_nickname; //表に出る名前
	std::string _username; //ログインIDみたいなもの
    std::string _realname; //本名
	/*PONG*/
	time_t _lastPongTime;  // 最後にPONGを受信したタイムスタンプ
	/*JOIN*/
	std::vector<std::string> _channels; // クライアントが参加しているチャンネル名一覧



public:
	// とりあえずこれだけ
	explicit	Client(int fd);
	int getFd() const { return _fd; }
	/*PASS*/
	std::string getNickname() const;
	void 	sendMessage(const std::string &msg);
	bool	isPassAccepted() const;
	void	setPassAccepted(bool v);
	bool	isFullyRegistered() const;
	/*NICK*/
	bool 	hasNick() const { return !_nickname.empty(); }
	void 	setNick(std::string n) { _nickname = n; }
	bool 	readyToRegister() const;
	void 	setRegistered(bool status);
	/*USER*/
	void  	setUsername(const std::string &name) { _username = name; }
    void  	setRealname(const std::string &name) { _realname = name; }
    void  	setHasUser(bool b) { _hasUser = b; }
	/*PONG*/
	void 	updatePongTime(){ _lastPongTime = time(NULL); }
	/*JOIN*/
	void 	joinChannel(const std::string& name) { _channels.push_back(name); }
    void 	partChannel(const std::string& name);
    const 	std::vector<std::string>& getChannels() const { return _channels; }

	~Client();
};
