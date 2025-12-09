#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include <sstream>
#include <cstdlib>


// MODE コマンドハンドラ（チャンネルモード）
void Server::handleMODE(Client& sender, const std::vector<std::string>& params)
{
    if (params.empty()) {
        sender.sendMessage("461 MODE :Not enough parameters\r\n");
        return;
    }
    const std::string& target = params[0];
    bool isChannel = !target.empty() && (target[0] == '#' || target[0] == '&');
    if (isChannel) {
        if (params.size() < 2) {
            sender.sendMessage("461 MODE :Not enough parameters\r\n");
            return;
        }
        const std::string& channelName = params[0];
        const std::string& modeStr = params[1];
        if (_channels.find(channelName) == _channels.end()) {
            sender.sendMessage("403 " + channelName + " :No such channel\r\n");
            return;
        }
        Channel* channel = _channels[channelName];
        if (!channel->hasClient(&sender)) {
            sender.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
            return;
        }
        if (!channel->isOperator(sender.getFd())) {
            sender.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
            return;
        }
        bool adding = true;
        int paramIndex = 2;
        for (size_t i = 0; i < modeStr.size(); i++) {
            char m = modeStr[i];
            if (m == '+') { adding = true; continue; }
            if (m == '-') { adding = false; continue; }
            switch (m) {
            case 'i': channel->setInviteOnly(adding); break;
            case 't': channel->setTopicProtected(adding); break;
            case 'k':
                if (adding) {
                    if (params.size() <= static_cast<size_t>(paramIndex)) {
                        sender.sendMessage("461 MODE :Not enough parameters for +k\r\n");
                        break;
                    }
                    std::string key = params[paramIndex++];
                    if (!key.empty())
                        channel->setKey(key);
                } else {
                    channel->setKey("");
                }
                break;
            case 'l':
                if (adding) {
                    if (params.size() <= static_cast<size_t>(paramIndex)) break;
                    int limit = atoi(params[paramIndex++].c_str());
                    if (limit > 0) channel->setUserLimit(limit);
                } else {
                    channel->setUserLimit(0);
                }
                break;
            case 'o': {
                if (params.size() <= static_cast<size_t>(paramIndex)) break;
                std::string nick = params[paramIndex++];
                Client* targetClient = findClientByNick(nick);
                if (!targetClient) break;
                if (adding)
                    channel->addOperator(targetClient->getFd());
                else
                    channel->removeOperator(targetClient->getFd());
                break;
            }
            default:
                sender.sendMessage("472 " + std::string(1, m) + " :Unknown MODE flag\r\n");
                break;
            }
        }
        std::string msg = ":" + sender.getNickname() + " MODE " + channelName + " ";
        for (size_t i = 1; i < params.size(); i++)
            msg += params[i] + " ";
        msg += "\r\n";
        broadcastToChannel(*channel, msg);
    } else {
        // ユーザーモード: 自分自身のみ許可
        if (target != sender.getNickname()) {
            sender.sendMessage("502 :Cannot change mode for other users\r\n");
            return;
        }
        // ユーザーモードの応答（現状は何も変更しない）
        sender.sendMessage(":" + sender.getNickname() + " MODE " + sender.getNickname() + "\r\n");
    }
}
