#pragma once

#include <string>
#include <map>
#include <set>
#include "Server.hpp"
#include "Client.hpp"

class Client; // 前方宣言

enum JoinResult {
    JOIN_SUCCESS,
    ERR_INVITEONLY,
    ERR_BADCHANNELKEY,
    ERR_CHANNELISFULL,
    ERR_ALREADYINCHANNEL
};

class Channel {
private:
    std::string _name;
    std::map<int, Client*> _clients;   // チャンネル参加クライアント
    std::string _topic;

    std::set<int> _operators;          // OP 権限を持つクライアントFD
    std::set<int> _invited;            // 招待されたクライアントFD

    /* MODE flags */
    bool _inviteOnly;      // +i
    bool _topicProtected;  // +t
    std::string _key;      // +k
    size_t _userLimit;     // +l

public:
    explicit Channel(const std::string& name)
        : _name(name),
          _inviteOnly(false),
          _topicProtected(false),
          _userLimit(0) {}

    /* --- 基本情報 --- */
    const std::string& getName() const { return _name; }
    const std::map<int, Client*>& getClients() const { return _clients; }

    /* --- クライアント追加/削除 --- */
    void addClient(Client* c) { _clients[c->getFd()] = c; }
    void removeClient(Client* c) { _clients.erase(c->getFd()); }

    bool hasClient(Client* c) const {
        return _clients.find(c->getFd()) != _clients.end();
    }

    bool isEmpty() const { return _clients.empty(); }

    /* --- Topic --- */
    const std::string& getTopic() const { return _topic; }
    void setTopic(const std::string& t) { _topic = t; }

    /* --- INVITE 管理 --- */
    void addInvite(int fd) {
        _invited.insert(fd);
    }
    bool isInvited(int fd) const {
        return _invited.count(fd) > 0;
    }
    void removeInvite(int fd) {
        _invited.erase(fd);
    }

    /* --- OP 管理（統一版） --- */
    bool isOperator(int fd) const { return _operators.count(fd) > 0; }
    void addOperator(int fd) { _operators.insert(fd); }
    void removeOperator(int fd) { _operators.erase(fd); }

    /* Client* を受けるバージョン（便利用） */
    bool isOperator(Client* c) const { return isOperator(c->getFd()); }
    void addOperator(Client* c) { addOperator(c->getFd()); }
    void removeOperator(Client* c) { removeOperator(c->getFd()); }

    /* --- MODE --- */
    bool isInviteOnly() const { return _inviteOnly; }
    void setInviteOnly(bool v) { _inviteOnly = v; }

    bool isTopicProtected() const { return _topicProtected; }
    void setTopicProtected(bool v) { _topicProtected = v; }

    const std::string& getKey() const { return _key; }
    void setKey(const std::string& k) { _key = k; }

    size_t getUserLimit() const { return _userLimit; }
    void setUserLimit(size_t l) { _userLimit = l; }

    /* --- JOIN 可能か？ --- */
    JoinResult canJoin(Client& c, const std::string& key) const {
        // すでに参加済み
        if (hasClient(&c))
            return ERR_ALREADYINCHANNEL;
        // 招待専用チャンネルで、招待されていない
        if (_inviteOnly && !_invited.count(c.getFd()))
            return ERR_INVITEONLY;
        // 鍵が設定されていて、提供された鍵が違う
        if (!_key.empty() && key != _key)
            return ERR_BADCHANNELKEY;
        // ユーザー数制限に達している
        if (_userLimit > 0 && _clients.size() >= _userLimit)
            return ERR_CHANNELISFULL;
        // すべてのチェックをパス
        return JOIN_SUCCESS;
    }
};
