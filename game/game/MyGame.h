#pragma once

class CMyGame : public CGame
{
private:
	// game variables
	double timer;
	int score;
	int livesCount;
	bool options;
	bool jump;

	bool wL;
	bool wR;

	// game objects
	CSprite player;
	CSpriteVector tiles;
	CSpriteVector bats;
	CSpriteVector sandWorms;

	// bulk operations
	std::vector<CSprite*> solidObstcles;
	std::vector<CSprite*> deadlyObstcles;
	std::vector<CSprite*> deco;

	// backgrounds
	CSprite cutScreenBG;
	CSprite closeBG;
	CSprite farBG;

	// ui in game
	CSprite lives;

	// menu
	CSprite mainMenuBG;
	CSprite titleText;
	CSprite startButton;
	CSprite optionsButton;
	CSprite muteSoundButton;

	// music and sfx
	CSoundPlayer music;
	CSoundPlayer sfx;
public:
	// game functions

	void PlayerControl();
	void BatControl();
	void SandWormControl();

	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
