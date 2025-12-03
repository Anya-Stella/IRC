#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

// sender を除外してチャンネル全員にメッセージ送信
void Server::broadcastToChannel(Channel& ch, const std::string& message, Client* sender)
{
    const std::map<int, Client*>& members = ch.getClients(); // チャンネルのメンバー一覧
    for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (sender && it->second->getNickname() == sender->getNickname())
            continue; // 送信者には送らない

        it->second->sendMessage(message);
    }
}

// サーバークラス内に追加
Client* Server::findClientByNick(const std::string& nickname)
{
    // _clients: map<int fd, Client*> に入っている全クライアントを走査
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        Client* c = it->second;
        // ニックネーム比較（大文字小文字を無視するため toUpperCaseString を使う）
        if (toUpperCaseString(c->getNickname()) == toUpperCaseString(nickname))
        {
            return c; // 一致したクライアントを返す
        }
    }
    // どのクライアントも一致しなかった場合
    return nullptr;
}

void Server::handlePRIVMSG(Client& sender, const std::vector<std::string>& params)
{
    // 1. 引数チェック
    if (params.empty()) {
        sender.sendMessage("411 :No recipient given\r\n");
        return;
    }
    if (params.size() < 2) {
        sender.sendMessage("412 :No text to send\r\n");
        return;
    }

    const std::string& target = params[0];  // 受信者
    const std::string& message = params[1]; // 本文

    if (target[0] == '#') {
        // チャンネル宛
        if (_channels.find(target) == _channels.end()) {
            sender.sendMessage("403 " + target + " :No such channel\r\n");
            return;
        }
        Channel* ch = _channels[target];
        if (!ch->hasClient(&sender)) {
            sender.sendMessage("404 " + target + " :Cannot send to channel\r\n");
            return;
        }
        broadcastToChannel(*ch, ":" + sender.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n", &sender);
    } else {
        // ユーザー宛
        Client* recipient = findClientByNick(target);
        if (!recipient) {
            sender.sendMessage("401 " + target + " :No such nick\r\n");
            return;
        }
        recipient->sendMessage(":" + sender.getNickname() + " PRIVMSG " + target + " :" + message + "\r\n");
    }
}
