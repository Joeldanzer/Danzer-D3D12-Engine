#include "EditorPCH.h"

#include "FileExplorer.h"

#include <tchar.h>

const std::wstring FileExplorer::FetchFileFromExplorer(const std::wstring fileLocation, const std::wstring fileType)
{
	WCHAR fileName[MAX_PATH];
	OPENFILENAME ofn;
	ZeroMemory(&ofn,      sizeof(ofn));
	ZeroMemory(&fileName, sizeof(fileName));
	
	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = NULL;
	ofn.lpstrFilter		= fileType.c_str();
	ofn.lpstrFile		= fileName;
	ofn.lpstrInitialDir = LPCWSTR(fileLocation.c_str());
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrTitle		= _T("Select File");
	ofn.Flags			= OFN_DONTADDTORECENT | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;
	
	if (GetOpenFileName(&ofn)) {
		return GetCorrectPath(&fileName[0], fileLocation, fileType);
	}

	return INVALID_FILE_FECTHED;
}

const std::wstring FileExplorer::GetCorrectPath(WCHAR* filePath, const std::wstring fileLocation, const std::wstring fileType)
{
	std::wstring newPath(filePath);
	if (!newPath.empty() && newPath.find(fileType) != std::wstring::npos) {
		size_t pos = newPath.find(fileLocation);
		newPath = newPath.erase(0, pos);
		pos = newPath.find(L"\\");
		newPath.replace(pos, 1, L"/");

		if (newPath.find(fileType) == std::string::npos) {
			newPath += fileType;
		}

		return newPath;
	}

	return INVALID_FILE_FECTHED;
}
