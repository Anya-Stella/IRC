#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

//NICKのバリデーション
bool Server::isValidNick(const std::string& nickname)
{
    // 許可される記号リスト
    const std::string allowedSymbols = "-_[]\\`^{}";

    // 長さチェック（1～9文字）
    if (nickname.size() < 1 || nickname.size() > 9)
        return false;

    // 先頭文字チェック
    char first = nickname[0];
    if (!isalpha(static_cast<unsigned char>(first)) &&
        allowedSymbols.find(first) == std::string::npos)
    {
        return false;
    }

    // 2文字目以降チェック
    for (size_t i = 1; i < nickname.size(); ++i)
    {
        char c = nickname[i];
        if (!isalnum(static_cast<unsigned char>(c)) &&
            allowedSymbols.find(c) == std::string::npos)
        {
            return false;
        }
    }

    return true;
}

//string型を大文字にする関数
std::string	toUpperCaseString(const std::string& n)
{
	std::string result;
	for(size_t i = 0; i < n.length(); i++)
	{
		result += toupper(static_cast<unsigned char>(n[i]));
	}
	return(result);
}
//NICKの重複チェックしてる関数
bool    Server::isUsedNick(const std::string& nickname)
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::string cNick = it->second->getNickname();
		//重複には大文字小文字関係ないからすべて大文字で比べる
		if(toUpperCaseString(cNick) == toUpperCaseString(nickname))
			return(true);
	}
	return(false);
}
//そのユーザーが属しているチャンネル全てへ通知する
void Server::broadcastToAllUserChannels(Client& sender, const std::string& message)
{
    const std::vector<std::string>& userChannels = sender.getChannels();

    for (size_t i = 0; i < userChannels.size(); i++)
    {
        Channel* ch = _channels[userChannels[i]];
        if (!ch) continue;

        const std::map<int, Client*>& members = ch->getClients();
        for (std::map<int, Client*>::const_iterator it = members.begin();
             it != members.end(); ++it)
        {
            it->second->sendMessage(message);
        }
    }
}

void	Server::handleNICK(Client &c, const std::vector<std::string> &params)
{
	// NICK に必要なパラメータが不足
	if(params.empty())
	{
		c.sendMessage("461 NICK :Not enough parameters\r\n"); // ERR_NEEDMOREPARAMS
        return;
	}
	const std::string newNick = params[0];

	// 2. 形式チェック
    if (!isValidNick(newNick)) {
        c.sendMessage("432 NICK :Erroneous nickname\r\n");
        return;
    }
	 // 3. 重複チェック
	if (isUsedNick(newNick)) {
        c.sendMessage("433 NICK :Nickname is already in use\r\n");
        return;
    }
    // 4. ニックネーム変更通知（既にニックネームがある場合
    //    かつ、クライアントが登録済みの場合のみ）
    if (c.hasNick() && c.isFullyRegistered()) {
        broadcastToAllUserChannels(c, ":" + c.getNickname() + " NICK :" + newNick + "\r\n");
    }

    // 5. 更新
    c.setNick(newNick);

    // 6. 登録完了チェック
    if (c.readyToRegister()) {
        sendWelcome(c);
    }
}
