#include "stdafx.h"
#include "WindowHandler.h"

#include "../3rdParty/imgui-master/backends/imgui_impl_win32.h"

WindowHandler::Data WindowHandler::s_data = WindowHandler::Data();
HWND WindowHandler::s_hwnd = HWND();

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WindowHandler::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static WindowHandler* windowHandler = nullptr;

	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;


	if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}
	else if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		windowHandler = reinterpret_cast<WindowHandler*>(createStruct->lpCreateParams);
		//Input::GetInstance().SetWindow(&s_hwnd);
	}
	// Might wanna use later
	//else if (uMsg == WM_DROPFILES) {
	//	HDROP drop = (HDROP)wParam;
	//	UINT filePathCount = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 512);
	//	wchar_t* fileName = nullptr;
	//	UINT longestFileNameLength = 0;
	//	for (UINT i = 0; i < filePathCount; ++i)
	//	{
	//		//If NULL as the third parameter: return the length of the path, not counting the trailing '0'
	//		UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
	//		if (fileNameLength > longestFileNameLength)
	//		{
	//			longestFileNameLength = fileNameLength;
	//			fileName = (wchar_t*)realloc(fileName, longestFileNameLength * sizeof(*fileName));
	//		}
	//		DragQueryFileW(drop, i, fileName, fileNameLength);
	//
	//		// From Windows UTf-16 to multibyte
	//		char charBuf[2048];
	//		HRESULT hr = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, fileName, -1, charBuf, 2048, "?", NULL);
	//		CHECK_OK(hr, "Convert dropped path to multibyte stream");
	//
	//		std::string dropFileType(charBuf);
	//		dropFileType = dropFileType.substr(dropFileType.find("Art"));
	//		if (dropFileType.find(".fbx") != std::string::npos) {
	//			myFbxDrop = dropFileType;
	//			myTextureDrop = "";
	//		}
	//		else if (dropFileType.find(".DDS") != std::string::npos) {
	//			myTextureDrop = dropFileType;
	//			myFbxDrop = "";
	//		}
	//		else {
	//			myTextureDrop = "";
	//			myFbxDrop = "";
	//		}
	//	}
	//	free(fileName);
	//	DragFinish(drop);
	//}
	//else
	//{
	//	Input::GetInstance().UpdateEvents(uMsg, wParam, lParam);
	//}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

WindowHandler::WindowHandler(WindowHandler::Data data)
{
	// DurationTimer timer(__FUNCSIG__, true);
	s_data = data;

	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowHandler::WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"stuff";
	RegisterClass(&windowClass);

	const wchar_t* windowTitle = L"Untitled Game";
	s_hwnd = CreateWindow(L"stuff", windowTitle, WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
		s_data.m_x, s_data.m_y, s_data.m_width, s_data.m_height, nullptr, nullptr, nullptr, this);

	//ShowWindow(s_hwnd, )
	UpdateWindow(s_hwnd);
}

WindowHandler::~WindowHandler()
{
}
