#pragma once

#include <tgbot/Bot.h>
#include <map>
#include <string>
#include <vector>

class SimpleTgBackup {
private:
    std::int64_t m_BackupChatId;
    TgBot::Bot m_Bot;
    TgBot::Chat::Ptr m_BackupChat;
    std::string m_BotName;
    std::string m_ApplicationName;
public:
    SimpleTgBackup(const std::string &token, std::int64_t backup_chat, const std::string &bot_name, const std::string &application_name);

    bool IsValid() const;

    bool Backup(const std::map<std::string, std::string> &files);

    bool BackupDirectory(const std::string &path);

    //bool Backup(const std::string &filename, const std::string &content);
private:
    std::string BuildZipArchive(const std::map<std::string, std::string> &files);
};