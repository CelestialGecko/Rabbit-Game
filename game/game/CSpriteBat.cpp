#include "stdafx.h"
#include "CSpriteBat.h"

CSpriteBat::CSpriteBat(CRectangle r, Uint32 time, CSprite*p, float*v, bool* pB) 
    : CSprite(r, time), s(SLEEP), headDirection(CVector(0, 1)), ani("NA"), aniChange(0), player(p), vol(v), playerBounce(pB) {}

// what makes this better is it returns a char meaning I can get information on what sort of collision it is
// good for allowing the player to hit the bat on the head to kill it (:
// not using this functionality now but may need it later
char CSpriteBat::BetterHitTest(CSprite& p) {
    if (s == DEAD) return 'd';
    if (p.HitTest(this)) {
        //checks if the player has jumped over the bat
        if (Dot(CVector(0, 1), this->BatDisplacement(&p).Normalise()) < 0) {
            return 'k';
        }
        else {
            return 'p';
        }
    }
    return 'n';
}

// needed to do this a lot
CVector CSpriteBat::BatDisplacement(CSprite* p) {
    return (p->GetPos() - this->GetPos());
}

// checks if the bat can go to attack the player
bool CSpriteBat::PlayerDetected(CSprite*p) {
    return (this->BatDisplacement(p)).Length() < 300;
}

// runs when the sprite gets updated
void CSpriteBat::OnUpdate(Uint32 nGameTime, Uint32 deltaTime) {
    static float pos = 0;
    static bool hit = false;
    char t = this->BetterHitTest(*player);
    if (t == 'k' || hit == true) {
        if (s != DEAD && s != DIE) {
            *playerBounce = true;
            KillBat();
            player->SetY(pos);
            hit = true;
        }
    }
    else if (t == 'p') {
        *playerBounce = false;
        player->SetHealth(0);
    }
    UpdateBat(player);
    CSprite::OnUpdate(nGameTime, deltaTime);
    // offsets the player up when ontop of the bat
    pos = player->GetY();
}

// updates the bat stuff
void CSpriteBat::UpdateBat(CSprite* p) {
    // if the player is being careless and wakes up a bat or is just too close
    if (s == SLEEP && PlayerDetected(p)) {
        CVector d = (this->BatDisplacement(p)).Normalise();
        // left
        if (d.m_x < 0) {
            s = TAKEOFFL;
            SetBatAnimation("offL", 16);
        }
        // right
        else {
            s = TAKEOFFR;
            SetBatAnimation("offR", 16);
        }
    }
    // takes off in the direction of the player
    if (s == TAKEOFFL) {
        aniChange++;
        std::cout << aniChange << "\n";
        if (aniChange == 15) {
            aniChange = 0;
            this->SetVelocity(-100, -100);
            SetBatAnimation("flyL", 8);
            s = FLYL;
        }
    }
    if (s == TAKEOFFR) {
        aniChange++;
        if (aniChange == 30) {
            aniChange = 0;
            this->SetVelocity(100, -100);
            SetBatAnimation("flyR", 8);
            s = FLYR;
        }
    }
    // ensures the bat is somewhat level with the players y level
    if ((s == FLYR || s == FLYL) && (this->GetYVelocity() == -100) && (p->GetY() >= this->GetY())) {
        this->SetYVelocity(0);
    }
    // triggers while the bat dies
    if (s == DIE) {
        if (ani != "die") {
            SetBatAnimation("die", 2);
            this->SetXVelocity(0);
            this->SetYVelocity(-200);
            this->SetOmega(200);
        }
        else {
            aniChange++;
            if (aniChange == 15) {
                s = DEAD;
                SetBatAnimation("die", 1, 1, 1);
            }
        }
    }
}

// sets the animation in a way where I can store the current animation name as well as get it
void CSpriteBat::SetBatAnimation(char* pPropName, int fps, int nIndexStart, int numFrames) {
	ani = pPropName;
	SetAnimation(pPropName, fps, nIndexStart, numFrames);
}