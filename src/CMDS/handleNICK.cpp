#include "../../include/Server.hpp"

void	Server::handleNICK(Client &c, const std::vector<std::string> &params)
{
	(void) c; //mkuida
	const std::string newNick = params[0];

	// こんな感じでparamsからとっていく...
}
