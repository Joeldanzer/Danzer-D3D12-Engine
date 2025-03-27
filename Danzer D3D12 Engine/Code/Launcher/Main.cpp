#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <thread>

#include "..\Editor\Editor.h"
#include "..\Engine\Core\Engine.h"
#include "..\Engine\Core\Input.hpp"
#include "..\Engine\Physics\Assignment\SimplePhysics.h"
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
		Engine& engine = Engine::GetInstance();

		Editor		  editor(Engine::GetInstance());
		Game		  game(Engine::GetInstance());
		SimplePhysics physics;

		Engine::GetInstance().EndInitFrame();
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

			engine.BeginFrame();

			game.Update(engine.GetDeltaTime());
			editor.Update(engine.GetDeltaTime());

			physics.SimulatePhysics(engine.GetDeltaTime());

			engine.UpdateFrame();
		}

	}
};

int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd) {
	hInstance; hPrevInstance; lpCmdLine; nShowCmd;

#ifdef DEBUG
	DebugWindow();
#endif // _DEBUG

	Launcher launcher;

	return 0;
}