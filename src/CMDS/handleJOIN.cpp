#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Client.hpp"
#include "../../include/Utils.hpp"

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
        // オペレーターの場合はニックネームの前に '@' を付ける
        if (channel.isOperator(it->first)) {
            names += "@";
        }
        names += it->second->getNickname() + " ";
    }
 
    // 最後の余分な空白を削除
    if (!names.empty())
        names.erase(names.size() - 1);

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

void Server::partClientFromAllChannels(Client &c)
{
    std::vector<std::string> channels = c.getAllChannels();

    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string name = channels[i];

        if (_channels.count(name))
        {
            Channel* ch = _channels[name];

            // Broadcast: <nick> PART #channel
            broadcastToChannel(*ch, ":" + c.getNickname() + " PART " + name + "\r\n");

            // Channel から削除
            ch->removeClient(&c);
        }

        // Client 側も削除
        c.leaveChannel(name);
    }
}

void Server::joinSingleChannel(Client &c, const std::string &channelName, const std::string &key)
{
    // チャンネルがすでに存在するか確認
    Channel *channel;

    if (_channels.count(channelName))
        channel = _channels[channelName];
    else
    {
        channel = new Channel(channelName);
        _channels[channelName] = channel;

        // ---- ここ大事！最初の参加者に OP 付与 ----
        channel->addOperator(c.getFd());
    }

    // JOIN 可能か（invite-only / key / userLimit）
    if (!channel->canJoin(c, key))
    {
        c.sendMessage(":ircserv 475 " + channelName + " :Cannot join channel\r\n");
        return;
    }

    // すでに参加しているなら何もしない
    if (channel->hasClient(&c))
        return;

    // 参加処理
    channel->addClient(&c);
    c.joinChannel(channelName);

    // 通知
    broadcastToChannel(*channel, ":" + c.getNickname() + " JOIN :" + channelName);

    // NAMES リスト送信
    sendNamesReply(c, *channel);
}


void Server::handleJOIN(Client &c, const std::vector<std::string> &params)
{
    if (params.empty()) {
        c.sendMessage("461 JOIN :Not enough parameters\r\n");
        return;
    }

    // JOIN 0 → 全てのチャンネルから抜ける
#ifdef DEBUG_MODE
    std::cerr << "[DEBUG] params[0] = '" << params[0] << "'" << std::endl;
#endif

    std::string target = trim(params[0]);
    if (target == "0" || target == "#0") {
        partClientFromAllChannels(c);
        return;
    }


    // 複数チャンネル・複数キーの処理
    std::vector<std::string> chList = split(params[0], ',');
    std::vector<std::string> keyList;

    if (params.size() > 1)
        keyList = split(params[1], ',');

    for (size_t i = 0; i < chList.size(); i++)
    {
        std::string channelName = chList[i];
        std::string key = (i < keyList.size()) ? keyList[i] : "";

        joinSingleChannel(c, channelName, key);
    }
}


// void Server::handleJOIN(Client &c, const std::vector<std::string> &params)
// {
//     if (params.empty()) {
//         c.sendMessage("461 JOIN :Not enough parameters\r\n");
//         return;
//     }

//     const std::string &channelName = params[0];
//     std::string key = (params.size() > 1) ? params[1] : "";

//     // チャンネル存在確認・作成
//     Channel* channel = NULL;
//     if (_channels.count(channelName))
//         channel = _channels[channelName];
//     else
//         channel = new Channel(channelName), _channels[channelName] = channel;

//     // 条件チェック（key, inviteOnly, userLimit など）
//     if (!channel->canJoin(c, key)) {
//         c.sendMessage("Cannot join channel...\r\n"); 
//         return;
//     }

//     // --- 参加処理 ---
//     channel->addClient(&c);
//     c.joinChannel(channelName);

//     // --- ★ 最初の参加者は OP（チャネルオペレーター）にする ---
//     if (channel->getClients().size() == 1) {
//         channel->addOperator(c.getFd());
//         // OP がついたことを通知
//         broadcastToChannel(*channel, ":" + c.getNickname() +
//                            " MODE " + channelName + " +o " + c.getNickname() + "\r\n");
//     }

//     // 通知
//     broadcastToChannel(*channel, ":" + c.getNickname() + " JOIN :" + channelName);

//     // NAMES リスト送信
//     sendNamesReply(c, *channel);
// }
