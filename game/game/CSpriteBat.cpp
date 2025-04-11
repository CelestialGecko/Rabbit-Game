#include "stdafx.h"
#include "CSpriteBat.h"

CSpriteBat::CSpriteBat(CRectangle r, Uint32 time, CSprite*p, float*v, bool* pB, bool* att, bool* rL, bool* re)
    : CSprite(r, time), s(SLEEP), headDirection(CVector(0, 1)), ani("NA"), aniChange(0), player(p), vol(v), playerBounce(pB), attack(att), attRight(rL), gameReset(re) {
}

void CSpriteBat::ResetBat() {
    this->SetVelocity(0, 0);
    this->SetPos(originalPos);
    s = SLEEP;
    SetAnimation("idle");
    ani = "NA";
    aniChange = 0;
    this->SetOmega(0);
    this->SetRotation(0);
}

// what makes this better is it returns a char meaning I can get information on what sort of collision it is
// good for allowing the player to hit the bat on the head to kill it (:
char CSpriteBat::BetterHitTest(CSprite& p) {
    // if the bat is dead
    if (s == DEAD) return 'd';
    CVector dis = this->BatDisplacement(&p);
    // normal hittest for the players jump and death
    if (p.HitTest(this)) {
        //checks if the player has jumped over the bat
        if (Dot(CVector(0, 1), dis.Normalise()) > 0.3) {
            return 'k';
        }
        else {
            return 'p';
        }
    }
    // checks if the player is close enough for the spear to attack
    if (dis.Length() < 60) return 'a';
    return 'n';
}

// needed to do this a lot
CVector CSpriteBat::BatDisplacement(CSprite* p) {
    return (p->GetPos() - this->GetPos());
}

// checks if the bat can go to attack the player
bool CSpriteBat::PlayerDetected(CSprite* p) {
    float distance = this->BatDisplacement(p).Length();
    return (distance < 100) || ((p->GetState() == 1) && (distance < 300));
}


bool CSpriteBat::BatAttack(CSprite *p) {
    // the player is attacking
    if (*attack) {
        CVector d = (this->BatDisplacement(p)).Normalise();
        float dot = Dot(d, CVector(-1, 0));
        // player is facing the right
        if (*attRight) {
            // checks if the spear is facing the enemy
            if (dot < 0) {
                return true;
            }
        }
        // player is facing the left
        else {
            if (dot > 0) {
                return true;
            }
        }
    }
    else {
        return false;
    }
}

// runs when the sprite gets updated
void CSpriteBat::OnUpdate(Uint32 nGameTime, Uint32 deltaTime) {
    if(originalPos == CVector(0, 0))originalPos = this->GetPos();
    if (*gameReset) {
        ResetBat();
        return;
    }
    static float pos = 0;
    static bool hit = false;
    // determines the collision for the player
    char t = this->BetterHitTest(*player);
    // the player collides from the top (the bat dies)
    if (t == 'k') {
        if (s != DEAD && s != DIE) {
            *playerBounce = true;
            KillBat();
            batSounds.Play("hit.wav");
            batSounds.Volume(*vol);
            hit = true;
        }
        player->SetY(pos);
    }
    else if (t == 'p') {
        // player dies or stops colliding with the bat for a jump
        *playerBounce = false;
        if (!hit)player->SetHealth(0);
    }
    else if (t == 'a') {
        // player attacks the bat
        if (BatAttack(player) && (s != DEAD && s != DIE)) {
            KillBat();
            batSounds.Play("hit.wav");
            batSounds.Volume(*vol);
        }
    }
    else if (t == 'n') {
        hit = false;
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
            SetBatAnimation("offL", 8);
        }
        // right
        else {
            s = TAKEOFFR;
            SetBatAnimation("offR", 8);
        }
    }
    // takes off in the direction of the player
    if (s == TAKEOFFL) {
        aniChange++;
        //std::cout << aniChange << "\n";
        if (aniChange == 20) {
            aniChange = 0;
            this->SetVelocity(-100, -100);
            SetBatAnimation("flyL", 8);
            s = FLYL;
        }
    }
    if (s == TAKEOFFR) {
        aniChange++;
        if (aniChange == 25) {
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