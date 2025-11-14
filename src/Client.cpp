#include "../include/Client.hpp"


/* default */

Client::Client(int fd) : _fd(fd)
{
	(void) _fd;
}

Client::~Client(){}