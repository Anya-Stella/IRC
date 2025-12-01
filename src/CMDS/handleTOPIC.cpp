#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleTOPIC(Client& c, const std::vector<std::string>& params)
{
    // 1. パラメータ不足
    if (params.empty()) {
        c.sendMessage("461 TOPIC :Not enough parameters\r\n");
        return;
    }

    const std::string channelName = params[0];

    // 2. チャンネルが存在するか
    if (_channels.count(channelName) == 0) {
        c.sendMessage("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Channel* channel = _channels[channelName];

    // 3. 参加しているか
    if (!channel->hasClient(&c)) {
        c.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // 4. トピック取得だけの場合（TOPIC #chan）
    if (params.size() == 1) {

        if (channel->getTopic().empty()) {
            // トピック未設定
            c.sendMessage("331 " + channelName + " :No topic is set\r\n");
        } else {
            // トピックあり
            c.sendMessage("332 " + channelName + " :" + channel->getTopic() + "\r\n");
        }

        return;
    }

    // 5. トピック変更（TOPIC #chan :new topic）
    // MODE +t の場合は OP チェックが必要
    if (channel->isTopicProtected() && !channel->isOperator(&c)) {
        c.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // 変更内容（:がついている部分）
    std::string newTopic = params[1];
    if (!newTopic.empty() && newTopic[0] == ':')
        newTopic.erase(0, 1); // ':' 取り除く

    // トピック更新
    channel->setTopic(newTopic);

    // 6. 他のメンバー全員に通知
    std::string msg = ":" + c.getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    broadcastToChannel(*channel, msg);

    return;
}
