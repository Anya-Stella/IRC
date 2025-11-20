#include "../include/Client.hpp"


/* default */

Client::Client(int fd) : _fd(fd), _nick("kosakats"), _passAccepted(false), _hasNick(false), _hasUser(false)
{
	(void) _fd;
}
/*ニックネーム*/
std::string Client::getNickname() const {
    return _nick;
}
/*メッセージ表示*/
void Client::sendMessage(const std::string &msg) {
    std::cout << "Message to " << _nick << ": " << msg;
}
/*PASS*/
bool Client::isPassAccepted() const { return _passAccepted; }
void Client::setPassAccepted(bool v) { _passAccepted = v; }

// 完全登録済み判定
bool Client::isFullyRegistered() const {
    // PASS受理済 + NICK受理済 + USER受理済
    return _passAccepted && _hasNick && _hasUser;
}

Client::~Client(){}