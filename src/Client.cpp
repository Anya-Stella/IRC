#include "../include/Client.hpp"


/* default */

Client::Client(int fd) : _fd(fd), _passAccepted(false), _hasNick(false), _hasUser(false), _registered(false),
_nickname("unknown"), _username("unknown"), _realname("unknown"), _lastPongTime(0)
{
	(void) _fd;
}
/*ニックネーム*/
std::string Client::getNickname() const {
    return _nickname;
}
/*メッセージ表示*/
void Client::sendMessage(const std::string &msg) {
    std::cout << "Message to " << _nickname << ": " << msg;
}
/*PASS*/
bool Client::isPassAccepted() const { return _passAccepted; }
void Client::setPassAccepted(bool v) { _passAccepted = v; }

// 完全登録済み判定
bool Client::isFullyRegistered() const {
    // PASS受理済 + NICK受理済 + USER受理済
    return _passAccepted && _hasNick && _hasUser;
}

//登録してもいいかの確認
bool Client::readyToRegister() const {
	return isFullyRegistered() && !_registered;
}

//登録の処理
void Client::setRegistered(bool status) {
	_registered = status;
}

/*PONG*/
Client::~Client(){}