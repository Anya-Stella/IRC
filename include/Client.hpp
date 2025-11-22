#pragma once
#include <string>
#include <iostream>

// ここはクライアント側の状態のみを扱う
// 


class Client
{
private:
	/* data */
	int _fd;
	bool _passAccepted;    // PASS コマンド受理済みか
    bool _hasNick;         // NICK コマンド受理済みか
    bool _hasUser;         // USER コマンド受理済みか
	bool _registered;	   // PASS,NICK,USERがすべて登録完了してるか
	// ここはコマンドとか実装してくに当たって必要そうなのを入れてけばいいと思う
	std::string	_nickname; //表に出る名前
	std::string _username; //ログインIDみたいなもの
    std::string _realname; //本名


	// ...



public:
	// とりあえずこれだけ
	explicit	Client(int fd);
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

	~Client();
};
