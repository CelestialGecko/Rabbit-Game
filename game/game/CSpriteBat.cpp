#include "stdafx.h"
#include "CSpriteBat.h"

CSpriteBat::CSpriteBat(CRectangle r, Uint32 time, CSprite*p) : CSprite(r, time), s(SLEEP), headDirection(CVector(0, 1)), ani("NA"), aniChange(0), player(p) {}

// checks if player collided with the bat
bool CSpriteBat::BetterHitTest(CSprite& s) {
    return s.HitTest(this);
}

// checks if the bat can go to attack the player
bool CSpriteBat::PlayerDetected(CSprite*p) {
    CVector bP = this->GetPosition();
    CVector pP = p->GetPosition();
    return (pP - bP).Length() < 300;
}

// pain
void CSpriteBat::OnUpdate(Uint32 nGameTime, Uint32 deltaTime) {
    UpdateBat(player);
    // only just relised OnUpdate actually has logic unlike OnDraw -_-
    CSprite::OnUpdate(nGameTime, deltaTime);
}

// updates the bat stuff
void CSpriteBat::UpdateBat(CSprite* p) {
    if (s == SLEEP && PlayerDetected(p)) {
        CVector bP = this->GetPosition();
        CVector pP = p->GetPosition();
        CVector d = (pP - bP).Normalise();
        // left
        if (d.m_x < 0) {
            s = TAKEOFFL;
            SetBatAnimation("offL", 5);
        }
        // right
        else {
            s = TAKEOFFR;
            SetBatAnimation("offR", 5);
        }
    }
    if (s == TAKEOFFL) {
        aniChange++;
        if (aniChange == 5) {
            aniChange = 0;
            this->SetVelocity(-100, -100);
            SetBatAnimation("flyL", 5);
            s = FLYL;
        }
    }
    if (s == TAKEOFFR) {
        aniChange++;
        if (aniChange == 5) {
            aniChange = 0;
            this->SetVelocity(100, -100);
            SetBatAnimation("flyR", 5);
            s = FLYR;
        }
    }
    if ((s == FLYR || s == FLYL) && (this->GetYVelocity() == -100) && (p->GetY() >= this->GetY())) {
        this->SetYVelocity(0);
    }
    if (s == DIE) {
        if (ani != "die") {
            SetBatAnimation("die");
        }
        else {
            aniChange++;
            if (aniChange == 3) {
                s = DEAD;
            }
        }
    }
}

void CSpriteBat::SetBatAnimation(char* pPropName, int fps, int nIndexStart, int numFrames) {
	ani = pPropName;
	SetAnimation(pPropName, fps, nIndexStart, numFrames);
}