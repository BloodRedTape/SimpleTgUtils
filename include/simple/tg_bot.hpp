#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <optional>
#include <bsl/format.hpp>
#include <tgbot/Bot.h>
#include <tgbot/net/TgLongPoll.h>

#undef SendMessage

enum class ButtonStyle {
    Default,
    Success,
    Primary,
    Danger
};

struct KeyboardButton {
    std::string Text;
    std::string CallbackData;
    bool Enabled = true;
    ButtonStyle Style;

    KeyboardButton(std::string text, std::string callback_data, bool enabled = true, ButtonStyle style = ButtonStyle::Default) :
        Text(std::move(text)),
        CallbackData(std::move(callback_data)),
        Enabled(enabled),
        Style(style)
    {}

    KeyboardButton(std::string text, bool enabled = true) :
        Text(text),
        CallbackData(text),
        Enabled(enabled)
    {}

    bool operator==(const KeyboardButton& other)const {
        return Text == other.Text && CallbackData == other.CallbackData && Enabled == other.Enabled;
    }
    bool operator!=(const KeyboardButton& other)const {
        return !(*this == other);
    }
};

using KeyboardLayout = std::vector<std::vector<KeyboardButton>>;

namespace Keyboard {
    KeyboardLayout ToKeyboard(const std::vector<std::string> &texts);

    KeyboardLayout ToNiceKeyboard(const std::vector<std::string> &texts, std::size_t row_size,  std::function<std::string(std::string)> make_text = [](auto s){return s;}, std::function<std::string(std::string)> make_key = [](auto s){return s;});
    
    template<typename T>
    KeyboardLayout ToNiceKeyboard(const std::vector<T>& entries, std::size_t row_size, std::function<std::string(const T &)> make_text, std::function<std::string(const T &)> make_key){
        KeyboardLayout layout;
        std::vector<KeyboardButton> row;
    
        std::size_t i = 0;
        for (const auto& entry: entries) {
            row.emplace_back(make_text(entry), make_key(entry));
            i++;

            if (i == row_size) {
                i = 0;
                layout.push_back(std::move(row));
            }
        }
    
        if(row.size())
            layout.push_back(std::move(row));

        return layout;
    }

    std::vector<KeyboardButton> ToKeyboardRow(const std::vector<std::string> &texts);
}

class SimpleTgBot: public TgBot::Bot{
    static constexpr const char *ParseMode = "HTML";
    static constexpr bool DisableWebpagePreview = true;
public:
    using LogHandler = std::function<void(const std::string&)>;

    using CommandHandler = std::function<void(TgBot::Message::Ptr)>;
    using MessageHandler = std::function<void(TgBot::Message::Ptr)>;
    using CallbackQueryHandler = std::function<void(TgBot::CallbackQuery::Ptr)>;
    using ChatMemberStatusHandler = std::function<void(TgBot::ChatMemberUpdated::Ptr)>;
private:
    LogHandler m_Log;

    std::unordered_map<std::string, CommandHandler> m_CommandHandlers;
    std::unordered_map<std::string, std::string> m_CommandDescriptions;

    std::string m_Username;
public:
    SimpleTgBot(const std::string &token, const TgBot::HttpClient &client = GetDefaultHttpClient());

    void LongPoll(std::int32_t limit = 100, std::int32_t timeout = 10, std::vector<std::string> &&allowed_updates = {});

    virtual void OnLongPollIteration();

    void OnLog(LogHandler handler);

    template<typename Type>
    void OnLog(Type *object, void (Type::*handler)(const std::string &));

    void Log(const std::string &message);
    
    template<typename ...ArgsType>
	void Log(const char* fmt, const ArgsType&...args);

    void ClearOldUpdates();

    bool SendChatAction(TgBot::Message::Ptr source, const std::string &action);

    TgBot::Message::Ptr SendMessage(std::int64_t chat, std::int32_t topic, const std::string& message, std::int64_t reply_message = 0, bool silent = false);

    TgBot::Message::Ptr SendMessage(TgBot::Message::Ptr source, const std::string& message, bool reply = false, bool silent = false);
    
    TgBot::Message::Ptr ReplyMessage(TgBot::Message::Ptr source, const std::string& message, bool silent = false){return SendMessage(source, message, true, silent); }

    //source->isTopicMessage ? source->messageThreadId : 0
    TgBot::Message::Ptr SendMessage(std::int64_t chat, std::int32_t topic, const std::string& message, TgBot::GenericReply::Ptr reply, std::int64_t reply_message = 0, bool silent = false);

    TgBot::Message::Ptr SendKeyboard(std::int64_t chat, std::int32_t topic, const std::string& message, const KeyboardLayout& keyboard, std::int64_t reply_message = 0);

    TgBot::Message::Ptr SendKeyboard(TgBot::Message::Ptr source, const std::string& message, const KeyboardLayout &keyboard, bool reply = false){return SendKeyboard(source->chat->id, source->isTopicMessage ? source->messageThreadId : 0, message, keyboard, reply ? source->messageId : 0); }

    TgBot::Message::Ptr ReplyKeyboard(TgBot::Message::Ptr source, const std::string& message, const KeyboardLayout &keyboard){return SendKeyboard(source, message, keyboard, true);}

    TgBot::Message::Ptr SendPhoto(std::int64_t chat, std::int32_t topic, const std::string& text, TgBot::InputFile::Ptr photo, std::int64_t reply_message = 0);

    TgBot::Message::Ptr SendPhoto(TgBot::Message::Ptr source, const std::string& text, TgBot::InputFile::Ptr photo);

    TgBot::Message::Ptr ReplyPhoto(TgBot::Message::Ptr source, const std::string& text, TgBot::InputFile::Ptr photo);

    TgBot::Message::Ptr SendFile(std::int64_t chat, std::int32_t topic, const std::string& text, TgBot::InputFile::Ptr file, std::int64_t reply_message = 0);

    TgBot::Message::Ptr SendFile(TgBot::Message::Ptr source, const std::string& text, TgBot::InputFile::Ptr photo);

    TgBot::Message::Ptr ReplyFile(TgBot::Message::Ptr source, const std::string& text, TgBot::InputFile::Ptr photo);

    TgBot::Message::Ptr EditMessage(TgBot::Message::Ptr message, const std::string& text, TgBot::InlineKeyboardMarkup::Ptr reply);

    TgBot::Message::Ptr EditMessage(TgBot::Message::Ptr message, const std::string& text, const KeyboardLayout& keyboard);

    TgBot::Message::Ptr EditMessage(TgBot::Message::Ptr message, const KeyboardLayout& keyboard);

    TgBot::Message::Ptr EditKeyboard(std::int64_t chat, std::int32_t message, TgBot::InlineKeyboardMarkup::Ptr reply);

    TgBot::Message::Ptr EditKeyboard(std::int64_t chat, std::int32_t message, const KeyboardLayout &keyboard);

    TgBot::Message::Ptr EditMessage(std::int64_t chat, std::int32_t message, const std::string& text, TgBot::InlineKeyboardMarkup::Ptr reply = nullptr);

    TgBot::Message::Ptr EditMessage(TgBot::Message::Ptr message, const std::string& text);

    bool DeleteMessage(TgBot::Message::Ptr message);

    bool RemoveKeyboard(TgBot::Message::Ptr message);

    TgBot::Message::Ptr EnsureMessage(TgBot::Message::Ptr ensurable, std::int64_t chat, std::int32_t topic, const std::string &message, TgBot::InlineKeyboardMarkup::Ptr reply = nullptr);

    TgBot::Message::Ptr EnsureKeyboard(TgBot::Message::Ptr ensurable, std::int64_t chat, std::int32_t topic, const std::string &message, const KeyboardLayout &keyboard);

    bool AnswerCallbackQuery(const std::string& callbackQueryId, const std::string& text = "");

    void OnCommand(const std::string &command, CommandHandler handler, std::string &&description = "");
    
    template<typename Type>
    void OnCommand(const std::string &command, Type *object, void (Type::*handler)(TgBot::Message::Ptr), std::string &&description = "");

    void BroadcastCommand(const std::string &command, TgBot::Message::Ptr message);

    void OnNonCommandMessage(MessageHandler message);

    template<typename Type>
    void OnNonCommandMessage(Type *object, void (Type::*handler)(TgBot::Message::Ptr));

    void OnCallbackQuery(CallbackQueryHandler handler);

    template<typename Type>
    void OnCallbackQuery(Type *object, void (Type::*handler)(TgBot::CallbackQuery::Ptr));

    void OnMyChatMember(ChatMemberStatusHandler chat_member);

    template<typename Type>
    void OnMyChatMember(Type *object, void (Type::*handler)(TgBot::ChatMemberUpdated::Ptr));

    void OnOtherChatMember(ChatMemberStatusHandler chat_member);

    template<typename Type>
    void OnOtherChatMember(Type *object, void (Type::*handler)(TgBot::ChatMemberUpdated::Ptr));

	std::optional<std::string> DownloadFile(const std::string &file_id)const;

    void UpdateCommandDescriptions();

    std::string ParseCommand(TgBot::Message::Ptr message);

    static std::size_t GetCommandLength(const std::string &text);

    static std::string GetTextWithoutCommand(TgBot::Message::Ptr message);

    static std::string GetTextWithoutCommand(const std::string &text);

    static const TgBot::HttpClient &GetDefaultHttpClient();
};

template<typename Type>
void SimpleTgBot::OnLog(Type* object, void (Type::* handler)(const std::string&)) {
    OnLog(std::bind(handler, object, std::placeholders::_1));
}

template<typename ...ArgsType>
void SimpleTgBot::Log(const char* fmt, const ArgsType&...args) {
	Log(Format(fmt, args...));
}

template<typename Type>
void SimpleTgBot::OnCommand(const std::string& command, Type *object, void (Type::* handler)(TgBot::Message::Ptr), std::string &&description) {
    OnCommand(command, std::bind(handler, object, std::placeholders::_1), std::move(description));
}

template<typename Type>
void SimpleTgBot::OnNonCommandMessage(Type* object, void (Type::* handler)(TgBot::Message::Ptr)) {
    OnNonCommandMessage(std::bind(handler, object, std::placeholders::_1));
}

template<typename Type>
void SimpleTgBot::OnCallbackQuery(Type* object, void (Type::* handler)(TgBot::CallbackQuery::Ptr)) {
    OnCallbackQuery(std::bind(handler, object, std::placeholders::_1));
}

template<typename Type>
void SimpleTgBot::OnMyChatMember(Type* object, void (Type::* handler)(TgBot::ChatMemberUpdated::Ptr)) {
    OnMyChatMember(std::bind(handler, object, std::placeholders::_1));
}

template<typename Type>
void SimpleTgBot::OnOtherChatMember(Type* object, void (Type::* handler)(TgBot::ChatMemberUpdated::Ptr)) {
    OnOtherChatMember(std::bind(handler, object, std::placeholders::_1));
}

class SimplePollBot: public SimpleTgBot{
    TgBot::TgLongPoll m_Poll;
public:
    SimplePollBot(const std::string &token, std::int32_t limit = 100, std::int32_t timeout = 10);
    
    void LongPollIteration();
};


class FastLongPoll {
public:
    FastLongPoll(const TgBot::Api* api, const TgBot::EventHandler* eventHandler, std::int32_t limit, std::int32_t timeout, std::shared_ptr<std::vector<std::string>> allowUpdates);
    FastLongPoll(const TgBot::Bot& bot, std::int32_t limit = 100, std::int32_t timeout = 10, const std::shared_ptr<std::vector<std::string>>& allowUpdates = nullptr);

    void start();

private:

    void handleUpdates();

private:
    const TgBot::Api* _api;
    const TgBot::EventHandler* _eventHandler;
    std::int32_t _lastUpdateId = 0;
    std::int32_t _limit;
    std::int32_t _timeout;
    std::shared_ptr<std::vector<std::string>> _allowUpdates;

    std::vector<TgBot::Update::Ptr> _updates;
};

