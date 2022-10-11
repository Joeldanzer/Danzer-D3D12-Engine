#pragma once

#include <string>
#include <vector>
#include <Windows.h>

enum FILE_EXPLORER {
	FILE_EXPLORER_GET,
	FILE_EXPLORER_OPEN,
	FILE_EXPLORER_SAVE
};

class FileExplorer
{
public:
	FileExplorer();
	~FileExplorer();

	struct FileType {
		std::wstring m_fileType;
		std::wstring m_folder;
	};

	std::wstring OpenFileExplorer(FILE_EXPLORER state, FileType fileType);

private:
	std::wstring GetCorrectPath(WCHAR* filePath, FileType& fileType);
};

