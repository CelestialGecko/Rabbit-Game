#include "stdafx.h"
#include "MyGame.h"
#include <sstream>

CMyGame::CMyGame(void) : player(400,20, "greenmonster.bmp", CColor::Black(), 0)
{
	// TODO: add initialisation here

	// set score to zero
	score=0;

}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsGameOverMode()) return;
	
	Uint32 t = GetTime();

	// ----- updating sprites -----------

	player.Update(t);

	// --------- game rules ---------------


}

void CMyGame::OnDraw(CGraphics* g)
{
	   // draw background

	   // draw player
       
	    player.Draw( g);
		
		// draw all fruit here
		

		// print score at position xy(50,570)
		*g << font(28) << color(CColor::Red()) << xy(10,570) << score;

		// print game over at at the centre of the screen
		if ( IsGameOverMode())
			*g << font(46) << color(CColor::Green()) << vcenter << center << " GAME OVER ";
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{


}

// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	player.SetOmega(200);

	// set score to 0
	score = 0;


	music.Play("music.wav", -1); // play continuously with (-1)

	// now start the game
	StartGame();
}



// called when Game Mode entered
void CMyGame::OnStartGame()
{
	HideMouse();
}

// called when Game is Over
void CMyGame::OnGameOver()
{
	ShowMouse();
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	// synchronise horizontal paddle position with the mouse position 
	if (IsGameMode()) player.SetPosition(x,y);
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
