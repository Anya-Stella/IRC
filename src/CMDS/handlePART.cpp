#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

#include <vector>
#include <string>
#include <set>
#include <algorithm>  // remove_if 用
#include <cctype>     // isspace, isalnum 用
#include <sstream>    // stringstream 用

std::string Server::trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// "A,B,C" → {"A", "B", "C"} に分割する関数
std::vector<std::string> Server::splitChannels(const std::string& param)
{
    std::vector<std::string> result;
    std::set<std::string> seen; // 重複排除用（#a,#b,#a → 重複の #a を無視する。）

    std::stringstream ss(param);
    std::string raw;

    while (std::getline(ss, raw, ',')) {
        // --- 1. 前後空白を除去 ---
        std::string ch = trim(raw);
        if (ch.empty()) continue;

        // --- 2. 途中にある余分な空白も除去 (#a   → #a) ---
        //   （RFCは途中の空白を許可しないので消して正規化）
        //  erase は文字列から指定範囲を削除する関数
        //  std::remove_if はアルゴリズムの一種で 「条件を満たす要素を末尾に寄せる」
        ch.erase(std::remove_if(ch.begin(), ch.end(), ::isspace), ch.end());

        // --- 3. '#' が無い場合は補う
        if (ch[0] != '#')
            ch = "#" + ch;

        // --- 4. チャンネル名のバリデーション ---
        //    RFC: #channel → 英数字/特殊記号のみ
        bool valid = true;
        for (size_t i = 1; i < ch.size(); ++i) {
            if (!std::isalnum(ch[i]) &&
                ch[i] != '-' && ch[i] != '_' && ch[i] != '.')
            {
                valid = false;
                break;
            }
        }
        if (!valid)
            continue;

        // --- 5. 重複チェック (#a,#a → 1個だけ) ---
        if (seen.count(ch))
            continue;

        seen.insert(ch);
        result.push_back(ch);
    }

    return result;
}

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

