#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	app.OpenWindow(800, 600, "Burrow Exodus :3");
	//app.OpenFullScreen(800, 600, 24);
	app.OpenConsole();
	app.SetClearColor(CColor(113, 113, 125, 1));
	app.Run(&game);
	return(0);
}
