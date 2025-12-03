#include "../include/Client.hpp"


/* default */
Client::Client(int fd)
	:
	_fd(fd),
	_nickname("unknown"),
	_username("unknown"),
	_realname("unknown"),
	_lastPongTime(0),
	_passAccepted(false),
	_hasNick(false),
	_hasUser(false),
	_registered(false){}

Client::~Client(){}

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

/* cmd */
const std::string& Client::getBuff() const	//mkuida
{
	return ( this->_buff);
}

void Client::setBuff( const std::string& str )	//mkuida
{
	this->_buff = str;
}

const std::string& Client::getCmd() const	//mkuida
{
	return ( this->_cmd );
}

void Client::setCmd( const std::string& str )	//mkuida
{
	this->_cmd = str;
}

std::string& Client::adjBuff()	//mkuida
{
	return (this->_buff);
}

bool Client::devBuff(const char* endstr)	//mkuida
{
	size_t pos = getBuff().find(endstr);
	if(pos != std::string::npos)
	{
		size_t endstr_len = std::strlen(endstr);
		this->setCmd(getBuff().substr(0, pos));
		std::string tmp = getBuff().substr(pos + endstr_len);
		this->setBuff(tmp);
		return (true);
	}
	return (false);
}

/* register */
bool Client::isFullyRegistered() const {
    // PASS受理済 + NICK受理済 + USER受理済
    return _passAccepted && _hasNick && _hasUser;
}

bool Client::readyToRegister() const {
	return isFullyRegistered() && !_registered;
}

void Client::setRegistered(bool status) {
	_registered = status;
}
