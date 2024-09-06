#include "simple_tg_logger/simple_tg_logger.hpp"
#include "bsl/file.hpp"

SimpleTgLogger::SimpleTgLogger(const std::string &token, std::int64_t log_chat, const std::string &bot_name):
	m_LogChatId(log_chat),
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
	if (!m_IsEnabled || !m_LogChat) {
		return;
	}

	try{
		m_Bot.getApi().sendMessage(m_LogChat->id, Format("[%]: %", m_BotName, message));
	} catch (const std::exception &exception) {
		Println("Can't log for chat_id % with token %, reason: %", m_LogChatId, m_Bot.getToken(), exception.what());
	}
}
