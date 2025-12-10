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

        std::string appliedModes;
        std::string appliedParams;

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
                        continue;
                    }
                    std::string key = params[paramIndex++];
                    channel->setKey(key);
                    appliedModes += "+k";
                    appliedParams += " " + key;
                } else {
                    channel->setKey("");
                    appliedModes += "-k";
                    // -k は引数を取るべきではないが、もしクライアントが誤って指定した場合、
                    // その引数が後続のモードに影響を与えないように消費する。
                    if (params.size() > static_cast<size_t>(paramIndex)) {
                        appliedParams += " " + params[paramIndex++];
                    }
                }
                break;
            case 'l':
                if (adding) {
                    if (params.size() <= static_cast<size_t>(paramIndex)) {
                        sender.sendMessage("461 MODE :Not enough parameters for +l\r\n");
                        continue;
                    }
                    const std::string& limitStr = params[paramIndex++];
                    // パラメータが有効な数値であるか検証
                    bool isValidNumber = true;
                    for (size_t j = 0; j < limitStr.length(); ++j) {
                        if (!isdigit(limitStr[j])) {
                            isValidNumber = false;
                            break;
                        }
                    }
                    if (!isValidNumber) {
                        sender.sendMessage("472 " + limitStr + " :is not a valid limit for +l\r\n");
                        continue;
                    }
                    int limit = atoi(limitStr.c_str());
                    if (limit <= 0) { // 制限値は正の数である必要がある
                        sender.sendMessage("472 " + limitStr + " :Limit must be a positive number for +l\r\n");
                        continue;
                    }
                    channel->setUserLimit(static_cast<size_t>(limit));
                    appliedModes += "+l";
                    appliedParams += " " + limitStr;
                } else {
                    channel->setUserLimit(0);
                    appliedModes += "-l";
                    // -l は引数を取らないが、もしクライアントが誤って指定した場合、
                    // その引数が後続のモードに影響を与えないように消費する。
                    if (params.size() > static_cast<size_t>(paramIndex)) {
                        paramIndex++; // 余分なパラメータを読み飛ばす
                    }
                }
                break;
            case 'o': {
                if (params.size() <= static_cast<size_t>(paramIndex)) {
                    sender.sendMessage("461 MODE :Not enough parameters for +/-o\r\n");
                    continue; // 次のモード文字へ
                }
                std::string nick = params[paramIndex++];
                Client* targetClient = findClientByNick(nick);

                // ターゲットユーザーがサーバーに存在しない、またはチャンネルにいない場合
                if (!targetClient || !channel->hasClient(targetClient)) {
                    sender.sendMessage("441 " + nick + " " + channelName + " :They aren't on that channel\r\n");
                    continue; // 次のモード文字へ
                }

                // 権限を付与または剥奪
                if (adding) {
                    channel->addOperator(targetClient->getFd());
                } else {
                    channel->removeOperator(targetClient->getFd());
                }

                // 成功したモードとパラメータを記録
                appliedModes += (adding ? '+' : '-');
                appliedModes += 'o';
                appliedParams += " " + nick;
                break;
            }
            default:
                sender.sendMessage("472 " + std::string(1, m) + " :Unknown MODE flag\r\n");
                continue;
            }
        }
        // 実際に適用されたモード変更があった場合のみ、ブロードキャストする
        if (!appliedModes.empty()) {
            std::string msg = ":" + sender.getNickname() + " MODE " + channelName + " " + appliedModes + appliedParams + "\r\n";
            broadcastToChannel(*channel, msg, NULL);
        }
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
