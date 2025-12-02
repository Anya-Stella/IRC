#pragma once
#include <string>
#include "Server.hpp"

struct ParsedMessage;

/* validation */
int		validatePort(const char* arg);
bool	validatePassword(const std::string& password);
void	printPmsg(const ParsedMessage& p);