#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) :
	player(300, 20, "spaceship.bmp", CColor::Black(), 0),
	enemy(400, 500, "enemy.bmp", CColor::Black(), 0)
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
	score = 0;
}


CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsMenuMode() || IsGameOver()) return;

	Uint32 t = GetTime();

	// update sprites here
	player.Update(t);
	enemy.Update(t);


	// update shots list
	for (CSprite* shot : shots)
	{
		shot->Update(t);
	}


	// ---- game rules -----

	if (enemy.GetY() < 0)
	{
		enemy.SetPosition(float(rand() % 800), 600);
		score = score - 2;
	}

	if (score < 0) GameOver();

	for (CSprite* shot : shots)
	{
		// delete a shot if it has reached the top of the screen
		if (shot->GetY() > 600) shot->Delete();

		// delete shots if there is a collision with the enemy
		if (shot->HitTest(&enemy,1))
		{
			shot->Delete(); // set sprite to be deleted 
			enemy.SetPosition(float(rand() % 800), 600);
			score++;
			
		}
	}
    // remove deleted sprites from list
	shots.remove_if(deleted);

	// --- player control ----------
	// if left key pressed move left, if right key pressed move right, otherwise don't move
	if (IsKeyDown(SDLK_LEFT)) player.SetMotion(-400, 0);
	else if (IsKeyDown(SDLK_RIGHT)) player.SetMotion(400, 0);
	else player.SetMotion(0, 0);

}

void CMyGame::OnDraw(CGraphics* g)
{
	// TODO: add drawing code here
    // draw background here

	player.Draw(g);	

	enemy.Draw(g);

	for (CSprite* pSprite : shots) pSprite->Draw(g);

	// print score at position xy(50,570)
	*g << font(28) << color(CColor::Red()) << xy(10, 570) << score;

	// print game over at at the centre of the screen
	if (IsGameOverMode())
		*g << font(46) << color(CColor::Green()) << vcenter << center << " GAME OVER ";
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{

}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	player.SetPosition(300, 20);
	enemy.SetPosition(float(rand() % 800), 600);
	enemy.SetMotion(0, -150); 
	shots.clear();
	score = 0;


	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
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
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	// create a shot instance when left control key is pressed
	if (sym == SDLK_LCTRL)
	{
		// dynamically creating a new shot sprite
		CSprite* newShot = new CSprite(player.GetX(), player.GetTop(), "shot.bmp", CColor::White(), GetTime());
		// set the motion of the new shot sprite
		newShot->SetMotion(0,400);
		// add the dynamically created shot sprite to the list
		shots.push_back(newShot);
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
