#include "../../include/Server.hpp"

void	Server::handleUSER(Client &c, const std::vector<std::string> &params)
{
	// USER に必要なパラメータが不足
	if(params.empty())
	{
		c.sendMessage("461 USER :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
        return;
	}
	// こんな感じでparamsからとっていく...
}