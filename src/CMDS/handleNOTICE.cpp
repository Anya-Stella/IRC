#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleNOTICE(Client& sender, const std::vector<std::string>& params)
{
    // 1. 引数チェック
    if (params.size() < 2)
        return; // NOTICE はエラー返信しないので無視

    const std::string& target = params[0]; // 宛先（ユーザー名 or チャンネル名）
    const std::string& message = params[1]; // メッセージ内容

    // 2. チャンネル宛の場合
    if (!target.empty() && target[0] == '#') {
        Channel* ch = _channels[target];
        if (!ch) return; // チャンネルが存在しない場合は無視
        broadcastToChannel(*ch, ":" + sender.getNickname() + " NOTICE " + target + " :" + message + "\r\n", &sender);
    }
    // 3. ユーザー宛の場合
    else {
        Client* recipient = findClientByNick(target);
        if (!recipient) return; // ユーザーがいなければ無視
        recipient->sendMessage(":" + sender.getNickname() + " NOTICE " + target + " :" + message + "\r\n");
    }
}
