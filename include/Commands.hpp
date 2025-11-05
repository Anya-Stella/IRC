#pragma once

#include "Server.hpp"
#include "Client.hpp"

// we use this like a Library.
namespace Command
{
	/*.dispatch */
	void dispatch(Server&, Client&);

	/*.commands */
	void pass(Server&, Client&);
	void nick(Server&, Client&);
	void user(Server&, Client&);
	void join(Server&, Client&);
	void privmsg(Server&, Client&);
	
	//....
}
