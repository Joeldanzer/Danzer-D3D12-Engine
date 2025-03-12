#pragma once

#include <string>
#include <vector>
#include <Windows.h>

enum FILE_EXPLORER {
	FILE_EXPLORER_GET,
	FILE_EXPLORER_OPEN,
	FILE_EXPLORER_SAVE
};

#define INVALID_FILE_FECTHED L"Error: Invalid File!"

class FileExplorer
{
public:
	static const std::wstring FetchFileFromExplorer(const std::wstring fileLocation, const std::wstring fileType);

private:
	static const std::wstring GetCorrectPath(WCHAR* filePath, const std::wstring fileLocation, const std::wstring fileType);
};

