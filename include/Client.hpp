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
	std::string _buff;

	int _fd;

	std::string	_nickname;
	// ...
	


public:
	// とりあえずこれだけ
	explicit	Client(int fd);
	std::string& getBuff( void ) const;
	std::string& setBuff( void );

	~Client();
};
