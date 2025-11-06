#pragma once

class Server
{
private:
	/* data */
	int						_listen_fd; // This is set up as soon as the Server instance is created.
	std::vector<pollfd>		_poll_fds; // fd array waiting for the poll().
	std::map<int, Client>	_clients;
	std::string				_password;

	/* utils */
	// ...




public:
	/* default */
	Server(int port, std::string &password);
	~Server();

	/* method */
	void	run();
};
