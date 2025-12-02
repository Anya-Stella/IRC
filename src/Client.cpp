#include "../include/Client.hpp"


/* default */

Client::Client(int fd) : _fd(fd)
{
	(void) _fd;
}

Client::~Client(){}

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
