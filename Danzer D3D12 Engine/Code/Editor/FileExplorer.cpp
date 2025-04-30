#include "EditorPCH.h"

#include "FileExplorer.h"
#include "Core/WindowHandler.h"

#include <tchar.h>
#include <shlobj.h>

static int CALLBACK BrowseFolderCallBack(HWND hwnd, UINT uMsg, LPARAM lparam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
	}
	return 0;
}

const std::wstring FileExplorer::FetchFileFromExplorer(const std::wstring fileLocation, const std::wstring fileType)
{
	WCHAR fileName[MAX_PATH];
	OPENFILENAME ofn;
	ZeroMemory(&ofn,      sizeof(ofn));
	ZeroMemory(&fileName, sizeof(fileName));
	
	ofn.lStructSize     = sizeof(ofn);
	ofn.hwndOwner       = WindowHandler::GetHWND();
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

const std::wstring FileExplorer::FetchFileLocation()
{
	WCHAR dirPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, dirPath);

	WCHAR path[MAX_PATH];
	BROWSEINFO bi = {};
	bi.lpszTitle  = L"Select folder";
	bi.hwndOwner  = WindowHandler::GetHWND();
	bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lParam     = LPARAM(dirPath);
	bi.lpfn       = BrowseFolderCallBack;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0) {
		SHGetPathFromIDList(pidl, path);
		IMalloc* malloc = 0;
		if (SUCCEEDED(SHGetMalloc(&malloc))) {
			malloc->Free(pidl);
			malloc->Release();
		}

		return path;
	}

	return INVALID_FILE_FECTHED;
}


const std::wstring FileExplorer::GetCorrectPath(WCHAR* filePath, const std::wstring fileLocation, const std::wstring fileType)
{
	std::wstring newPath(filePath);
	if (!newPath.empty() && newPath.find(fileType) != std::wstring::npos) {
		size_t pos = newPath.find(L"Bin\\");
		newPath = newPath.erase(0, pos + 4);
		pos = newPath.find(L"\\");
		if(pos != std::string::npos)
			newPath.replace(pos, 1, L"/");

		if (newPath.find(fileType) == std::string::npos) {
			newPath += fileType;
		}

		return newPath;
	}

	return INVALID_FILE_FECTHED;
}
