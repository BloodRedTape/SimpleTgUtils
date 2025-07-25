#pragma once

#include <fstream>
#include <chrono>
#include <map>
#include <tgbot/Bot.h>
#include <bsl/format.hpp>

struct LoggedMessage {
	std::string Content;
	std::chrono::steady_clock::time_point Time;
	std::int32_t MessageId = 0;
	std::int32_t Count = 0;
};

class SimpleTgLogger{
	int64_t m_LogChatId;
	int64_t m_LogTopicId;
	TgBot::Bot m_Bot;
	TgBot::Chat::Ptr m_LogChat;
	std::string m_BotName;
	bool m_IsEnabled = false;
	std::int64_t m_TopicId;

	std::map<std::int64_t, LoggedMessage> m_LastMessages;
public:
	SimpleTgLogger(const std::string &token, std::int64_t log_chat, const std::string &bot_name, std::int64_t topic_id = 0);

	bool IsValid()const;

	void SetEnabled(bool is) {
		m_IsEnabled = is;
	}

	bool IsEnabled()const {
		return m_IsEnabled;
	}

	void Log(const std::string &message);
	
	template<typename...ArgsType>
	void Log(const char* fmt, ArgsType&&...args) {
		return Log(Format(fmt, std::forward<ArgsType>(args)...));
	}
};
