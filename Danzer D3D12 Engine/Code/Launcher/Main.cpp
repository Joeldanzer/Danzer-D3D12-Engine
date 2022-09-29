#include <windows.h>
#include <strsafe.h>
#include <iostream>

#include "..\Editor\Editor.h"
#include "..\Engine\Core\Engine.h"
#include "..\Engine\Core\Input.hpp"
#include "..\Game\Game.h"

void DebugWindow() {
 // The only warnings that we are disabling
#pragma warning( push )
#pragma warning( disable : 4996 )
#pragma warning ( push )
#pragma warning( disable : 6031 )
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
#pragma warning( pop )
#pragma warning( pop )
}

int main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd) 
{
	hInstance; hPrevInstance; lpCmdLine; nShowCmd;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	DebugWindow();

	// Will thread this

	//Launch Engine stuff!
	Engine engine = Engine(1920, 1080);

	Editor editor(engine);

	Game game(engine);

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

		game.Update(engine.GetDeltaTime());
		engine.Update();
		editor.Update(engine.GetDeltaTime());
		
		engine.LateUpdate();

		// Plans for this engine perhaps?
		// #ifdef _DEBUG
		// if(editor.StartGame()){
		// Change size of window and other stuff
		// game.update()
		// } 
		// else {
		//	editor.Update()
		// }
		//#endif
	}
	
	return 0;
}