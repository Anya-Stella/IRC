#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

//あるチャンネルに属する全員へ通知する(Channel& → 全メンバーへ送信)
void Server::broadcastToChannel(Channel& ch, const std::string& message)
{
    const std::map<int, Client*>& members = ch.getClients();
    for (std::map<int, Client*>::const_iterator it = members.begin();
         it != members.end(); ++it)
    {
        it->second->sendMessage(message);
    }
}
// 新しく参加したクライアントにチャンネルの参加者一覧（NAMES リスト）を送信する関数
void Server::sendNamesReply(Client& c, const Channel& channel)
{
    // 参加者一覧を取得
    const std::map<int, Client*>& members = channel.getClients();
    std::string names;

    // 各クライアントのニックネームを連結
    for (std::map<int, Client*>::const_iterator it = members.begin();
         it != members.end(); ++it)
    {
        names += it->second->getNickname() + " "; // スペース区切りで追加
    }

    // 最後の余分な空白を削除
    if (!names.empty())
        names.pop_back();

    // --- IRCプロトコルに従ったメッセージ送信 ---
    
    // 1. RPL_NAMREPLY (353): チャンネル内のニックネーム一覧を送る
    // フォーマット: :server 353 <nick> = <channel> :<nick1> <nick2> ...
    c.sendMessage(":ircserv 353 " + c.getNickname() +
                  " = " + channel.getName() + " :" + names + "\r\n");

    // 2. RPL_ENDOFNAMES (366): NAMESリストの終わりを通知
    // フォーマット: :server 366 <nick> <channel> :End of /NAMES list
    c.sendMessage(":ircserv 366 " + c.getNickname() +
                  " " + channel.getName() + " :End of /NAMES list\r\n");
}

void Server::handleJOIN(Client &c, const std::vector<std::string> &params)
{
    if (params.empty()) {
        c.sendMessage("461 JOIN :Not enough parameters\r\n");
        return;
    }

    const std::string &channelName = params[0];
    std::string key = (params.size() > 1) ? params[1] : "";

    // チャンネル存在確認・作成
    Channel* channel = nullptr;
    if (_channels.count(channelName))
        channel = _channels[channelName];
    else
        channel = new Channel(channelName), _channels[channelName] = channel;

    // 条件チェック（key, inviteOnly, userLimit など）
    if (!channel->canJoin(c, key)) {
        c.sendMessage("Cannot join channel...\r\n"); 
        return;
    }

    // 参加登録
    channel->addClient(&c);
    c.joinChannel(channelName);

    // 通知
    broadcastToChannel(*channel, ":" + c.getNickname() + " JOIN :" + channelName);

    // ユーザー一覧送信
    sendNamesReply(c, *channel);
}
