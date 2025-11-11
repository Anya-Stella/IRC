#include "../include/Server.hpp"
#include <stdio.h>
#include <cmath>
#include <string>




int	main(int argc, char **argv)
{
	/*　input validation */ 
	if (argc != 3)
	{
		//TODO:kokoyaru
	}

	/*.define parameters */
	int port = atoi(argv[1]);
	std::string	password = argv[2];

	/*.generate and move server */
	Server	MainServer(port, password);
	MainServer.run();

	return (0);
}
