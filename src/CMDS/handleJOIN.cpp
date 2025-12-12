#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"
#include "../../include/Client.hpp"
#include "../../include/Utils.hpp"
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

            // Channel からクライアントを先に削除
            ch->removeClient(&c);

            // Broadcast: <nick> QUIT :Client Quit
            broadcastToChannel(*ch, ":" + c.getPrefix() + " QUIT :Client Quit\r\n", &c);
        }

        // Client 側も削除
        c.leaveChannel(name);
    }
}

void Server::joinSingleChannel(Client &c, const std::string &channelName, const std::string &key)
{
    // クライアントがすでに対象チャンネルに参加していると認識している場合は、何もしない
    if (c.getJoinedChannels().count(channelName)) {
        return;
    }

    Channel *channel;
    bool isNewChannel = false;

    if (_channels.count(channelName))
        channel = _channels[channelName];
    else
    {
        isNewChannel = true;
        channel = new Channel(channelName);
        _channels[channelName] = channel;

        // ---- ここ大事！最初の参加者に OP 付与 ----
        channel->addOperator(c.getFd());
    }

    // JOIN 可能か（invite-only / key / userLimit）
    JoinResult result = channel->canJoin(c, key);
    if (result != JOIN_SUCCESS)
    {
        switch (result)
        {
            case ERR_BADCHANNELKEY:
                // 475 ERR_BADCHANNELKEY: <channel> :Cannot join channel (+k)
                c.sendMessage(":ircserv 475 " + c.getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n");
                break;
            case ERR_INVITEONLY:
                // 473 ERR_INVITEONLYCHAN: <channel> :Cannot join channel (+i)
                c.sendMessage(":ircserv 473 " + c.getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n");
                break;
            case ERR_CHANNELISFULL:
                // 471 ERR_CHANNELISFULL: <channel> :Cannot join channel (+l)
                c.sendMessage(":ircserv 471 " + c.getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n");
                break;
            case ERR_ALREADYINCHANNEL:
                // すでに参加している場合は何もしない
                return;
            default: // JOIN_SUCCESS or other cases
                break;
        }
        // 新規作成したチャンネルへの参加に失敗し、他に誰もいなければ削除してメモリリークを防ぐ
        if (isNewChannel && channel->isEmpty())
        {
            _channels.erase(channelName);
            delete channel;
        }
        return;
    }

    // 参加処理
    channel->addClient(&c);
    c.joinChannel(channelName);

    // 参加に成功したので、招待は消費されたとみなす
    channel->removeInvite(c.getFd());

    // 通知
    broadcastToChannel(*channel, ":" + c.getPrefix() + " JOIN :" + channelName + "\r\n", NULL);

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
        c.sendMessage(":ircserv 000 " + c.getNickname() + " :You have left all channels\r\n");
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
