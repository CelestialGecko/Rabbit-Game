#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	delete pMyRocket;
	spaceships.delete_all();
	missiles.delete_all();
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsGameOverMode())
		return;

	// create enemy sprites - on average, once every 40th frame
	if (rand() < RAND_MAX / 40)
	{
		// Create enemy battleship
		CSprite *pNewShip = new CSprite((float)(800 * rand() / RAND_MAX), 650.f, "spaceship.bmp", CColor::Black(), GetTime());
		pNewShip->SetVelocity(0, -100);
		spaceships.push_back(pNewShip);
	}

	// Update All Sprites...
	for (CSprite *pShip : spaceships)
		pShip->Update(GetTime());
	for (CSprite *pMissile : missiles)
		pMissile->Update(GetTime());
	pMyRocket->Update(GetTime());

	// For each enemy ship, check for collisions
	for (CSprite *pShip : spaceships)
	{
		// with the rocket - game over!
		if (pMyRocket->HitTest(pShip))
		{
			GameOver();
			pShip->Delete();
			pMyRocket->SetPos(-100, -100);

			m_playerMusic.Play("explode.wav");
			m_playerMusic.Play("music2.wav", -1, 5000);
		}

		// with missiles
		for (CSprite *pMissile : missiles)
			if (pShip->HitTest(pMissile))
			{
				pShip->Delete();
				pMissile->Delete();

				m_playerExplode.Play("explode.wav");
				m_nScore += 10;
			}
	}

	// remove enemy sprites that flied away of the screen
	for (CSprite *pShip : spaceships)
		if (pShip->GetY() < -50)
			pShip->Delete();
	
	// remove missiles that flied away of the screen
	for (CSprite *pMissile : missiles)
		if (pMissile->GetY() > 420)
			pMissile->Delete();

	spaceships.delete_if(deleted);
	missiles.delete_if(deleted);
}

void CMyGame::OnDraw(CGraphics* g)
{
	// Draw All Sprites...
	for (CSprite *pShip : spaceships)
		pShip->Draw(g);
	for (CSprite *pMissile : missiles)
		pMissile->Draw(g);

	pMyRocket->Draw(g);

	// Texts
	if (IsGameOverMode())
	{
		int nTimeCredit = (GetTimeGameOver() / 10000) * 10;
		*g << font(48) << color(CColor::Red()) << vcenter << center << "GAME OVER" << endl;
		*g << font(24);
		*g << "You survived: " << GetTimeGameOver() / 1000 << " seconds" << endl;
		*g << "Time Credit: " << nTimeCredit << endl;
		*g << "Score: " << m_nScore + nTimeCredit << flush;
	}
	else
	{
		*g << font(18) << color(CColor::Blue());
		*g << bottom << "Time elapsed: " << timetext(GetTime());
		*g << right << "Score: " << m_nScore << flush;
	}
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
	delete pMyRocket;
	spaceships.delete_all();
	missiles.delete_all();

	pMyRocket = new CSprite(CVector(400, 70), "rocket.bmp", CColor::Blue(), GetTime());
	StartGame();
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
	m_nScore = 0;
	m_playerMusic.Play("music.wav", -1);
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

	// Steer the rocket
	if (sym == SDLK_LEFT)
		pMyRocket->Accelerate(-100, 0);
	if (sym == SDLK_RIGHT)
		pMyRocket->Accelerate(100, 0);
	if (sym == SDLK_SPACE && !IsGameOver())
	{
		float x = pMyRocket->GetX();
		float y = pMyRocket->GetTop();
		CSpriteOval *pNewMissile = new CSpriteOval(x, y, 10, 10, CColor::White(), GetTime());
		pNewMissile->SetVelocity(0, 250);
		missiles.push_back(pNewMissile);
		m_playerShot.Play("shot.wav");
		m_nScore--;
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_LEFT)
		pMyRocket->Accelerate(100, 0);
	if (sym == SDLK_RIGHT)
		pMyRocket->Accelerate(-100, 0);
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
