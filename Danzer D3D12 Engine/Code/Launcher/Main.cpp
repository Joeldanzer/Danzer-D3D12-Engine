#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <thread>

#include "..\Editor\Editor.h"
#include "Core\Engine.h"
#include "Core\Input.hpp"
#include "Core/ResourceLoadingHandler.h"
#include "..\Game\Game.h"

void DebugWindow() {
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma warning ( push )
#pragma warning( disable : 6031 )
	AllocConsole();
	freopen("CONIN$", "r",  stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
#pragma warning( pop )
#pragma warning( pop )
}

class Launcher {
public:
	Launcher() {
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		WindowHandler::Data windowData;
		windowData.m_w = desktop.right;
		windowData.m_h = desktop.left;
		
		// Initialise engine through ::GetInstance
		Engine::GetInstance();

		Editor editor(Engine::GetInstance());
		Game   game(Engine::GetInstance());

		//Engine::GetInstance().EndInitFrame();
		//RLH& rlh = RLH::Instance();


		while (true) {

			Input::GetInstance().Update();

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;

				Input::GetInstance().UpdateEvents(msg.message, msg.wParam, msg.lParam);
			}

			if (GetAsyncKeyState(VK_ESCAPE)) {
				break;
			}

			Engine::GetInstance().BeginFrame();

			game.Update(Engine::GetInstance().GetDeltaTime());
			editor.Update(Engine::GetInstance().GetDeltaTime());

			Engine::GetInstance().UpdateFrame();
		}

	}
};

int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd) 
{
	hInstance; hPrevInstance; lpCmdLine; nShowCmd;

#ifdef DEBUG
	DebugWindow();
#endif // _DEBUG

	Launcher launcher;

	return 0;
}