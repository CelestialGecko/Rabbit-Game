#pragma once
// for the bat sprites to give them more properties
enum stateAni { SLEEP, TAKEOFFL, TAKEOFFR, FLYL, FLYR, DIE, DEAD };
class CSpriteBat :
    public CSprite
{
private:
    stateAni s;
    CVector headDirection;
    char* ani;
    Uint8 aniChange;
    // pointer to the player
    CSprite* player;
public:
    CSpriteBat(CRectangle r, Uint32 time, CSprite*p);
    bool BetterHitTest(CSprite& s);
    stateAni GetStateAni() const { return s; }
    void KillBat() { s = DIE; }
    bool PlayerDetected(CSprite*p);
    void UpdateBat(CSprite* p);
    void SetBatAnimation(char* pPropName, int fps = 10, int nIndexStart = 0, int numFrames = -1);
protected:
    // this was a lot of trial and error, im not used to classes so like pain
    virtual void OnUpdate(Uint32 nGameTime, Uint32 deltaTime);
};

