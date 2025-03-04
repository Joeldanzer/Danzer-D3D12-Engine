#include "EditorPCH.h"

#include "FileExplorer.h"

#include <tchar.h>

FileExplorer::FileExplorer(){}
FileExplorer::~FileExplorer(){}

std::wstring FileExplorer::OpenFileExplorer(FILE_EXPLORER state, FileType file)
{
	WCHAR fileName[MAX_PATH];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&fileName, sizeof(fileName));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;

	if (file.m_fileType == L".fbx")
		ofn.lpstrFilter = _T(".fbx");

	else if (file.m_fileType == L".dds")
		ofn.lpstrFilter = _T(".dds");

	else if (file.m_fileType == L".json")
		ofn.lpstrFilter = _T(".json");
	else
		ofn.lpstrFilter = _T("");

	ofn.lpstrFile = fileName;
	ofn.lpstrInitialDir = LPCWSTR(file.m_folder.c_str());
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = _T("Select File");
	
	switch (state)
	{
	case FILE_EXPLORER_GET:
		ofn.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn)) {
			return GetCorrectPath(fileName, file);
		}
		break;
	case FILE_EXPLORER_SAVE:
		ofn.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;
		if (GetSaveFileName(&ofn)) {
			return GetCorrectPath(fileName, file);
		}
		break;
	default:
		break;
	}

	return L"";
}

std::wstring FileExplorer::GetCorrectPath(WCHAR* filePath, FileType& fileType)
{
	std::wstring texture(filePath);
	if (!texture.empty() || texture.find_last_of(fileType.m_fileType) != std::wstring::npos) {
		size_t pos = texture.find(fileType.m_folder);
		texture = texture.erase(0, pos);

		pos = texture.find(L"\\");
		texture.replace(pos, 1, L"/");

		if (texture.find(fileType.m_fileType) == std::string::npos) {
			texture += fileType.m_fileType;
		}

		return texture;
	}

	return L"";
}
