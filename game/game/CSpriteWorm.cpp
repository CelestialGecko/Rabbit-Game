#include "stdafx.h"
#include "CSpriteWorm.h"

CSpriteWorm::CSpriteWorm(CRectangle r, Uint32 time, CSprite* p, float* vol, bool* att, bool* rL, bool* re, std::vector<CSprite*>* sO)
	: hitBox(r), w(STATIC), player(p), ani("NA"), vol(vol), attack(att), attRight(rL), gameReset(re), solidObstcles(sO){
    this->SetHealth(5);
}

CSpriteWorm::~CSpriteWorm() {
    delete solidObstcles;
}

void CSpriteWorm::FindTerritory(std::vector<CSprite*>* b, CSprite* p) {
	// searches its territory
	for (CSprite* w : *b) {
		if (w->HitTest(hitBox)) {
			territory = w;
			return;
		}
	}
	territory = nullptr;
}

// needed to do this a lot
CVector CSpriteWorm::WormDisplacement(CSprite* p) {
	return (p->GetPos() - this->GetPos());
}

// sets worm to original state
void CSpriteWorm::ResetWorm() {
    this->SetVelocity(0, 0);
    this->SetHealth(5);
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
    return (distance < 100) || ((p->GetState() == 1) && (distance < 200));
    
}

// checks if the player can actually hit the worm
bool CSpriteWorm::WormAttack(CSprite* p) {
    if (w != ATTACK && w != ATTCKIDLE && w != DRILLDOWN)return false;
    // the player is attacking
    if (*attack) {
        CVector d = this->WormDisplacement(p);
        if (d.Length() > 60)return false;
		//std::cout << d.Length() << "\n";
        float dot = Dot(d.Normalise(), CVector(-1, 0));
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
        // player is close to worm
        if (dis.Length() < 120) return 'a';
    }
    return 'n';
}

// will run on every update called from MyGame
void CSpriteWorm::OnUpdate(Uint32 nGameTime, Uint32 deltaTime) {
    // cooldown inbetween the player getting hurt
	if (cool > 0)cool++;
	// sets the default pos
    if (originalPos == CVector(0, 0))originalPos = this->GetPos();
    // positions hitbox
    hitBox.MoveTo(this->GetX() - hitBox.w / 2, this->GetBottom());
    // the game has been reset
    if (*gameReset) {
        ResetWorm();
        return;
    }
    // determines the collision for the player
    char t = this->BetterHitTest(*player);
    // the player died
    if (t == 'p') {
        if (cool == 0) {
            player->SetHealth(player->GetHealth() - 1);
            cool++;
        }
    }
    // worms is getting attacked
    if (t == 'a') {
		// checks if player can actually attack worm
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
    // changes worms visual state and position
    UpdateWorm(player);
    CSprite::OnUpdate(nGameTime, deltaTime);
    // attack colldown
	if (cool == 60)cool = 0;
    if (coolDown != 60)++coolDown;
}

// updates the bat stuff
void CSpriteWorm::UpdateWorm(CSprite* p) {
    // reset worm if bugged
    if (aniChange > 150)ResetWorm();
    // if the player is being careless and wakes up a worm
    if (w == STATIC && PlayerDetected(p, false)) {
        this->SetWormAnimation("warn", 8);
        w = WARN;
        // finds territory first
        if (temp) {
            FindTerritory(solidObstcles, player);
            temp = false;
        }
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
        if (aniChange == 22) {
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
        if (aniChange == 14) {
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
        else if (aniChange == 30) {
            w = DEATH;
            this->SetWormAnimation("die", 1, 6, 1);
            aniChange--;
        }
        aniChange++;
    }
}

// logic for following the players location
// also checks for neighboring walls so it doesnt clip inside
void CSpriteWorm::FollowPlayer(std::vector<CSprite*>* b, CSprite* p) {
	if (territory == nullptr) return;
    // calculates the move distance
    CVector dis = WormDisplacement(p);
    float moveDis = dis.m_x / 120;
    bool onWall = true;
    // loops through all walls that could be next to the worm
	for (CSprite* w : *solidObstcles) {
		if (w == territory) continue;
        // if the worm is touching a wall
        if (w->HitTest(hitBox)) {
            // checks if the wall is infront of where the worm wants to move
            if (dis.m_x > 0) {
                // if wall is not in the way then it will be able to move
                if (w->HitTest(hitBox.Right(), hitBox.Bottom())) {
					onWall = false;
                    break;
                }
            }
            else if(dis.m_x < 0){
                if (w->HitTest(hitBox.Left(), hitBox.Bottom())) {
                    //std::cout << "hit: " << w->GetX() << " " << territory->GetX() << "\n";
					onWall = false;
                    break;
                }
            }
        }
	}
	std::cout << onWall << "\n";
    // Move the worm only if it is on a wall
    if (onWall) {
        this->SetX(this->GetX() + moveDis);
    }

    // this was the old system, just keeping it for reference

    //for (CSprite* wall : *b) {
    //    if (wall->HitTest(hitBox)) {
    //        onWall = true;
    //        float wallL = wall->GetLeft();
    //        float wallR = wall->GetRight();
    //        // checks if move location is out of the worms bounds
    //        if (hitBox.Right() + moveDis > wallR || hitBox.Left() + moveDis < wallL) {
    //            //std::cout << moveDis << "\n";
    //            return;
    //        }
    //        else {
    //            break;
    //        }
    //    }
    //}
    //// Move the worm only if it is on a wall
    //if (onWall) {
    //    this->SetX(this->GetX() + moveDis);
    //}
}

// sets the animation in a way where I can store the current animation name as well as get it
void CSpriteWorm::SetWormAnimation(char* pPropName, int fps, int nIndexStart, int numFrames) {
    ani = pPropName;
    SetAnimation(pPropName, fps, nIndexStart, numFrames);
}
