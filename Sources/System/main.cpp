// ì¬“ú : 2024/05/21 17:00
// ì¬Ò : “c’†ƒ~ƒmƒ‹
// ŠT—v   : entry point
#include "Application.h"

#ifndef _DEBUG
int main(void)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	auto& app = Application::Instance();
	if (!app.Init()) { return -1; }

	app.Run();
	app.Terminate();
	return 0;
}
