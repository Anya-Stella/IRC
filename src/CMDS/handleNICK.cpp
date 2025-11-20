#include "../../include/Server.hpp"

void	Server::handleNICK(Client &c, const std::vector<std::string> &params)
{
	(void) c;
	const std::string newNick = params[0];
	std::cout << "OK" << std::endl;
	// こんな感じでparamsからとっていく...
}
