#pragma once
#include <string>

// ここはクライアント側の状態のみを扱う
// 


class Client
{
private:
	/* data */
	std::string	NickName;
	//  所属しているチャンネルリスト


public:
	Client(/* args */);
	~Client();
};
