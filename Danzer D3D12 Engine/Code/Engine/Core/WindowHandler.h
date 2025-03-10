#pragma once
#include <Windows.h>

#include "DirectX/include/directx/d3d12.h"

// Class that handles the games Window
class WindowHandler
{
public:
	struct Data {
		int m_x = 0;
		int m_y = 0;
		unsigned int m_w = 1920;
		unsigned int m_h = 1080;
		bool m_windowed = true;

		const float AspectRatio() {
			return m_w / m_h;
		}
	};

	static LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	WindowHandler() {}
	WindowHandler(Data data);
	~WindowHandler();

	static HWND GetHWND()	 { return s_hwnd; }
	static Data WindowData() { return s_data; }

	static D3D12_VIEWPORT GetViewPort();

private:
	static bool m_windowed;
	static HWND s_hwnd;
	static Data s_data;
};

