#include "../../include/Server.hpp"

void 	Server::handlePING(Client &c, const std::vector<std::string> &params)
{
	if (params.empty())
	{
    	c.sendMessage("409 PING :No origin specified\r\n"); // ERR_NEEDMOREPARAMS
    	return;
	}
	 // 引数1つ目だけ使用（2つ以上来ても無視）
	const std::string &token = params[0];
	c.sendMessage("PONG :" + token + "\r\n");
}
