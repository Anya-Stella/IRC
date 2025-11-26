#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handlePART(Client& c, const std::vector<std::string>& params)
{
    // 1. 引数チェック
    if (params.empty()) {
        c.sendMessage("461 PART :Not enough parameters\r\n");
        return;
    }

    // 2. チャンネル名ごとに処理
    std::vector<std::string> channelNames = splitChannels(params[0]); // ',' で分割
    for (size_t i = 0; i < channelNames.size(); ++i) {
        const std::string& channelName = channelNames[i];

        // 3. チャンネル存在確認
        if (_channels.find(channelName) == _channels.end()) {
            c.sendMessage("403 " + channelName + " :No such channel\r\n");
            continue;
        }

        Channel* channel = _channels[channelName];

        // 4. クライアントが参加しているか確認
        if (!channel->hasClient(&c)) {
            c.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
            continue;
        }

        // 5. 退出処理
        channel->removeClient(&c);
        c.partChannel(channelName);

        // 6. 退出通知を送信
        broadcastToChannel(*channel, ":" + c.getNickname() + " PART :" + channelName + "\r\n");

        // 7. チャンネルが空なら削除
        if (channel->isEmpty()) {
            _channels.erase(channelName);
            delete channel;
        }
    }
}

