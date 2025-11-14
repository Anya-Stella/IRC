#include <string>
#include "../../include/Server.hpp"

void	Server::executeCmds(Client &c, ParsedMessage &msg)
{
    (void)  c;


    if (msg.command == "NICK")
        handleNICK();
    else if (msg.command == "PASS")
    {

    }
    else
    {

    }
}
