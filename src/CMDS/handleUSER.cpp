#include "../../include/Server.hpp"

void Server::handleUSER(Client &c, const std::vector<std::string> &params)
{
    // 1. パラメータ不足
    if (params.size() < 4)
    {
        c.sendMessage("461 USER :Not enough parameters\r\n");
        return;
    }

    // 2. すでに登録済みならエラー
    if (c.isFullyRegistered())
    {
        c.sendMessage("462 USER :You may not reregister\r\n");
        return;
    }

    // 3. USER 情報を登録
    std::string username = params[0];
    std::string realname = params[3];

    c.setUsername(username);
    c.setRealname(realname);
    c.setHasUser(true);

    // 4. 登録完了チェック
    if (c.tryToRegister())
        sendWelcome(c);

    std::cout << "USER CMD DONE" << std::endl;
}
