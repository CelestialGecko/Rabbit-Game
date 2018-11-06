#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	srand( (unsigned)time( NULL ) );

	//app.DebugMemoryLeaks();
	app.OpenWindow(800, 600, "SPACE BATTLE 2");
	//app.OpenFullScreen(800, 600, 24);
	//app.OpenConsole();
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
