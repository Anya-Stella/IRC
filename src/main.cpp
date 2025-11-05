#include <stdio.h>
#include <cmath>
#include <string>




int	main(int argc, char **argv)
{
	/*.input validation */ 
	if (argc != 3)
	{

	}

	/*.define parameters */
	int port = atoi(argv[1]);
	std::string	password = argv[2];

	/*.generate and move server */
	// Server	MainServer(port, password);
	// server.run();

	return (0);
}
