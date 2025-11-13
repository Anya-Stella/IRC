#include "../include/Utils.hpp"
#include <cstdlib>
#include <climits>

/* validation */
int validatePort(const char* arg)
{
	if (!arg || *arg == '\0')
		return -1;

	char *endptr = 0;
	long port = std::strtol(arg, &endptr, 10);

	if (*endptr != '\0' || port < 1 || port > 65535)
		return -1;

	return static_cast<int>(port);
}

bool validatePassword(const std::string& password)
{
	if (password.empty())
		return false;
	if (password.size() > 512)
		return false;
	return true;
}

