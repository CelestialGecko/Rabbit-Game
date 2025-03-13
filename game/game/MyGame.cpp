#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void)	: player(CRectangle(100, 100, 200, 40), "CutScene.png", GetTime())
{
	livesCount = 3;
	score = 0;
	timer = 0;
	options = false;
	wL = false;
	wR = false;
	jump = false;
	playCutscene = false;
	timerCut = 0;
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here

	// push test
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();
	if (IsMenuMode()) {
		rileyGlow.SetPos(riley.GetPos());
		rogerGlow.SetPos(roger.GetPos());
		riley.Update(t);
		roger.Update(t);
		rileyGlow.Update(t);
		rogerGlow.Update(t);
		speechBubble.Update(t);
		return;
	}
	PlayerControl();
}

void CMyGame::PlayerControl() {
	static bool jumpAir = false;
	
	// player controls - this almost killed me getting it to work 
	if (IsKeyDown(SDLK_LEFT) || IsKeyDown(SDLK_a)) {
		// set walking left animation if not already set
		if (!wL) {
			playerAni.SetAnimation("walkL");
			wL = true;
			wR = false;
		}
		if (!attack)player.SetXVelocity(-120);

		// set running left animation if CTRL key is held down
		if (IsKeyDown(SDLK_LCTRL)) {
			if(!attack)player.SetXVelocity(-240);
			if (!wL) {
				playerAni.SetAnimation("runL");
			}
		}
	}
	else if (IsKeyDown(SDLK_RIGHT) || IsKeyDown(SDLK_d)) {
		// set walking right animation if not already set
		if (!wR) {
			playerAni.SetAnimation("walkR");
			wR = true;
			wL = false;
		}
		if (!attack)player.SetXVelocity(120);

		// set running right animation if CTRL key is held down
		if (IsKeyDown(SDLK_LCTRL)) {
			if (!attack)player.SetXVelocity(240);
			if (!wR) {
				playerAni.SetAnimation("runR");
			}
		}
	}
	else {
		// stop the player and set idle animation if moving
		player.SetXVelocity(0);
		if (wL || wR) {
			playerAni.SetAnimation("idle");
			wL = false;
			wR = false;
		}
	}



	if ((IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP)) && jump) {

		player.SetYVelocity(1000);
		if (wR)playerAni.SetAnimation("jumpR", 6);
		else playerAni.SetAnimation("jumpL", 6);
		jumpAir = true;
		jump = false;
	}
	if (player.GetYVelocity() > -200) {
		player.Accelerate(0, -100);
	}

	//if (player.GetYVelocity() >= 0)std::cout << player.GetYVelocity() << std::endl;

	CVector p = player.GetPos();

	if (attack)	playerAni.SetPos(player.GetPos() + CVector(0, -2));
	else playerAni.SetPos(player.GetPos() + CVector(0, 5));
	player.Update(GetTime());
	playerAni.Update(GetTime());

	jump = false;
	// player collision with solid objects
	int h = player.GetHeight() / 2 - 1;
	for (CSprite* s : solidObstcles) {
		if (player.HitTest(s)) {
			// top section of the block
			if (p.m_y >= s->GetTop() + h) {
				player.SetY(s->GetTop() + h);
				jump = true;
				if (jumpAir) {
					jumpAir = false;
					if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
						playerAni.SetAnimation("walkL");
					}
					else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)) {
						playerAni.SetAnimation("walkR");
					}
					else {
						playerAni.SetAnimation("idle");
					}
					attack = false;
				}
			}
			// not sure if this works yet as there is no jumping
			else if (p.m_y <= s->GetBottom() - h && player.GetXVelocity() > 0) {
				player.SetY(s->GetBottom() - h);
			}
			// sides 
			else if (p.m_x < s->GetLeft()) {
				player.SetX(s->GetLeft() - player.GetWidth() / 2);
			}
			else if (p.m_x > s->GetRight()) {
				player.SetX(s->GetRight() + player.GetWidth() / 2);
			}
		}
	}
}


void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		if (playCutscene) {
			// initialise the cutscene
			if (timerCut == 0) {
				music.Play("CutScene.wav", 9999);
				music.Volume(0.4);
				riley.SetPos(300, 275);
				roger.SetPos(200, 280);
				riley.SetXVelocity(100);
				riley.SetAnimation("walkR");
				speechBubble.LoadImage("SpeechUI.png", "riley", CSprite::Sheet(1, 2).Tile(0, 0));
				speechBubble.LoadImage("SpeechUI.png", "roger", CSprite::Sheet(1, 2).Tile(0, 1));
				speechBubble.SetImage("roger");
				speechBubble.SetPos(300, 100);
			}

			cutScreenBG.Draw(g);
			rogerGlow.Draw(g);
			roger.Draw(g);
			rileyGlow.Draw(g);
			riley.Draw(g);

			if (timerCut > 2 && timerCut < 5) {
				if (timerCut < 2.016f) {
					riley.SetXVelocity(0);
					riley.SetAnimation("idle");
					sfx.Play("rogerHappy.wav");
				}
				speechBubble.Draw(g);
				*g << font(30) << color(CColor::Black()) << xy(210, 120) << "Oi, just where do you think";
				*g << font(30) << color(CColor::Black()) << xy(210, 70) << "you are going?!";
			}

			if (timerCut > 5.5 && timerCut < 8) {
				if (timerCut < 5.516f) {
					speechBubble.SetImage("riley");
					speechBubble.SetPos(500, 100);
					sfx.Play("rileyHappy.wav");
				}
				speechBubble.Draw(g);
				*g << font(30) << color(CColor::Black()) << xy(410, 120) << "Our burrow just isn't safe";
				*g << font(30) << color(CColor::Black()) << xy(410, 70) << "anymore";
			}

			// timer for different events
			timerCut += 0.016f;

			// helps keep track of stuff
			*g << font(20) << color(CColor::White()) << top << left << "Tim: " << timerCut;
			if (timerCut > 40) StartGame();
			return;
		}
		
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
		return;
	}

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
	//player.Draw(g);
	playerAni.Draw(g);


	// Game UI
	lives.Draw(g);
	*g << top << left << "Score: " << score;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// where she/her makes the UI and player

	cutScreenBG.SetImageFromFile("CutScene.png");
	cutScreenBG.SetPos(400, 300);

	// music 
	music.Play("MenuMusic.wav", 9999);

	// cutscene
	riley.LoadAnimation("PlayerIdleL.png", "idle", CSprite::Sheet(4, 1).Row(0).From(0).To(4), CColor::Black());
	riley.LoadAnimation("cPlayerWalk.png", "walkR", CSprite::Sheet(12, 1).Row(0).From(0).To(5), CColor::Black());
	riley.LoadAnimation("cPlayerWalk.png", "walkL", CSprite::Sheet(12, 1).Row(0).From(6).To(11), CColor::Black());
	riley.SetAnimation("idle");

	rileyGlow.SetImageFromFile("bgGlow.png");
	rogerGlow.SetImageFromFile("bgGlow.png");

	roger.LoadAnimation("daddyIdle.png", "idle", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Black());
	roger.LoadAnimation("daddyWalk.png", "walk", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Black());
	roger.SetAnimation("idle");

	speechBubble.SetImageFromFile("speech.png");
	speechBubble.SetSize(300, 200);

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

	player.SetSize(30, 50);
	// players animations
	playerAni.LoadAnimation("PlayerIdleR.png", "idle", CSprite::Sheet(4, 1).Row(0).From(0).To(4), CColor::Black());

	playerAni.LoadAnimation("PlayerWalk.png", "walkR", CSprite::Sheet(12, 1).Row(0).From(0).To(5), CColor::Black());
	playerAni.LoadAnimation("PlayerWalk.png", "walkL", CSprite::Sheet(12, 1).Row(0).From(6).To(11), CColor::Black());

	playerAni.LoadAnimation("PlayerRun.png", "runR", CSprite::Sheet(12, 1).Row(0).From(0).To(5), CColor::Black());
	playerAni.LoadAnimation("PlayerRun.png", "runL", CSprite::Sheet(12, 1).Row(0).From(6).To(11), CColor::Black());

	playerAni.LoadAnimation("PlayerJump.png", "jumpR", CSprite::Sheet(6, 1).Row(0).From(0).To(2), CColor::Black());
	playerAni.LoadAnimation("PlayerJump.png", "jumpL", CSprite::Sheet(6, 1).Row(0).From(3).To(5), CColor::Black());

	playerAni.LoadAnimation("PlayerAttack.png", "attackR", CSprite::Sheet(6, 1).Row(0).From(0).To(2), CColor::Black());
	playerAni.LoadAnimation("PlayerAttack.png", "attackL", CSprite::Sheet(6, 1).Row(0).From(3).To(5), CColor::Black());

	playerAni.SetAnimation("idle");
	player.SetPos(400, 300);
	playerAni.SetPos(player.GetPos());

	// Level design/gameplay. This is where you work Karl Marx
	// if you look in the h file you will see we have pointer lists, if an object is solid it needs to also
	// go in the solidObstcles list, if it is deadly it needs to go in the deadlyObstcles list
	// all objects go in tiles though

	// ass you can see I have created some tile pointers for you, all you need to do is clone them and then pick their location
	// you will need to create some new pointer blocks for any extra tiles I havent done already

	// first few are done for you so you understand what im doing
	// 1 grid space is 40, the sheet is 9 by 4 but you may wish to make bigger sprites using lets say 3 by 1 as shown
	// I would simplify this with functions however im too lazy

	// Create and Define Blocks from Tilesheet \\
	
	// "Normal" block
	CSprite* defBlock = new CSprite();
	defBlock->LoadImage("CaveTileset.png", "i", CSprite::Sheet(3, 1).Tile(0, 0), CColor::Black());
	defBlock->SetImage("i");
	defBlock->SetSize(120, 160);

	// Small block
	CSprite* babyBlock = new CSprite();
	babyBlock->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(3, 1), CColor::Black());
	babyBlock->SetImage("i");
	babyBlock->SetSize(40, 40);

	// Torch
	CSprite* torch1 = new CSprite();
	torch1->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(8, 1), CColor::Black());
	torch1->SetImage("i");
	torch1->SetSize(40, 40);
	// Purple Rock Thing
	CSprite* purpleRock = new CSprite();
	purpleRock->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(7, 1), CColor::Black());
	purpleRock->SetImage("i");
	purpleRock->SetSize(40, 40);

	// Green Rock Thing
	CSprite* greenRock = new CSprite();
	greenRock->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(7, 0), CColor::Black());
	greenRock->SetImage("i");
	greenRock->SetSize(40, 40);

	// Purple Crystal
	CSprite* greenCrystal = new CSprite();
	greenCrystal->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(7, 2), CColor::Black());
	greenCrystal->SetImage("i");
	greenCrystal->SetSize(40, 40);

	// Purple Crystal
	CSprite* purpleCrystal = new CSprite();
	purpleCrystal->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(7, 3), CColor::Black());
	purpleCrystal->SetImage("i");
	purpleCrystal->SetSize(40, 40);

	// POV: Rock tells a joke
	// Haha, classic rock
	CSprite* rock = new CSprite();
	rock->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(8, 3), CColor::Black());
	rock->SetImage("i");
	rock->SetSize(40, 40);

	// Dynamite Stick
	CSprite* dynamiteStick = new CSprite();
	dynamiteStick->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(8, 2), CColor::Black());
	dynamiteStick->SetImage("i");
	dynamiteStick->SetSize(40, 40);

	// minecart
	CSprite* minecart = new CSprite();
	minecart->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(4, 0), CColor::Black());
	minecart->SetImage("i");
	minecart->SetSize(40, 40);

	// TNT
	CSprite* TNT = new CSprite();
	TNT->LoadImage("CaveTileset.png", "i", CSprite::Sheet(9, 4).Tile(3, 0), CColor::Black());
	TNT->SetImage("i");
	TNT->SetSize(40, 40);

	// level design or smt idk
	// some of the lists may need changing, i put all rocks / crystals as collidable, tnt as deadly etc but im not sure.
	// mans tired.

	// the creator of this fine world
	CSprite* god;

	// removed them for now just while I work on the player
	god = defBlock->Clone();
	god->SetPos(400, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(400, 75);

	god = defBlock->Clone();
	god->SetPos(300, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(500, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(600, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);
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
	music.Play("GameplayMusic.wav", 999, 1);
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

	// this was so fucking painful, holy shit

	if (sym == SDLK_s && IsMenuMode())StartGame();

	if (sym == SDLK_LEFT || sym == SDLK_a) {
		if (!wL) {
			playerAni.SetAnimation("walkL");
			wL = true;
			wR = false;
		}
		attack = false;
	}
	if (sym == SDLK_RIGHT || sym == SDLK_d) {
		if (!wR) {
			playerAni.SetAnimation("walkR");
			wR = true;
			wL = false;
		}
		attack = false;
	}
	if (sym == SDLK_LCTRL) {
		if (wL && !wR) {
			playerAni.SetAnimation("runL");
		}
		else if (wR && !wL) {
			playerAni.SetAnimation("runR");
		}
	}
	if (sym == SDLK_ESCAPE) {
		options = false;
		if (IsGameMode()) PauseGame();
	}
	if (sym == SDLK_o) {
		options = true;
	}
	if (sym == SDLK_SPACE) {
		playCutscene = true;
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_LEFT || sym == SDLK_a) {
		if (wL) {
			playerAni.SetAnimation("idle");
			wL = false;
		}
	}
	if (sym == SDLK_RIGHT || sym == SDLK_d) {
		if (wR) {
			playerAni.SetAnimation("idle");
			wR = false;
		}
	}
	if (sym == SDLK_LCTRL) {
		if (wL && !wR) {
			playerAni.SetAnimation("walkL");
		}
		else if (wR && !wL) {
			playerAni.SetAnimation("walkR");
		}
	}
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	attack = true;
	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
		playerAni.SetAnimation("attackL");
		player.SetXVelocity(0);
	}
	else {
		playerAni.SetAnimation("attackR");
		player.SetXVelocity(0);
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	attack = false;
	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
		playerAni.SetAnimation("walkL");
	}
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)){
		playerAni.SetAnimation("walkR");
	}
	else {
		playerAni.SetAnimation("idle");
	}
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
