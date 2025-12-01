#pragma once
#include <string>

#define CLIENT_MAXBUFF 8192 // mkuida
#define CMD_MAXBUFF 512

// ここはクライアント側の状態のみを扱う
// 


class Client
{
private:
	/* data */
	// ここはコマンドとか実装してくに当たって必要そうなのを入れてけばいいと思う
	std::string _buff;	//m
	std::string _cmd;	//m

	int _fd;

	std::string	_nickname;
	// ...
	


public:
	// とりあえずこれだけ
	explicit	Client(int fd);
	const std::string& getBuff( void ) const;	//mkuida
	void setBuff( const std::string& str );	//mkuida
	const std::string& getCmd( void ); //mkuida
	void setCmd( const std::string& str ) const;	//mkuida
	bool devBuff(const char* str);	//mkuida

	~Client();
};

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

