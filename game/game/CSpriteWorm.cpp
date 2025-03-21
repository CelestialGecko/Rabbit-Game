#include "stdafx.h"
#include "CSpriteWorm.h"

CSpriteWorm::CSpriteWorm(CRectangle r, Uint32 time, CSprite* p, float* vol, bool* att, bool* rL, bool* re, std::vector<CSprite*>* sO)
	: hitBox(r), w(STATIC), player(p), ani("NA"), vol(vol), attack(att), attRight(rL), gameReset(re), solidObstcles(sO){
    this->SetHealth(5);
}

CSpriteWorm::~CSpriteWorm() {
    delete solidObstcles;
}

// needed to do this a lot
CVector CSpriteWorm::WormDisplacement(CSprite* p) {
	return (p->GetPos() - this->GetPos());
}

void CSpriteWorm::ResetWorm() {
    this->SetVelocity(0, 0);
    this->SetPos(originalPos);
    w = STATIC;
    SetAnimation("idle");
    ani = "NA";
    aniChange = 0;
}

// checks if the worm can go for the attack
bool CSpriteWorm::PlayerDetected(CSprite* p, bool l) {
    float distance = this->WormDisplacement(p).Length();
    if (l) return (distance < 400);
    return (distance < 50) || ((p->GetState() == 1) && (distance < 200));
    
}

// checks if the player can actually hit the worm
bool CSpriteWorm::WormAttack(CSprite* p) {
    if (w != ATTACK && w != ATTCKIDLE && w != DRILLDOWN)return false;
    // the player is attacking
    if (*attack) {
        CVector d = (this->WormDisplacement(p)).Normalise();
        float dot = Dot(d, CVector(-1, 0));
        // player is facing the right
        if (*attRight) {
            // checks if the spear is facing the enemy
            if (dot > 0) {
                return true;
            }
        }
        // player is facing the left
        else {
            if (dot < 0) {
                return true;
            }
        }
    }
    else {
        return false;
    }
    return false;
}

// what makes this better is it returns a char meaning I can get information on what sort of collision it is
// similar to the bat version
char CSpriteWorm::BetterHitTest(CSprite& p) {
    // if the worm is dead
    if (w == DEATH) return 'd';
    CVector dis = this->WormDisplacement(&p);
    // normal hittest for the players jump and death
    if (w == ATTACK || w == ATTCKIDLE || w == DRILLDOWN) {
        if (p.HitTest(hitBox)) {
            return 'p';
        }
        // checks if the player is close enough for the spear to attack
        if (dis.Length() < 120) return 'a';
    }
    return 'n';
}

void CSpriteWorm::OnUpdate(Uint32 nGameTime, Uint32 deltaTime) {
    if (originalPos == CVector(0, 0))originalPos = this->GetPos();
    hitBox.MoveTo(this->GetX() - hitBox.w / 2, this->GetBottom());
    if (*gameReset) {
        ResetWorm();
        return;
    }
    // cooldown inbetween player attacks
    static int coolDown = 30;
    // determines the collision for the player
    char t = this->BetterHitTest(*player);
    if (t == 'p') {
        player->SetHealth(0);
    }
    if (t == 'a') {
        if (WormAttack(player) && coolDown == 60) {
            coolDown = 0;
            this->SetHealth(this->GetHealth() - 1);
            wormSound.Play("hit.wav");
            wormSound.Volume(*vol);
            if (this->GetHealth() == 0) {
                w = DIEA;
                aniChange = 0;
            }
        }
    }
    UpdateWorm(player);
    CSprite::OnUpdate(nGameTime, deltaTime);
    if (coolDown != 60)++coolDown;
}

// updates the bat stuff
void CSpriteWorm::UpdateWorm(CSprite* p) {
    // if the player is being careless and wakes up a worm :skull:
    if (w == STATIC && PlayerDetected(p, false)) {
        this->SetWormAnimation("warn", 8);
        w = WARN;
    }
    // warns the player so they back away
    if (w == WARN) {
        aniChange++;
        if (aniChange == 60) {
            aniChange = 0;
            SetWormAnimation("att", 8);
            w = ATTACK;
        }
    }
    // shoots up and attacks the player
    if (w == ATTACK) {
        aniChange++;
        if (aniChange == 30) {
            aniChange = 0;
            SetWormAnimation("attW", 8);
            w = ATTCKIDLE;
        }
    }
    // gives time for the player to attack while its in a bad spot
    if (w == ATTCKIDLE) {
        aniChange++;
        if (aniChange == 120) {
            aniChange = 0;
            w = DRILLDOWN;
            SetWormAnimation("back", 12);
        }
    }
    // the worm hides away
    if (w == DRILLDOWN) {
        aniChange++;
        if (aniChange == 20) {
            aniChange = 0;
            w = FOLLOW;
            SetWormAnimation("move", 8);
        }
    }
    // the worm has gone back down but is still active
    if (w == FOLLOW) {
        if (aniChange == 120) {
            w = WARN;
            this->SetWormAnimation("warn", 8);
            aniChange = 0;
        }
        else if (PlayerDetected(player, true)) {
            aniChange++;
            FollowPlayer(solidObstcles, player);
        }
        else {
            SetWormAnimation("idle");
            w = STATIC;
        }
    }
    // when the worm is defeated
    if (w == DIEA) {
        if (aniChange == 0) {
            this->SetWormAnimation("die", 8);
        }
        else if (aniChange == 40) {
            w = DEATH;
            this->SetWormAnimation("die", 1, 6, 1);
        }
        aniChange++;
    }
}

// logic for following the players location
void CSpriteWorm::FollowPlayer(std::vector<CSprite*>* b, CSprite* p) {
    CVector dis = WormDisplacement(p);
    float moveDis = dis.m_x / 120;
    bool onWall = false;
    for (CSprite* wall : *b) {

        if (wall->HitTest(hitBox)) {
            onWall = true;
            float wallL = wall->GetLeft();
            float wallR = wall->GetRight();
            // checks if move location is out of the worms bounds
            if (hitBox.Right() + moveDis > wallR || hitBox.Left() + moveDis < wallL) {
                //std::cout << moveDis << "\n";
                return;
            }
            else {
                break;
            }
        }
    }
    // Move the worm only if it is on a wall
    if (onWall) {
        this->SetX(this->GetX() + moveDis);
    }
}

// sets the animation in a way where I can store the current animation name as well as get it
void CSpriteWorm::SetWormAnimation(char* pPropName, int fps, int nIndexStart, int numFrames) {
    ani = pPropName;
    SetAnimation(pPropName, fps, nIndexStart, numFrames);
}
