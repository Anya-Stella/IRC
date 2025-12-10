#include "../../include/Server.hpp"

void 	Server::handlePING(Client &c, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		// RFC 2812 specifies 461 for not enough parameters.
    	c.sendMessage("461 PING :Not enough parameters\r\n");
    	return;
	}
	 // 引数1つ目だけ使用（2つ以上来ても無視）
	const std::string &token = params[0];
	// The standard PONG reply includes the server name.
	c.sendMessage("PONG " + _server_name + " :" + token + "\r\n");
}
