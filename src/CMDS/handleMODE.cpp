#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include <sstream>
#include <cstdlib>


// MODE コマンドハンドラ（チャンネルモード）
void Server::handleMODE(Client& sender, const std::vector<std::string>& params)
{
    // ---- 1. パラメータ不足チェック ----
    if (params.size() < 2) {
        sender.sendMessage("461 MODE :Not enough parameters\r\n");
        return;
    }

    const std::string& channelName = params[0];
    const std::string& modeStr = params[1];

    // ---- 2. チャンネル存在チェック ----
    if (_channels.find(channelName) == _channels.end()) {
        sender.sendMessage("403 " + channelName + " :No such channel\r\n");
        return;
    }

    Channel* channel = _channels[channelName];

    // ---- 3. チャンネルメンバーか確認 ----
    if (!channel->hasClient(&sender)) {
        sender.sendMessage("442 " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // ---- 4. OP（操作権限）チェック ----
    if (!channel->isOperator(sender.getFd())) {
        sender.sendMessage("482 " + channelName + " :You're not channel operator\r\n");
        return;
    }

    // ---- 5. MODE パーサー準備 ----
    bool adding = true;  // + = true, - = false
    int paramIndex = 2;  // params[2] 以降に key / limit / nick が入る

    // ---- 6. MODE 文字列解析 ----
    for (size_t i = 0; i < modeStr.size(); i++) {
        char m = modeStr[i];

        // + または - の場合はフラグ変更
        if (m == '+') { adding = true; continue; }
        if (m == '-') { adding = false; continue; }

        switch (m)
        {
        // +i / -i : 招待制チャンネル切り替え
        case 'i':
            channel->setInviteOnly(adding);
            break;

        // +t / -t : トピック保護（OP のみ変更可）
        case 't':
            channel->setTopicProtected(adding);
            break;

        // +k <key> / -k : チャンネルキー設定・解除
        case 'k':
            if (adding) {
                // +k は追加パラメータが必要
                if (params.size() <= static_cast<size_t>(paramIndex)) break;
                channel->setKey(params[paramIndex++]);
            } else {
                // -k はキーをクリア
                channel->setKey("");
            }
            break;

        // +l <limit> / -l : ユーザー上限の設定・解除
        case 'l':
            if (adding) {
                if (params.size() <= static_cast<size_t>(paramIndex)) break;

                int limit = atoi(params[paramIndex++].c_str());
                if (limit > 0)
                    channel->setUserLimit(limit);
            } else {
                // -l は上限解除 → 無制限
                channel->setUserLimit(0);
            }
            break;

        // +o <nick> / -o <nick> : OP を付与・剥奪
        case 'o': {
            if (params.size() <= static_cast<size_t>(paramIndex)) break;

            std::string nick = params[paramIndex++];
            Client* target = findClientByNick(nick);

            if (!target) break;

            if (adding)
                channel->addOperator(target->getFd());
            else
                channel->removeOperator(target->getFd());

            break;
        }

        // 不明なモード文字
        default:
            sender.sendMessage("472 " + std::string(1, m) + " :Unknown MODE flag\r\n");
            break;
        }
    }

    // ---- 7. MODE 設定が終わったのでチャンネル全員に通知 ----
    std::string msg = ":" + sender.getNickname() + " MODE " + channelName + " ";

    // モード + パラメータをまとめて送信
    for (size_t i = 1; i < params.size(); i++)
        msg += params[i] + " ";

    msg += "\r\n";

    broadcastToChannel(*channel, msg);
}
