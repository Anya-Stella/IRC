#include "../../include/Server.hpp"

void Server::handlePONG(Client &c, const std::vector<std::string> &params)
{
	(void)params;
    // パラメータは特に使わない
    c.updatePongTime();
    
    // 任意: ログ表示
    std::cout << "[PONG] " << c.getNickname() << " responded." << std::endl;
}
