#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void Server::handleQUIT(Client& c, const std::vector<std::string>& params)
{
    // 1. 終了理由（reason）を取得
    std::string reason;
    if (params.empty())
        reason = "Client Quit";
    else
        reason = params[0];  // 先頭の ":" はそのままでもよい

    // 2. 参加している全チャンネルに QUIT 通知を送信
    std::string quitMsg =
        ":" + c.getNickname() + " QUIT :Quit: " + reason + "\r\n";
    broadcastToAllUserChannels(c, quitMsg);

    // 3. クライアントにERRORメッセージを送信して切断を通知
    c.sendMessage("ERROR :Closing link: (" + c.getNickname() + ") [Quit: " + reason + "]\r\n");

    // 4. サーバーからクライアントを切断する
    // この関数は、チャンネルからの退出、メモリ解放、ソケットクローズなど、
    // すべての後処理を行う。
    disconnectClient(c.getFd());
}
