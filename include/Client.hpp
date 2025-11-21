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
	std::string _nick;
	bool _passAccepted;    // PASS コマンド受理済みか
    bool _hasNick;         // NICK コマンド受理済みか
    bool _hasUser;         // USER コマンド受理済みか
	// ここはコマンドとか実装してくに当たって必要そうなのを入れてけばいいと思う
	std::string	_nickname;


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
	/*USER*/
	bool hasNick() const {
        return !_nickname.empty();
    }
	void setNick(std::string n) {
		_nickname = n;
	}
	~Client();
};
