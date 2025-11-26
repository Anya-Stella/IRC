#pragma once

#include "Server.hpp"

class Client; // 前方宣言

class Channel {
private:
    std::string _name;
    std::map<int, Client*> _clients; // このチャンネルに参加しているクライアント
    std::string _key;
    bool _inviteOnly;
    size_t _userLimit;
    std::string _topic;

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
};

