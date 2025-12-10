#include "../../include/Server.hpp"
#include "../../include/Client.hpp"

void Server::handlePONG(Client &c, const std::vector<std::string> &params)
{
	(void)params;
    // パラメータは特に使わない
    // This correctly updates the client's last activity time.
    c.updatePongTime();
    
    // 任意: ログ表示
    std::cout << "[PONG] " << c.getNickname() << " responded." << std::endl;
}
