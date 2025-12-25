#include "simple/tg_logger.hpp"
#include "bsl/file.hpp"

SimpleTgLogger::SimpleTgLogger(const std::string &token, std::int64_t log_chat, const std::string &bot_name, std::int64_t log_topic):
	m_LogChatId(log_chat),
	m_LogTopicId(log_topic),
	m_Bot(token),
	m_BotName(bot_name),
	m_IsEnabled(true)
{
	try {
		m_LogChat = m_Bot.getApi().getChat(m_LogChatId);
	} catch (const std::exception &exception) {
		Println("Can't get chat for chat_id % with token %, reason: %", m_LogChatId, m_Bot.getToken(), exception.what());
	}
}

bool SimpleTgLogger::IsValid() const {
	return (bool)m_LogChat;
}

void SimpleTgLogger::Log(const std::string& message) {
	if (!m_IsEnabled || !m_LogChat || !message.size()) {
		return;
	}

	std::string content = m_LogTopicId ? message : Format("[%]: %", m_BotName, message);

	LoggedMessage& last_message = m_LastMessages[m_LogTopicId];

	constexpr auto ResendPeriod = std::chrono::minutes(10);
	const auto now = std::chrono::steady_clock::now();

	try {
		if (last_message.Content == content && now - last_message.Time < ResendPeriod) {
			last_message.Count++;

			std::string edited_content = Format("%\n\n<b>Repeated % Times</b>", content, last_message.Count);

			auto edited = m_Bot.getApi().editMessageText(edited_content, m_LogChatId, last_message.MessageId, "", "HTML");

			if(edited)
				return;
		}
	}catch (const std::exception &e) {
		Println("Can't edit repeated message for chat_id % with token %, reason: %", m_LogChatId, m_Bot.getToken(), exception.what());
	}
	
	try{
		auto message = m_LogTopicId ? m_Bot.getApi().sendMessage(m_LogChat->id, content, nullptr, nullptr, nullptr, "", false, {}, m_LogTopicId) : m_Bot.getApi().sendMessage(m_LogChat->id, content);
			
		if(message){
			last_message.Content = content;
			last_message.Time = now;
			last_message.MessageId = message->messageId;
			last_message.Count = 1;
		}

	} catch (const std::exception &exception) {
		Println("Can't log for chat_id % with token %, reason: %", m_LogChatId, m_Bot.getToken(), exception.what());
	}
}
