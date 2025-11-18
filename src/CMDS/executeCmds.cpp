#include <string>
#include "../../include/Server.hpp"

void	Server::executeCmds(Client &c, const ParsedMessage &msg)
{
    (void)  c;


    if (msg.command == "NICK")
        handleNICK(c, msg.params);
    else if (msg.command == "PASS")
    {
        handlePASS(c, msg.params);
    }
    else
    {

    }
}
