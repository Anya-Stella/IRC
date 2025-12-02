#pragma once
#include <string>
#include <cstring>

#define CLIENT_MAXBUFF 8192	// mkuida
#define CMD_MAXBUFF 512		// mkuida

// ここはクライアント側の状態のみを扱う
// 


class Client
{
private:
	/* data */
	// ここはコマンドとか実装してくに当たって必要そうなのを入れてけばいいと思う
	std::string _buff;	//mkuida
	std::string _cmd;	//mkuida

	int _fd;

	std::string	_nickname;
	// ...



public:
	// とりあえずこれだけ
	explicit	Client(int fd);

	const std::string& getBuff( void ) const;	//mkuida
	void setBuff( const std::string& str );		//mkuida
	const std::string& getCmd( void ) const;	//mkuida
	void setCmd( const std::string& str );		//mkuida
	bool devBuff(const char* str);				//mkuida
	std::string& adjBuff();						//mkuida

	~Client();
};
