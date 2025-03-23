#pragma once

class CMyGame : public CGame
{
private:
	// game variables
	double timer;
	int score;
	int livesCount;
	bool dead;
	bool options;
	bool jump;
	bool attack;
	bool attRight;
	float vol;
	bool volMove;

	bool playCutscene;
	double timerCut;
	double timerDeath;
	bool playerBounce;
	bool resetGame;

	bool wL;
	bool wR;

	// game objects
	CSprite player;
	CSprite playerAni;
	CSprite house;
	CSpriteVector tiles;
	CSpriteVector enemies;
	CSpriteList particles;

	// bulk operations
	std::vector<CSprite*> solidObstcles;
	std::vector<CSprite*> deadlyObstcles;
	std::vector<CSprite*> collectables;
	std::vector<CSprite*> deco;

	// cutscene
	CSprite cutScreenBG;
	CSprite riley;
	CSprite rileyGlow;
	CSprite roger;
	CSprite rogerGlow;
	CSprite speechBubble;

	// backgrounds
	CSprite backL1;
	CSprite backL2;
	CSprite backL3;

	// ui in game
	CSprite lives;
	CSprite pause;

	// menu holders for each type
	CSpriteVector menuButtons;
	CSpriteVector menuUIstatic;
	// background
	CSprite background;
	CSprite deathScreen;
	// also contains the sprites original size for scaling effect
	std::unordered_map<CSprite*, std::pair<bool, CVector>> extraItemData;

	// music and sfx
	CSoundPlayer music;
	CSoundPlayer sfx;
	CSoundPlayer deathSoundPlayer;
public:
	// game functions

	// creating the ui
	void CreateNewElement(char* fileName, CVector&offset, char type, float sizeOffset = 1);
	void CreateNewElement(CRectangle& r, CColor& c);
	void PlaceElement(int item, CGraphics* g, bool d);
	// sound control
	void UpdateSound();

	// cutscene control
	void CutSceneControl(CGraphics* g);

	// big mans dead innit
	void CMyGame::Death(CGraphics* g);

	// player and enemy
	void PlayerControl();
	void ParticleControl(Uint32 t);

	CSprite* CreateBat();
	CSprite* CreateWorm();

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
