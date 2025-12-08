#pragma once
#include <string>
#include "Server.hpp"
#include <vector>
#include <string>
#include <sstream>

struct ParsedMessage;

/* validation */
int		validatePort(const char* arg);
bool	validatePassword(const std::string& password);
void	printPmsg(const ParsedMessage& p);
std::vector<std::string> split(const std::string& s, char delimiter);