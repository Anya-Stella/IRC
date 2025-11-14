#pragma once
#include <string>

// ここはクライアント側の状態のみを扱う
// 


class Client
{
private:
	/* data */
	// ここはコマンドとか実装してくに当たって必要そうなのを入れてけばいいと思う

	int _fd;

	std::string	_nickname;
	// ...



public:
	// とりあえずこれだけ
	explicit	Client(int fd);

	~Client();
};
