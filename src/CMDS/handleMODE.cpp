#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include <sstream>

// MODE コマンドハンドラ
void Server::handleMODE(Client& sender, const std::vector<std::string>& params)
{
    // --- 1. 引数チェック ---
    if (params.size() < 2) {
        sender.sendMessage("461 MODE :Not enough parameters\r\n");
        return;
    }

    const std::string& channelName = params[0];
    const std::string& modeStr = params[1];
    std::string targetUser; // +o/-o の場合に指定されるユーザー
    if (params.size() > 2)
        targetUser = params[2];

    // --- 2. チャンネル存在チェック ---
    if (_channels.find(channelName) == _channels.end()) {
        sender.sendMessage("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Channel* channel = _channels[channelName];

    // --- 3. 参加確認 ---
    if (!channel->hasClient(&sender)) {
        sender.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // --- 4. 権限チェック (OPでなければ変更不可) ---
    if (!channel->isOperator(sender.getFd())) {
        sender.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // --- 5. モード処理 ---
    for (size_t i = 0; i < modeStr.size(); i++) {
    char c = modeStr[i];

    switch (c) {
        case '+': /* ON にするフラグの開始 */ break;
        case '-': /* OFF にするフラグの開始 */ break;
        case 'i': channel->setInviteOnly(true); break;
        case 't': channel->setTopicProtected(true); break;
        case 'k':
            if (!targetUser.empty()) 
                channel->setKey(targetUser);
            break;
        case 'l':
            if (!targetUser.empty()) {
                std::stringstream ss(targetUser);
                int limit;
                ss >> limit;
                channel->setUserLimit(limit);
            }
            break;
        case 'o': {
            Client* target = findClientByNick(targetUser);
            if (target != NULL) 
                channel->addOperator(target->getFd());
            break;
        }
        default:
            sender.sendMessage("472 " + std::string(1, c) + " :Unknown mode char\r\n");
    }
}


    // --- 6. チャンネル内全員に通知 ---
    broadcastToChannel(*channel, ":" + sender.getNickname() + " MODE " + channelName + " " + modeStr +
                       (targetUser.empty() ? "" : " " + targetUser) + "\r\n");
}
