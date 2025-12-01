#pragma once

#include <string>
#include <map>
#include <set>
#include "Server.hpp"
#include "Client.hpp"

class Client; // 前方宣言

class Channel {
private:
    std::string _name;
    std::map<int, Client*> _clients;       // このチャンネルに参加しているクライアント
    std::string _topic;
    std::set<int> _operators;              // オペレーターの fd を保持
    std::set<int> _invited;                // 招待されたクライアントFD一覧

    /*MODE*/
    bool _inviteOnly;      // +i
    bool _topicProtected;  // +t
    std::string _key;      // +k
    size_t _userLimit;     // +l

public:
    explicit Channel(const std::string& name) : _name(name), _inviteOnly(false), _userLimit(0) {}

    // チャンネル名取得
    const std::string& getName() const { return _name; }

    // 参加クライアントを返す（broadcast用）
    const std::map<int, Client*>& getClients() const { return _clients; }

    // クライアント追加
    void addClient(Client* c) { _clients[c->getFd()] = c; }

    // クライアント削除
    void removeClient(Client* c) { _clients.erase(c->getFd()); }

    // チャンネル参加可能か（キー、制限、招待制など）
    bool canJoin(Client& c, const std::string& key) const {
        if (_inviteOnly) return false;
        if (!_key.empty() && key != _key) return false;
        if (_userLimit > 0 && _clients.size() >= _userLimit) return false;
        return true;
    }

    // topic 取得 / 設定
    const std::string& getTopic() const { return _topic; }
    void setTopic(const std::string& t) { _topic = t; }

    // チャンネルにクライアントがいるかチェック
    bool hasClient(Client* c) const {
        return _clients.find(c->getFd()) != _clients.end();
    }
    // チャンネルが空かどうか確認
    bool isEmpty() const {
        return _clients.empty();
    }

    //operator
    void addOperator(Client* c) { _operators.insert(c->getFd()); }
    void removeOperator(Client* c) { _operators.erase(c->getFd()); }
    bool isOperator(Client* c) const { return _operators.count(c->getFd()) > 0; }

    bool isInviteOnly() const;         // +i 状態の取得
    void setInviteOnly(bool mode);     // +i 設定
    void addInvite(int fd);            // 招待リストに追加
    bool isInvited(int fd) const;      // 招待されてるか？

    /*TOPIC*/
    const std::string& getTopic() const { return _topic; }
    void setTopic(const std::string& t) { _topic = t; }
    bool isTopicProtected() const { return _topicProtected; }
    void setTopicProtected(bool p) { _topicProtected = p; }

    /*MODE*/
    bool isInviteOnly() const { return _inviteOnly; }
    void setInviteOnly(bool v) { _inviteOnly = v; }
    bool isTopicProtected() const { return _topicProtected; }
    void setTopicProtected(bool v) { _topicProtected = v; }
    const std::string& getKey() const { return _key; }
    void setKey(const std::string& k) { _key = k; }
    size_t getUserLimit() const { return _userLimit; }
    void setUserLimit(size_t l) { _userLimit = l; }
    bool isOperator(int fd) const { return _operators.count(fd); }
    void addOperator(int fd) { _operators.insert(fd); }
    void removeOperator(int fd) { _operators.erase(fd); }
};

