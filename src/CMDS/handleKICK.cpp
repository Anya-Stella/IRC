#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleKICK(Client& sender, const std::vector<std::string>& params)
{
    // 1. 引数チェック
    if (params.size() < 2) {
        sender.sendMessage("461 KICK :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
        return;
    }

    const std::string& channelName = params[0];
    const std::string& targetNick = params[1];
    const std::string comment = (params.size() > 2) ? params[2] : targetNick;

    // 2. チャンネル存在確認
    if (_channels.find(channelName) == _channels.end()) {
        sender.sendMessage("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Channel* channel = _channels[channelName];

    // 3. 実行者がオペレーターか確認
    if (!channel->isOperator(&sender)) {
        sender.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // 4. 対象ユーザーがチャンネルに参加しているか確認
    Client* targetClient = findClientByNick(targetNick);
    if (!targetClient || !channel->hasClient(targetClient)) {
        sender.sendMessage("441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }

    // 5. KICK通知をチャンネル全員に送信
    std::string msg = ":" + sender.getPrefix() + " KICK " + channelName + " " + targetNick + " :" + comment + "\r\n";
    broadcastToChannel(*channel, msg, NULL);

    // 6. チャンネルから削除
    channel->removeClient(targetClient);
    targetClient->leaveChannel(channelName);

    // 7. 空のチャンネルは削除
    if (channel->isEmpty()) {
        _channels.erase(channelName);
        delete channel;
    }
}
