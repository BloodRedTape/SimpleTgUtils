#include "simple/tg_backup.hpp"
#include <filesystem>
#include <miniz.h>
#include <bsl/file.hpp>

SimpleTgBackup::SimpleTgBackup(const std::string &token, std::int64_t backup_chat, const std::string &bot_name, const std::string &application_name):
	m_Bot(token),
	m_BackupChatId(backup_chat),
	m_BotName(bot_name),
	m_ApplicationName(application_name)
{
	try {
		m_BackupChat = m_Bot.getApi().getChat(m_BackupChatId);
	} catch (const std::exception &exception) {
		Println("Can't get chat for chat_id % with token %, reason: %", m_BackupChatId, m_Bot.getToken(), exception.what());
	}
}

bool SimpleTgBackup::IsValid() const {
	return (bool)m_BackupChat;
}

bool SimpleTgBackup::Backup(const std::map<std::string, std::string>& files) {
	TgBot::InputFile::Ptr file(new TgBot::InputFile());
	file->data = BuildZipArchive(files);
	file->fileName = Format("%_backup.zip", m_ApplicationName);
	file->mimeType = "application/zip";

	auto caption = Format("#%", m_ApplicationName);

	return !!m_Bot.getApi().sendDocument(m_BackupChatId, file, "", caption);
}

bool SimpleTgBackup::BackupDirectory(const std::string &directory_path) {
    if (!IsValid()) {
        return false;
    }

    std::map<std::string, std::string> files;
    std::filesystem::path base_path = std::filesystem::path(directory_path);
        
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            std::string rel_path = entry.path().lexically_relative(base_path).string();
            std::string file_content = ReadEntireFile(entry.path().string());
            files[rel_path] = file_content;
        }
    }
	
	return Backup(files);
}

std::string SimpleTgBackup::BuildZipArchive(const std::map<std::string, std::string> &files) {
	mz_zip_archive zip;
	memset(&zip, 0, sizeof(zip));
	if (!mz_zip_writer_init_heap(&zip, 0, 0)) {
		return {};
	}

	bool success = true;

	for (const auto& [file_path, content] : files) {
		if (!mz_zip_writer_add_mem(&zip, file_path.c_str(), content.data(), content.size(), MZ_BEST_COMPRESSION)) {
			success = false;
			break;
		}
	}

	void* zip_buffer = nullptr;
	size_t zip_size = 0;
	std::string zip_data;

	if (success && mz_zip_writer_finalize_heap_archive(&zip, &zip_buffer, &zip_size)) {
		zip_data.assign(static_cast<const char*>(zip_buffer), zip_size);
	}

	mz_zip_writer_end(&zip);

	return zip_data;
}
