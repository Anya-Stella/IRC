#include "../include/Server.hpp"
#include "../include/Utils.hpp"
#include <stdio.h>
#include <cmath>
#include <string>

int	main(int argc, char **argv)
{
	/*　input validation */ 
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>\n";
		return 1;
	}

	/* validate parameters */
	int port = validatePort(argv[1]);
	if (port == -1)
	{
		std::cerr << "Error: invalid port number (must be 1-65535)\n";
		return 1;
	}

	std::string password = argv[2];
	if (!validatePassword(password))
	{
		std::cerr << "Error: invalid password (length 1-512 required)\n";
		return 1;
	}

	/*.generate and move server */
	try
	{
		Server	MainServer(port, password);
		MainServer.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error : " << e.what() << std::endl; 
	}
	return 0;
}
