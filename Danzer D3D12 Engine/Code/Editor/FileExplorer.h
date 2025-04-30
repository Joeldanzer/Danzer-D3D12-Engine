#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#define INVALID_FILE_FECTHED L"Error: Invalid File!"

class FileExplorer
{
public:
	static const std::wstring FetchFileFromExplorer(const std::wstring fileLocation, const std::wstring fileType);
	static const std::wstring FetchFileLocation();

	static const std::wstring SearchForFile(const std::wstring startLocation, const std::wstring fileType);

private:
	static const std::wstring GetCorrectPath(WCHAR* filePath, const std::wstring fileLocation, const std::wstring fileType);
};

