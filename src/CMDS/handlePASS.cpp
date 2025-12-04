#include "../../include/Server.hpp"

void Server::sendWelcome(Client &c)
{
    std::string nick = c.getNickname();

    c.sendMessage(":server 001 " + nick + " :Welcome to the IRC Network\r\n");
    c.sendMessage(":server 002 " + nick + " :Your host is server\r\n");
    c.sendMessage(":server 003 " + nick + " :This server was created ...\r\n");
}

void Server::handlePASS(Client &c, const std::vector<std::string> &params)
{
    // PASS に必要なパラメータが不足
    if (params.empty())
    {
        c.sendMessage("461 PASS :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
        return;
    }

    // すでにPASS認証済み（PASSを二回送った時）
    if (c.isPassAccepted())
    {
        c.sendMessage("462 :You may not reregister\r\n"); // ERR_ALREADYREGISTRED
        return;
    }

    const std::string &pass = params[0];

    // パスワードが一致しない
    if (pass != this->_password)
    {
        c.sendMessage("464 :Password incorrect\r\n"); // ERR_PASSWDMISMATCH
        return;
    }

    // PASS 成功！
    c.setPassAccepted(true);

    // ここで NICK + USER + PASS が揃っていれば WELCOME を送る
    if (c.tryToRegister())
        sendWelcome(c);
}
