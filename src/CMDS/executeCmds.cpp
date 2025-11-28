#include <string>
#include "../../include/Server.hpp"

void	Server::executeCmds(Client &c, const ParsedMessage &msg)
{
    (void)  c;

    if (msg.command == "NICK")
    {
        handleNICK(c, msg.params);
    }
    else if (msg.command == "PASS")
    {
        handlePASS(c, msg.params);
    }
    else if (msg.command == "USER")
    {
        handleUSER(c, msg.params);
    }
    else if (msg.command == "PING")
    {
        handlePING(c, msg.params);
    }
    else if (msg.command == "PONG")
    {
        handlePONG(c, msg.params);
    }
    else if (msg.command == "JOIN")
    {
        handleJOIN(c, msg.params);
    }
    else if (msg.command == "PART")
    {
        handlePART(c, msg.params);
    }
    else if (msg.command == "KICK")
    {
        handleKICK(c, msg.params);
    }
    else if (msg.command == "QUIT")
    {
        handleQUIT(c, msg.params);
    }
    else
    {
        return;
    }
}
