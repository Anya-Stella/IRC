#include "../../include/Server.hpp"

void	Server::handleNICK(Client &c, const std::vector<std::string> &params)
{
	// NICK に必要なパラメータが不足
	if(params.empty())
	{
		c.sendMessage("461 NICK :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
        return;
	}
	const std::string newNick = params[0];
	// こんな感じでparamsからとっていく...
}
