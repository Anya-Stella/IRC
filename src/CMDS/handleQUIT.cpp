#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleQUIT(Client& c, const std::vector<std::string>& params)
{
    // 1. 終了理由（reason）を決定
    std::string reason;
    if (params.empty())
        reason = "Client Quit";
    else
        reason = params[0];  // 先頭の ":" はそのままでもよい

    // 2. 全チャンネルに QUIT 通知を送る
    std::string quitMsg =
        ":" + c.getNickname() + " QUIT :" + reason + "\r\n";

    broadcastToAllUserChannels(c, quitMsg);

    // 3. 全チャンネルからクライアントを削除
    const std::vector<std::string>& joined = c.getChannels();

    for (size_t i = 0; i < joined.size(); i++)
    {
        const std::string& name = joined[i];

        //count(std::map や std::set に対して キーが存在するかどうか調べる関数)
        if (_channels.count(name) == 0)
            continue;

        Channel* ch = _channels[name];
        ch->removeClient(&c);

        // 空になったチャンネルは削除
        if (ch->isEmpty())
        {
            delete ch;
            //erase(map, set, vector などに共通してある 要素削除メソッド)
            _channels.erase(name);
        }
    }

    // 4. サーバーのクライアントリストから削除
    int fd = c.getFd();
    _clients.erase(fd);

    // 5. ソケットを閉じる
    close(fd);

    // 6. クライアントを delete
    delete &c;
}
