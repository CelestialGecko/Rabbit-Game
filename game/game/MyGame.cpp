#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void): player(CRectangle(100, 100, 200, 40), "CutScene.png", GetTime()), 
backL1(CRectangle(-10, -10, 1500, 1000), "backL1.png", GetTime()),
backL2(CRectangle(-10, -10, 1500, 1000), "backL2.png", GetTime()),
backL3(CRectangle(0, 0, 800, 600), "backL3.png", GetTime())
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
	vol = 1;
	volMove = false;
	playerBounce = false;
	attRight = true;
	resetGame = false;
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
	ParticleControl(t);
	if (IsMenuMode()) {
		if (playCutscene) {
			rileyGlow.SetPos(riley.GetPos());
			rogerGlow.SetPos(roger.GetPos());
			riley.Update(t);
			roger.Update(t);
			rileyGlow.Update(t);
			rogerGlow.Update(t);
			speechBubble.Update(t);
		}
		else{
			for (CSprite* b : menuButtons) b->Update(t);
			menuUIstatic.at(2)->Update(t);
		}
		return;
	}
	if (IsGameMode()) {
		PlayerControl();
		for (CSprite* b : enemies) {
			b->Update(t);
		}
		resetGame = false;
		// collectables logic
		for (CSprite* c : collectables) {
			if (c->HitTest(&player)) {
				c->Delete();
				sfx.Play("collect.wav");
				sfx.Volume(vol);
				score++;
			}
		}
		tiles.delete_if(deleted);

		for (CSprite* f : deadlyObstcles) {
			if (f->HitTest(&player)) {
				player.SetHealth(0);
			}
		}
		backL1.Update(t);
		backL2.Update(t);
		backL3.Update(t);

		if (player.GetHealth() == 0) GameOver();
	}
}

// looks a bit meh but it works
void CMyGame::ParticleControl(Uint32 t) {
    float x = rand() % 800 + 1;
    float y = 610;
    int s = rand() % 3 + 1;
    int h = rand() % 3 + 1;
    int ran;
	// new particle
    CSprite* newP = new CSprite(CRectangle(x, y, 4, 4), t);
    newP->SetImageFromFile("part.png");
    newP->SetState(s);
    newP->SetHealth(h);
    newP->SetYVelocity(-200);
	// varied size
    switch (h) {
        case 1:
            newP->SetSize(4, 4);
            break;
        case 2:
            newP->SetSize(0.5, 0.5);
            break;
        case 3:
            newP->SetSize(2, 2);
            break;
    }
    particles.push_back(newP);

    for (CSprite* p : particles) {
		// kill it
        if (p->GetY() < -10 || p->GetX() < 0 || p->GetX() > 800) {
            p->Delete();
            continue;
        }

        ran = rand() % 15;
		// might change its settings
        if (ran < 1) {
            p->SetState(rand() % 3 + 1);
        } else if (ran < 2) {
            p->SetHealth(rand() % 3 + 1);
        }

		// changes its movement
        switch (p->GetState()) {
            case 1:
                p->Accelerate(-3, 0);
                break;
            case 2:
                p->Accelerate(3, 0);
                break;
            case 3:
                p->SetXVelocity(p->GetXVelocity() * 0.8);
                break;
        }

		// changes its scale
        switch (p->GetHealth()) {
            case 1:
                p->SetSize(p->GetSize() * 0.99);
                break;
            case 2:
                if (p->GetSize().m_x < 4) p->SetSize(p->GetSize() * 1.01);
                break;
            case 3:
                // no size change
                break;
        }

        p->Update(t);
    }
    particles.delete_if(deleted);
}

void CMyGame::PlayerControl() {
	static bool jumpAir = false;
	static int jumpTim = 0;
	// player controls - this almost killed me getting it to work 
	if (IsKeyDown(SDLK_LEFT) || IsKeyDown(SDLK_a)) {
		// set walking left animation if not already set
		if (!wL) {
			playerAni.SetAnimation("walkL");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
			wL = true;
			wR = false;
		}
		if (!attack)player.SetXVelocity(-120);

		// set running left animation if CTRL key is held down
		if (IsKeyDown(SDLK_LCTRL)) {
			if(!attack)player.SetXVelocity(-240);
			if (!wL) {
				playerAni.SetAnimation("runL");
				sfx.Play("run.wav", 999);
				sfx.Volume(vol);
				player.SetState(1);
			}
		}
	}
	else if (IsKeyDown(SDLK_RIGHT) || IsKeyDown(SDLK_d)) {
		// set walking right animation if not already set
		if (!wR) {
			playerAni.SetAnimation("walkR");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
			wR = true;
			wL = false;
		}
		if (!attack)player.SetXVelocity(120);

		// set running right animation if CTRL key is held down
		if (IsKeyDown(SDLK_LCTRL)) {
			if (!attack)player.SetXVelocity(240);
			if (!wR) {
				playerAni.SetAnimation("runR");
				playerAni.SetAnimation("runL");
				sfx.Play("walk.wav", 999);
				player.SetState(1);
			}
		}
	}
	else {
		// stop the player and set idle animation if moving
		player.SetXVelocity(0);
		if (wL || wR) {
			playerAni.SetAnimation("idle");
			sfx.Stop();
			player.SetState(0);
			wL = false;
			wR = false;
		}
	}

	// allows the player to do a small jump, lets them jump off bats too
	if ((IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP)) && (jump || playerBounce)) {

		player.SetYVelocity(1000);
		sfx.Play("jump.wav");
		sfx.Volume(vol);
		if (wL)playerAni.SetAnimation("jumpL", 6);
		else playerAni.SetAnimation("jumpR", 6);
		jumpTim = 15;
		player.SetState(1);
		jumpAir = true;
		jump = false;
		playerBounce = false;
	}
	if (player.GetYVelocity() > -200) {
		player.Accelerate(0, -100);
	}
	// stops jump animation playing tons of times
	if (jumpTim != 0) {
		jumpTim--;
		if (jumpTim == 0) {
			if (wL)playerAni.SetAnimation("jumpL", 1, 2, 1);
			else playerAni.SetAnimation("jumpR", 6, 2, 1);
		}
	}

	CVector p = player.GetPos();

	// plays attack animation
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
					sfx.Play("land.wav");
					sfx.Volume(vol);
					if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
						playerAni.SetAnimation("walkL");
						sfx.Play("walk.wav", 999);
						sfx.Volume(vol);
						player.SetState(0);
					}
					else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)) {
						playerAni.SetAnimation("walkR");
						sfx.Play("walk.wav", 999);
						sfx.Volume(vol);
						player.SetState(0);
					}
					else {
						playerAni.SetAnimation("idle");
						sfx.Stop();
						player.SetState(0);
					}
					attack = false;
				}
			}
			// head bump
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

void CMyGame::CutSceneControl(CGraphics* g) {
	// initialise the cutscene
	if (timerCut == 0) {
		music.Play("CutScene.wav", 9999);
		music.Volume(std::clamp(static_cast<float>(vol - 0.4f), 0.0f, 1.0f));
		riley.SetPos(300, 275);
		roger.SetPos(200, 280);
		riley.SetXVelocity(125);
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

	if (timerCut > 2 && timerCut < 5.25) {
		if (timerCut < 2.016f) {
			riley.SetXVelocity(0);
			riley.SetAnimation("idle");
			sfx.Play("rogerHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 120) << "Oi, just where do you think";
		*g << font(30) << color(CColor::Black()) << xy(210, 70) << "you're going?!";
	}

	if (timerCut > 5.5 && timerCut < 8) {
		if (timerCut < 5.516f) {
			speechBubble.SetImage("riley");
			speechBubble.SetX(500);
			sfx.Play("rileyHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "Our burrow just isn't safe";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "anymore";
	}

	if (timerCut > 8 && timerCut < 12.25) {
		if (timerCut < 8.016f) {
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(408, 130) << "This whole mining operation";
		*g << font(30) << color(CColor::Black()) << xy(408, 90) << "- it's causing damage to";
		*g << font(30) << color(CColor::Black()) << xy(408, 50) << "our burrow!";
	}

	if (timerCut > 12.5 && timerCut < 15.5) {
		if (timerCut < 12.516f) {
			speechBubble.SetImage("roger");
			speechBubble.SetX(300);
			sfx.Play("rogerHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 120) << "It's fine, our house is";
		*g << font(30) << color(CColor::Black()) << xy(210, 70) << "still intact, isn't it?";
	}

	if (timerCut > 15.5 && timerCut < 18.25) {
		if (timerCut < 15.516f) {
			sfx.Play("rogerHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 120) << "A little 'mining' ain't";
		*g << font(30) << color(CColor::Black()) << xy(210, 70) << "ever worrying me, son.";
	}

	if (timerCut > 18.5 && timerCut < 22.5) {
		if (timerCut < 18.516f) {
			speechBubble.SetImage("riley");
			speechBubble.SetX(500);
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "I'm worried dad, and I'm";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "getting sick of you now.";
	}

	if (timerCut > 22.5 && timerCut < 24.75) {
		if (timerCut < 22.516f) {
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
			roger.SetXVelocity(50);
			roger.SetAnimation("walk");
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "Why can't we just retreat";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "to the surface?";
	}

	if (timerCut > 25 && timerCut < 28.25) {
		if (timerCut < 25.016f) {
			speechBubble.SetImage("roger");
			speechBubble.SetX(300);
			sfx.Play("rogerAngry.wav");
			sfx.Volume(vol);
			roger.SetXVelocity(0);
			roger.SetAnimation("idle");
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 120) << "It's too dangerous, don't";
		*g << font(30) << color(CColor::Black()) << xy(210, 70) << "you dare suggest that!";
	}

	// riley
	if (timerCut > 28.5 && timerCut < 32.5) {
		if (timerCut < 28.516f) {
			speechBubble.SetImage("riley");
			speechBubble.SetX(500);
			sfx.Play("rileyHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "And waiting for our burrow";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "to cave in isn't?";
	}

	// riley
	if (timerCut > 32.5 && timerCut < 35) {
		if (timerCut < 32.516f) {
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
			riley.SetXVelocity(-50);
			riley.SetAnimation("walkL");
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "You never give me a reason";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "why.";
	}

	// riley
	if (timerCut > 35 && timerCut < 38.25) {
		if (timerCut < 35.016f) {
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
			riley.SetXVelocity(0);
			riley.SetAnimation("idle");
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "How could it possibly be";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "any worse than this?";
	}

	// roger
	if (timerCut > 38.5 && timerCut < 40.5) {
		if (timerCut < 38.516f) {
			speechBubble.SetImage("roger");
			speechBubble.SetX(300);
			sfx.Play("rogerHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 90) << "You're too young, son...";
	}

	// riley
	if (timerCut > 40.5 && timerCut < 45) {
		if (timerCut < 40.516f) {
			speechBubble.SetImage("riley");
			speechBubble.SetX(500);
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(408, 130) << "Shut up! I'm tired of being";
		*g << font(30) << color(CColor::Black()) << xy(408, 90) << "trapped here like I'm some";
		*g << font(30) << color(CColor::Black()) << xy(408, 50) << "sort of prisoner.";
	}

	// riley
	if (timerCut > 45 && timerCut < 48.25) {
		if (timerCut < 45.016f) {
			sfx.Play("rileyAngry.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 120) << "I'm leaving this dump and";
		*g << font(30) << color(CColor::Black()) << xy(410, 70) << "nothing will stop me.";
	}

	// roger
	if (timerCut > 48.5 && timerCut < 51.5) {
		if (timerCut < 48.516f) {
			speechBubble.SetImage("roger");
			speechBubble.SetX(300);
			sfx.Play("rogerAngry.wav");
			sfx.Volume(vol);
			riley.SetAnimation("walkR");
			riley.SetXVelocity(100);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(210, 120) << "NO! Stop, don't leave its";
		*g << font(30) << color(CColor::Black()) << xy(210, 70) << "too danger-";
	}

	// riley
	if (timerCut > 51.5 && timerCut < 55) {
		if (timerCut < 51.516f) {
			speechBubble.SetImage("riley");
			speechBubble.SetX(500);
			sfx.Play("rileyHappy.wav");
			sfx.Volume(vol);
		}
		speechBubble.Draw(g);
		*g << font(30) << color(CColor::Black()) << xy(410, 90) << "Bye, Dad.";
	}

	// timer for different events
	timerCut += 0.016f;

	// helps keep track of stuff
	//*g << font(20) << color(CColor::White()) << top << left << "Tim: " << timerCut;
	*g << font(20) << color(CColor::White()) << top << right << "Press S to skip";
	if (timerCut > 55) StartGame();
}


void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode()){
		if (playCutscene) {
			CutSceneControl(g);
			return;
		}
		if (options){
			background.Draw(g);
			// draw options menu
			PlaceElement(3, g, true);
			PlaceElement(1, g, false);
			PlaceElement(4, g, true);
			PlaceElement(2, g, false);
			// dont feel like centering it |:
			g->FillRect(CRectangle(150, 95, 600, 300), CColor(131, 37, 212, 60), 10);
			*g << font(50) << color(CColor::White()) << xy(240, 340) << "CONTROLS:";
			*g << font(50) << color(CColor::White()) << xy(180, 300) << "W or up arrow for jump.";
			*g << font(50) << color(CColor::White()) << xy(175, 260) << "a and d or left and right";
			*g << font(50) << color(CColor::White()) << xy(240, 220) << "arrow for move.";
			*g << font(50) << color(CColor::White()) << xy(190, 180) << "ctrl key for running.";
			*g << font(50) << color(CColor::White()) << xy(220, 140) << "esc for pause.";
			*g << font(50) << color(CColor::White()) << xy(170, 100) << "mouse left click for attack.";
		}
		else{
			background.Draw(g);
			// draw main menu
			PlaceElement(0, g, false);
			PlaceElement(0, g, true);
			PlaceElement(1, g, true);
			PlaceElement(2, g, true);
		}
		return;
	}
	static CVector pP = player.GetPos();
	static CVector bP1 = backL1.GetPos();
	static CVector bP2 = backL2.GetPos();

    // game world 2400x2400: 1900 = 2400 - 800 + 300
	// Karl: if you need more space then make the world bigger
	// if you make it bigger and the parallax goes offscreen then you can manually make the image loger or change the size
	// size change may look weird so not recommended
    static const int leftScreenLimit = 300;
	static const int topScreenLimit = 300;
    static const int rightScreenLimit = 1900;
    static const int bottomScreenLimit = 1900;

    int scrolloffsetX = 0;
    int scrolloffsetY = 0;
	// for the cool parallax
	CVector d = CVector(0, 0);

	backL3.Draw(g);
    // left limit
    if (player.GetX() < leftScreenLimit){
		scrolloffsetX = 0;
    }
	// scroll left to right
    else if (player.GetX() >= leftScreenLimit && player.GetX() <= rightScreenLimit){
		scrolloffsetX = leftScreenLimit - player.GetX();
		d.m_x = player.GetX() - pP.m_x;
    }
	// right limit
    else if (player.GetX() > rightScreenLimit){
		scrolloffsetX = leftScreenLimit - rightScreenLimit;
    }
    // bottom limit
    if (player.GetY() < topScreenLimit){
		scrolloffsetY = 0;
    }
	// scroll up and down
    else if (player.GetY() >= topScreenLimit && player.GetY() <= bottomScreenLimit){
		scrolloffsetY = topScreenLimit - player.GetY();
		d.m_y = player.GetY() - pP.m_y;
    }
	// top limit
    else if (player.GetY() > bottomScreenLimit){
		scrolloffsetY = topScreenLimit - bottomScreenLimit;
    }

	// the scroll setter
    g->SetScrollPos(scrolloffsetX, scrolloffsetY);

	backL1.SetPos(bP1 + d * 0.8);
	backL2.SetPos(bP2 + d * 0.85);

	// will store its current spot for the next run
	pP = player.GetPos();
	bP1 = backL1.GetPos();
	bP2 = backL2.GetPos();
	backL1.Draw(g);
	backL2.Draw(g);

	for (CSprite* s : tiles){
		s->Draw(g);
	}
	for (CSprite* s : enemies){
		s->Draw(g);
	}

	//player.Draw(g);
	playerAni.Draw(g);

	// little house
	house.Draw(g);

	// don't scroll the overlay screen
	g->SetScrollPos(0, 0);

	for (CSprite* p : particles) p->Draw(g);

	// Game UI
	lives.Draw(g);
	*g << font(30) << color(CColor::White()) << top << left << "Score: " << score;

	if (IsPaused()) {
		pause.Draw(g);
	}
}

/////////////////////////////////////////////////////
// Game Life Cycle

// The new and improved UI system
// used for placing all interactive UI elements
void CMyGame::PlaceElement(int item, CGraphics* g, bool d) {
	// if a button is not static
	if (d) {
		CSprite* b = menuButtons.at(item);
		CVector* v = &extraItemData.find(b)->second.second;
		if (b->GetHealth() == 1) {
			// applies scaling when mouse is on the button
			b->SetSize(*v * 1.1);
		}
		else {
			b->SetSize(*v);
		}
		// draws the menu item and marks it as drawn
		b->Draw(g);
		extraItemData.find(b)->second.first = true;
	}
	else {
		CSprite* b = menuUIstatic.at(item);
		b->Draw(g);
		extraItemData.find(b)->second.first = true;
	}
}

// new and improved UI element creator
// allows for location and size offsets and supports 2 types of UI elements
// b for button and s for static
void CMyGame::CreateNewElement(char* fileName, CVector&offset, char type, float sizeOffset) {
	CSprite* item = new CSprite();
	CVector loc = CVector((GetWidth() / 2) * offset.m_x, (GetHeight() / 2) * offset.m_y);
	// determine if its creating a button or static
	if (type == 'b') {
		// buttons have 2 different images with one being bright and the other being dark
		item->LoadImage(fileName, "sta", 1, 2, 0, 1);
		item->LoadImage(fileName, "bSta", 1, 2, 0, 0);
		item->SetImage("sta");
		// sets it to the position with the offset
		item->SetPos(loc.m_x, loc.m_y);
		item->SetSize(item->GetSize() * sizeOffset);
		menuButtons.push_back(item);
	}
	// static element
	else if (type == 's') {
		// these objects are static on the screen
		item->LoadImage(fileName, "txt");
		item->SetImage("txt");
		item->SetPos(loc.m_x, loc.m_y);
		item->SetSize(item->GetSize() * sizeOffset);
		menuUIstatic.push_back(item);
	}
	// static animated object
	else {
		item->LoadImage(fileName, "max", 2, 2, 0, 1);
		item->LoadImage(fileName, "mid", 2, 2, 1, 1);
		item->LoadImage(fileName, "min", 2, 2, 0, 0);
		item->LoadImage(fileName, "off", 2, 2, 1, 0);
		item->SetImage("max");
		item->SetPos(loc.m_x, loc.m_y);
		item->SetSize(item->GetSize() * sizeOffset);
		menuUIstatic.push_back(item);
	}
	// adds it to the lovely unordered map
	extraItemData.insert({ item, std::make_pair(false, item->GetSize()) });
}
// takes rectangles instead of sprites
void CMyGame::CreateNewElement(CRectangle&r, CColor& c) {
	CSprite* item = new CSpriteRect(r, c, GetTime());
	menuUIstatic.push_back(item);
	extraItemData.insert({ item, std::make_pair(false, item->GetSize()) });
}

// one time initialisation
void CMyGame::OnInitialize()
{
	// where she/her makes the UI and player

	background.SetImageFromFile("MainMenu.png");
	background.SetPos(GetWidth() / 2, GetHeight() / 2);

	// 0 - title text
	CreateNewElement("TitleText.png", CVector(1.0f, 1.7f), 's');

	// 0 - start button
	CreateNewElement("Start.png", CVector(1.0f, 0.5f), 'b', 4);

	// 1 - exit button
	CreateNewElement("Exit.png", CVector(1.0f, 0.2f), 'b', 4);

	// 2 - options button
	CreateNewElement("Options.png", CVector(0.1f, 0.14f), 'b', 4);

	// 3 - back button
	CreateNewElement("BackButton.png", CVector(0.07f, 1.9f), 'b', 1);

	// 4 - balls
	CreateNewElement("VolumeSlider.png", CVector(1.5f, 1.685f), 'b', 1);

	// 1 - line
	CreateNewElement(CRectangle(150, 500, 450, 10), CColor::DarkGray());

	// 2 - volume system
	CreateNewElement("VolumeVisual.png", CVector(1.64f, 1.685f), 'a', 3);

	//std::cout << "stat: " << menuUIstatic.size() << "\n";
	//std::cout << "dyn: " << menuButtons.size() << "\n";

	cutScreenBG.SetImageFromFile("CutScene.png");
	cutScreenBG.SetPos(400, 300);

	pause.SetImageFromFile("PauseScreen.png");
	pause.SetPos(400, 300);

	house.SetImageFromFile("House.png");

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
	player.SetPos(600, 100);
	playerAni.SetPos(player.GetPos());
	player.SetHealth(1);

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

	house.SetPos(-50, 196);
	solidObstcles.push_back(&house);

	god = defBlock->Clone();
	god->SetPos(50, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(100, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(200, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(300, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(400, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(400, 75);

	god = defBlock->Clone();
	god->SetPos(500, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(500, 75);

	god = defBlock->Clone();
	god->SetPos(600, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(700, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(800, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(900, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);

	god = defBlock->Clone();
	god->SetPos(1000, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1000, 75);

	god = defBlock->Clone();
	god->SetPos(1100, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1100, 75);

	god = defBlock->Clone();
	god->SetPos(1200, 92);
	tiles.push_back(god);
	god->SetSize(god->GetSize().m_x, god->GetSize().m_y*1.15);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1200, 92);

	god = defBlock->Clone();
	god->SetPos(1300, 92);
	tiles.push_back(god);
	god->SetSize(god->GetSize().m_x, god->GetSize().m_y * 1.15);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1300, 92);

	god = defBlock->Clone();
	god->SetPos(1500, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1500, 50);

	god = defBlock->Clone();
	god->SetPos(1400, 92);
	tiles.push_back(god);
	god->SetSize(god->GetSize().m_x, god->GetSize().m_y * 1.15);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1400, 92);

	god = purpleCrystal->Clone();
	god->SetPos(1500, 150);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1500, 150);

	god = CreateBat();
	god->SetPos(1750, 300);
	enemies.push_back(god);

	god = defBlock->Clone();
	god->SetPos(1600, 92);
	tiles.push_back(god);
	god->SetSize(god->GetSize().m_x, god->GetSize().m_y * 1.15);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1600, 92);

	god = defBlock->Clone();
	god->SetPos(1700, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1700, 75);

	god = defBlock->Clone();
	god->SetPos(1800, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1800, 50);

	god = defBlock->Clone();
	god->SetPos(1900, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1900, 50);

	god = defBlock->Clone();
	god->SetPos(2000, 10);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2000, 10);

	god = defBlock->Clone();
	god->SetPos(2100, 50);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2100, 50);

	god = CreateWorm();
	god->SetPos(2000, 150);
	enemies.push_back(god);

	god = defBlock->Clone();
	god->SetPos(2200, 75);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2200, 75);

	god = greenCrystal->Clone();
	god->SetPos(2200, 175);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(2200, 175);

	god = defBlock->Clone();
	god->SetPos(2300, 92);
	tiles.push_back(god);
	god->SetSize(god->GetSize().m_x, god->GetSize().m_y * 1.15);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2300, 92);

	god = defBlock->Clone();
	god->SetPos(2400, 0);
	god->SetRotation(-180);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2400, 0);

	god = defBlock->Clone();
	god->SetPos(2400, 145);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2400, 145);

	// Second Floor
	// Auf gehts

	god = babyBlock->Clone();
	god->SetPos(2300, 295);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2300, 295);

	god = babyBlock->Clone();
	god->SetPos(2200, 325);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2200, 325);

	god = babyBlock->Clone();
	god->SetPos(2100, 365);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2100, 365);

	god = babyBlock->Clone();
	god->SetPos(2000, 405);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(2000, 405);

	god = defBlock->Clone();
	god->SetPos(1800, 395);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1800, 395);

	god = defBlock->Clone();
	god->SetPos(1700, 425);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1700, 425);

	god = greenCrystal->Clone();
	god->SetPos(1700, 525);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1700, 525);

	god = defBlock->Clone();
	god->SetPos(1600, 425);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1600, 425);

	god = defBlock->Clone();
	god->SetPos(1500, 450);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1500, 450);

	god = defBlock->Clone();
	god->SetPos(1400, 475);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1400, 475);

	god = defBlock->Clone();
	god->SetPos(1300, 475);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1300, 475);

	god = defBlock->Clone();
	god->SetPos(1200, 475);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1200, 475);

	god = purpleCrystal->Clone();
	god->SetPos(1200, 575);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1200, 575);

	god = defBlock->Clone();
	god->SetPos(1100, 500);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1100, 500);

	god = defBlock->Clone();
	god->SetPos(1000, 575);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1000, 575);

	god = defBlock->Clone();
	god->SetPos(800, 540);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(800, 540);

	god = defBlock->Clone();
	god->SetPos(900, 600);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(900, 600);

	god = greenCrystal->Clone();
	god->SetPos(800, 640);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(800, 640);

	god = defBlock->Clone();
	god->SetPos(700, 600);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(700, 600);

	god = defBlock->Clone();
	god->SetPos(500, 560);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(500, 560);

	god = defBlock->Clone();
	god->SetPos(600, 600);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(600, 600);

	god = CreateWorm();
	god->SetPos(500, 700);
	enemies.push_back(god);

	god = defBlock->Clone();
	god->SetPos(400, 630);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(400, 630);

	god = defBlock->Clone();
	god->SetPos(300, 655);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(300, 655);

	god = defBlock->Clone();
	god->SetPos(200, 675);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(200, 675);

	god = defBlock->Clone();
	god->SetPos(100, 675);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(100, 675);

	god = defBlock->Clone();
	god->SetPos(0, 700);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(0, 700);

	// Third Floor

	god = babyBlock->Clone();
	god->SetPos(100, 850);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(100, 850);

	god = babyBlock->Clone();
	god->SetPos(200, 890);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(200, 890);

	god = babyBlock->Clone();
	god->SetPos(300, 930);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(300, 930);

	god = babyBlock->Clone();
	god->SetPos(400, 970);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(400, 970);

	god = defBlock->Clone();
	god->SetPos(500, 960);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(500, 960);

	god = defBlock->Clone();
	god->SetPos(600, 995);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(600, 995);

	god = defBlock->Clone();
	god->SetPos(700, 980);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(700, 980);

	god = defBlock->Clone();
	god->SetPos(800, 980);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(800, 980);

	god = defBlock->Clone();
	god->SetPos(900, 1000);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(900, 1000);

	god = defBlock->Clone();
	god->SetPos(1100, 950);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1100, 950);

	god = greenCrystal->Clone();
	god->SetPos(1100, 1050);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1100, 1050);

	god = defBlock->Clone();
	god->SetPos(1000, 1000);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1000, 1000);

	god = defBlock->Clone();
	god->SetPos(1200, 1050);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1200, 1050);

	god = defBlock->Clone();
	god->SetPos(1300, 1020);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1300, 1020);

	god = purpleCrystal->Clone();
	god->SetPos(1300, 1120);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1300, 1120);

	god = greenCrystal->Clone();
	god->SetPos(1360, 1120);
	tiles.push_back(god);
	deadlyObstcles.push_back(god);
	tiles.back()->SetPos(1360, 1120);

	god = defBlock->Clone();
	god->SetPos(1400, 1020);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1400, 1020);

	god = defBlock->Clone();
	god->SetPos(1500, 1060);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1500, 1060);

	god = defBlock->Clone();
	god->SetPos(1600, 1060);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1600, 1060);

	god = defBlock->Clone();
	god->SetPos(1700, 1080);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1700, 1080);

	god = defBlock->Clone();
	god->SetPos(1800, 1080);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1800, 1080);

	god = defBlock->Clone();
	god->SetPos(1900, 1080);
	tiles.push_back(god);
	solidObstcles.push_back(god);
	tiles.back()->SetPos(1900, 1080);



//	god = defBlock->Clone();
//	god->SetPos(1750, 50);
//	god->SetSize(god->GetSize().m_x * 16, god->GetSize().m_y);
//	tiles.push_back(god);
//	solidObstcles.push_back(god);

	//god = CreateBat();
	//god->SetX(700);
	//enemies.push_back(god);

	std::cout << enemies.size() << "\n";
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
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

	backL1.SetBottomLeft(CVector(-10, -10));
	backL2.SetBottomLeft(CVector(-10, -10));

	resetGame = true;
	player.SetHealth(1);

	playerAni.SetAnimation("idle");
	player.SetPos(50, 200);
	playerAni.SetPos(player.GetPos());
	music.Play("MenuMusic.wav", 9999);
	music.Volume(vol);
	sfx.Stop();

	//StartGame();	// exits the menu mode and starts the game mode
}

// chose not to use clone for bats not due to it not working but it being experimental
CSprite* CMyGame::CreateBat() {
	CSprite* b = new CSpriteBat(CRectangle(400, 300, 20, 20), GetTime(), &player, &vol, &playerBounce, &attack, &attRight, &resetGame);
	b->LoadAnimation("BatIdle.png", "idle", CSprite::Sheet(4, 1).Row(0).From(0).To(4), CColor::Black());
	b->LoadAnimation("BatTakeOff.png", "offR", CSprite::Sheet(8, 1).Row(0).From(0).To(3), CColor::Black());
	b->LoadAnimation("BatTakeOff.png", "offL", CSprite::Sheet(8, 1).Row(0).From(4).To(7), CColor::Black());
	b->LoadAnimation("BatFly.png", "flyR", CSprite::Sheet(8, 1).Row(0).From(0).To(3), CColor::Black());
	b->LoadAnimation("BatFly.png", "flyL", CSprite::Sheet(8, 1).Row(0).From(4).To(7), CColor::Black());
	b->LoadAnimation("BatDie.png", "die", CSprite::Sheet(2, 1).Row(0).From(0).To(1), CColor::Black());
	b->SetAnimation("idle");
	return b;
}

CSprite* CMyGame::CreateWorm() {
	CSprite* b = new CSpriteWorm(CRectangle(0, 0, 45, 200), GetTime(), &player, &vol, &attack, &attRight, &resetGame, &solidObstcles);
	b->LoadAnimation("SandSleep.png", "idle", CSprite::Sheet(1, 1).Row(0).From(0).To(0), CColor::Black());
	b->LoadAnimation("SandWarn.png", "warn", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Black());
	b->LoadAnimation("SandAttack.png", "att", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
	b->LoadAnimation("SandWeak.png", "attW", CSprite::Sheet(2, 1).Row(0).From(0).To(1), CColor::Black());
	b->LoadAnimation("SandDip.png", "back", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
	b->LoadAnimation("SandMove.png", "move", CSprite::Sheet(3, 1).Row(0).From(0).To(2), CColor::Black());
	b->LoadAnimation("SandDie.png", "die", CSprite::Sheet(7, 1).Row(0).From(0).To(6), CColor::Black());
	b->SetAnimation("idle");
	return b;
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	music.Play("GameplayMusic.wav", 999);
	music.Volume(vol);
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
	sfx.Stop();
	music.Stop();
	NewGame();
}

// one time termination code
void CMyGame::OnTerminate()
{

}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT))) {
		StopGame();
	}
	if (sym == SDLK_F2) {
		if (IsPaused()) PauseGame();
		NewGame();
	}

	if (sym == SDLK_ESCAPE) {
		options = false;
		if (IsGameMode()) {
			PauseGame();
			if (IsPaused()) {
				music.Pause();
				sfx.Pause();
			}
			else {
				music.Resume();
				sfx.Resume();
			}
		} 
	}

	// this was so fucking painful, holy shit
	if (IsPaused() || IsGameOver()) return;
	if (sym == SDLK_s && playCutscene)StartGame();

	if (sym == SDLK_LEFT || sym == SDLK_a) {
		if (!wL) {
			playerAni.SetAnimation("walkL");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
			wL = true;
			wR = false;
		}
		attack = false;
	}
	if (sym == SDLK_RIGHT || sym == SDLK_d) {
		if (!wR) {
			playerAni.SetAnimation("walkR");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
			wR = true;
			wL = false;
		}
		attack = false;
	}
	if (sym == SDLK_LCTRL) {
		if (wL && !wR) {
			playerAni.SetAnimation("runL");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(1);
		}
		else if (wR && !wL) {
			playerAni.SetAnimation("runR");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(1);
		}
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (IsPaused() || IsGameOver()) return;
	if (sym == SDLK_LEFT || sym == SDLK_a) {
		if (wL) {
			playerAni.SetAnimation("idle");
			sfx.Stop();
			player.SetState(0);
			wL = false;
		}
	}
	if (sym == SDLK_RIGHT || sym == SDLK_d) {
		if (wR) {
			playerAni.SetAnimation("idle");
			sfx.Stop();
			player.SetState(0);
			wR = false;
		}
	}
	if (sym == SDLK_LCTRL) {
		if (wL && !wR) {
			playerAni.SetAnimation("walkL");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
		}
		else if (wR && !wL) {
			playerAni.SetAnimation("walkR");
			sfx.Play("walk.wav", 999);
			sfx.Volume(vol);
			player.SetState(0);
		}
	}
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	if (IsMenuMode()) {
		for (CSprite* b : menuButtons) {
			// will only work for elements that are currently drawn
			// plays specific UI sounds and sets the bright button
			// also ensures the sfx is not spammed, still a bit buggy though
			if (b->HitTest(x, y) && extraItemData[b].first) {
				b->SetImage("bSta");
				if (b->GetHealth() == 2) continue;
				b->SetHealth(1);
				if (b->GetState() == 0) {
					if (!sfx.IsPlaying()) {
						sfx.Play("UIhover.wav");
						sfx.Volume(vol);
					}
					b->SetState(1);
				}
			}
			// no mouse interaction
			else {
				b->SetImage("sta");
				if (b->GetHealth() == 2) continue;
				b->SetHealth(0);
				b->SetState(0);
			}
		}
		if (volMove == true) {
			menuButtons.at(4)->SetX(std::clamp(static_cast<int>(x), 150, 600));
			vol = static_cast<double>((std::clamp(static_cast<int>(x), 150, 600)) - 150) / 450.0f;
			CSprite* m = menuUIstatic.at(2);
			if (vol >= 0.66f) {
				m->SetImage("max");
			}
			else if (vol >= 0.33f) {
				m->SetImage("mid");
			}
			else if (vol >= 0.05) {
				m->SetImage("min");
			}
			else {
				m->SetImage("off");
			}
			m->SetSize(m->GetSize() * 3);
			UpdateSound();
		}
	}
}

// changes the games volume
void CMyGame::UpdateSound() {
	music.Volume(vol);
	sfx.Volume(vol);
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (IsPaused() || IsGameOver()) return;
	attack = true;
	if (!IsMenuMode()) {
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
			sfx.Play("Attack.wav");
			sfx.Volume(vol);
			playerAni.SetAnimation("attackL");
			attRight = false;
			player.SetState(0);
			player.SetXVelocity(0);
		}
		else {
			playerAni.SetAnimation("attackR");
			sfx.Play("Attack.wav");
			sfx.Volume(vol);
			attRight = true;
			player.SetState(0);
			player.SetXVelocity(0);
		}
	}

	// start
	if (menuButtons.at(0)->GetHealth() == 1) {
		playCutscene = true;
	}
	// exit
	if (menuButtons.at(1)->GetHealth() == 1) {
		sfx.Play("UIclick.wav");
		sfx.Volume(vol);
		StopGame();
	}
	// options
	if(menuButtons.at(2)->GetHealth() == 1){
		sfx.Play("UIclick.wav");
		sfx.Volume(vol);
		options = true;
	}
	// exit options
	if (menuButtons.at(3)->GetHealth() == 1) {
		sfx.Play("UIclick.wav");
		sfx.Volume(vol);
		options = false;
	}
	// volume slider
	if (menuButtons.at(4)->GetHealth() == 1) {
		volMove = true;
	}

}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	attack = false;
	volMove = false;
	if (IsPaused() || IsGameOver()) return;
	if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) {
		playerAni.SetAnimation("walkL");
		sfx.Play("walk.wav", 999);
		sfx.Volume(vol);
		player.SetState(0);
	}
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)){
		playerAni.SetAnimation("walkR");
		sfx.Play("walk.wav", 999);
		sfx.Volume(vol);
		player.SetState(0);
	}
	else {
		playerAni.SetAnimation("idle");
		sfx.Stop();
		player.SetState(0);
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
