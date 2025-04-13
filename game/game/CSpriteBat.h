#pragma once
// for the bat sprites to give them more properties
enum batAni { SLEEP, TAKEOFFL, TAKEOFFR, FLYL, FLYR, DIE, DEAD };
class CSpriteBat :
    public CSprite
{
private:
    batAni s;
    CVector headDirection;
    CVector originalPos;
    // simple cooldown
    int cool = 0;
    char* ani;
    bool* attack;
    bool* attRight;
    bool* gameReset;
    Uint16 aniChange;
    // pointer to the player, needed so it can kill and follow the player
    CSprite* player;
    // keeps the volume consistent
    float* vol;
    bool* playerBounce;
    // squeak squeak
    CSoundPlayer batSounds;
public:
    CSpriteBat(CRectangle r, Uint32 time, CSprite*p, float* vol, bool* pB, bool* att, bool* rL, bool* re);
    char BetterHitTest(CSprite& p);
    batAni GetStateAni() const { return s; }
    void KillBat() { s = DIE; }
    bool PlayerDetected(CSprite*p);
    void UpdateBat(CSprite* p);
    void SetBatAnimation(char* pPropName, int fps = 10, int nIndexStart = 0, int numFrames = -1);
    CVector BatDisplacement(CSprite*p);
    bool BatAttack(CSprite*p);
    void ResetBat();
protected:
    virtual void OnUpdate(Uint32 nGameTime, Uint32 deltaTime);
};

