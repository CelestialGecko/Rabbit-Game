#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : theRocket(CVector(50, 300), "rocket.bmp", CColor::Blue(), 0)
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
	theEnemies.delete_all();
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	theRocket.Update(GetTime());

	if (rand() % 60 == 0)
	{
		CSprite *pSprite = new CSprite(CVector(850, rand() % 600), "enemy.bmp", CColor::Blue(), GetTime());
		pSprite->SetVelocity(-100, 0);
		theEnemies.push_back(pSprite);
	}

	for each (CSprite *pSprite in theEnemies)
	{
		pSprite->Update(GetTime());

		if (theRocket.HitTest(pSprite))
			GameOver();
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
	theRocket.Draw(g);
	for each (CSprite *pSprite in theEnemies)
		pSprite->Draw(g);

	if (IsGameOverMode())
		*g << font(40) << color(CColor::Red()) << vcenter << center << "GAME OVER" << endl
		<< font(24) << timetext(GetTimeGameOver());
	else
		*g << font(18) << color(CColor::Blue()) << bottom << timetext(GetTime());

}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	theRocket.Rotate(90);
}

// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	StartGame();	// this allows to start the game immediately
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
}

// called when Game is Over
void CMyGame::OnGameOver()
{
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
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
	if (sym == SDLK_UP)
		theRocket.SetVelocity(0, 100);
	if (sym == SDLK_DOWN)
		theRocket.SetVelocity(0, -100);
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_UP)
		theRocket.SetVelocity(0, 0);
	if (sym == SDLK_DOWN)
		theRocket.SetVelocity(0, 0);
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x, Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x, Uint16 y)
{
}
