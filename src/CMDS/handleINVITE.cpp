#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleINVITE(Client& sender, const std::vector<std::string>& params)
{
    // 1. パラメータチェック
    // INVITE <nickname> <channel>
    if (params.size() < 2) {
        sender.sendMessage("461 INVITE :Not enough parameters\r\n");
        return;
    }

    const std::string& targetNick  = params[0]; // 招待する相手
    const std::string& channelName = params[1];

    // 2. 対象ユーザーが存在するか確認
    Client* target = findClientByNick(targetNick);
    if (!target) {
        sender.sendMessage("401 " + targetNick + " :No such nick\r\n");
        return;
    }

    // 3. チャンネルが存在するか確認
    if (_channels.count(channelName) == 0) {
        sender.sendMessage("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Channel* channel = _channels[channelName];

    // 4. 招待者がチャンネルに在籍しているか
    if (!channel->hasClient(&sender)) {
        sender.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // 5. チャンネルが招待制なら OP かチェック
    //    MODE +i を実装している場合に必要
    if (channel->isInviteOnly() && !channel->isOperator(&sender)) {
        sender.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // 6. ターゲットがすでにチャンネルにいるか
    if (channel->hasClient(target)) {
        sender.sendMessage("443 " + targetNick + " " + channelName +
                           " :is already on channel\r\n");
        return;
    }

    // 7. 招待リストに登録（後で JOIN 時に認めるため）
    channel->addInvite(target->getFd());

    // 8. 招待成功メッセージを sender に送信（RFC 準拠）
    sender.sendMessage("341 " + sender.getNickname() + " " +
                       targetNick + " " + channelName + "\r\n");

    // 9. 招待対象ユーザーに INVITE メッセージを送信
    target->sendMessage(":" + sender.getNickname() +
                        " INVITE " + targetNick + " :" + channelName + "\r\n");
}
