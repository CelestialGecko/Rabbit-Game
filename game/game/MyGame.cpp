#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)	
{
	livesCount = 3;
	score = 0;
	timer = 0;
	options = false;
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	if (IsMenuMode())return;
	player.Update(t);
}

void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		mainMenuBG.Draw(g);
		if (options)
		{
			// draw options menu
		}
		else
		{
			// draw main menu
			startButton.Draw(g);
			optionsButton.Draw(g);
			titleText.Draw(g);
		}
	}

	player.Draw(g);
	for (CSprite* s : tiles)
	{
		s->Draw(g);
	}
	for (CSprite* s : bats)
	{
		s->Draw(g);
	}
	for (CSprite* s : sandWorms)
	{
		s->Draw(g);
	}


	// Game UI
	lives.Draw(g);
	*g << top << left << "Score: " << score;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// main menu and stuff
	mainMenuBG.SetImageFromFile("MainMenu.png");
	mainMenuBG.SetPosition(400, 300);
	titleText.SetImageFromFile("TitleText.png");
	titleText.SetPosition(400, 500);
	optionsButton.SetSize(700, 150);
	startButton.SetImageFromFile("MainMenuClick.png");
	startButton.SetPosition(400, 100);
	startButton.SetSize(300, 50);
	optionsButton.SetImageFromFile("OptionsClick.png");
	optionsButton.SetPosition(400, 50);
	optionsButton.SetSize(200, 25);

	// players animations
	player.LoadAnimation("PlayerIdle.png", "idle", CSprite::Sheet(4, 1).Row(0).From(0).To(4), CColor::Black());
	player.LoadAnimation("PlayerWalk.png", "walk", CSprite::Sheet(6, 1).Row(0).From(0).To(6), CColor::Black());
	player.LoadAnimation("PlayerRun.png", "run", CSprite::Sheet(6, 1).Row(0).From(0).To(6), CColor::Black());
	player.LoadAnimation("PlayerJump.png", "jump", CSprite::Sheet(3, 1).Row(0).From(0).To(3), CColor::Black());
	player.LoadAnimation("PlayerAttack.png", "attack", CSprite::Sheet(3, 1).Row(0).From(0).To(3), CColor::Black());
	player.SetAnimation("idle");
	player.SetPos(400, 300);

	// Level design/gameplay. This is where you work Carla
	// if you look in the h file you will see we have pointer lists, if an object is solid it needs to also
	// go in the solidObstcles list, if it is deadly it needs to go in the deadlyObstcles list
	// all objects go in tiles though

	CSprite* b = new CSprite(CRectangle(100, 100, 100, 100), " ", CColor::Black(), GetTime());
	b->SetSize(b->GetSize() * 3);
	tiles.push_back(b);
	solidObstcles.push_back(b);
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	
	//StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
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
	if (sym == SDLK_F2)
		NewGame();
	if (sym == SDLK_ESCAPE) {
		options = false;
		if (IsGameMode())PauseGame();
	}
	if (sym == SDLK_o) {
		options = true;
	}
	if (sym == SDLK_SPACE) {
		StartGame();
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
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
