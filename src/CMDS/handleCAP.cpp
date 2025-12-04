#include "Server.hpp"

void	Server::handleCAP(Client &c, const std::vector<std::string> &params)
{
	if (params.empty())
		return ;
	
	const std::string	&subcmd = params[0];
	std::string nick = c.getNickname();
	if (nick.empty())
		nick = "*";
	
	// base
	if (subcmd == "LS")
	{
		c.sendMessage(":" + _server_name + " CAP " + nick + " LS :");

		return ;
	}

	if (subcmd == "REQ")
	{
		std::string	caps;
		if (params.size() >= 2)
			caps = params[1];
		
		c.sendMessage(":" + _server_name + " CAP " + nick + " NAK :" + caps);
	}
	
	if (subcmd == "END")
		return ;
}
