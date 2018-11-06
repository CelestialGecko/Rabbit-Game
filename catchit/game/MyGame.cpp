#include "stdafx.h"
#include "MyGame.h"
#include <sstream>

CMyGame::CMyGame(void) : ball(500, 600, "ball.bmp", CColor::Black(), GetTime()), 
	                     paddle(400,10, "paddle.bmp", CColor::Black(), GetTime())
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
	if (IsMenuMode() || IsGameOver()) return;

	Uint32 t = GetTime();

	// ----- updating sprites -----------

	// updating ball
	ball.Update( t);

	// updating paddle
	paddle.Update(t);



	// --------- game rules ---------------

	// test if ball has hit the paddle
	if (ball.HitTest(&paddle))
	{
		// task 1: increment score
		
		// set ball to a new random position at the top
		ball.SetPosition(float(rand() % 800), 600);

		// task 2: increase speed each time the ball has been caught
		
	}

	// ball hit the bottom of the screen, we did not catch the ball, thus game over
	if (ball.GetY() < 0)
	{
       GameOver();
	}

}

void CMyGame::OnDraw(CGraphics* g)
{
		// draw rock
		ball.Draw( g);

        // draw paddle
	    paddle.Draw( g);

		//fruit.Draw(g);

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
	// set ball initial position
	ball.SetPosition(float(rand() % 800), 600);

	// set rock motion downwards 300 pixels/sec
	ball.SetVelocity(0, -300);

	// set score to 0
	score = 0;

	 // now start the game
	StartGame();
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
	paddle.SetX( x);
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
