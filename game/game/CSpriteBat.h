#pragma once
// for the bat sprites to give them more properties
enum stateAni { SLEEP, TAKEOFFL, TakeOFFR, FLYL, FLYR, DIE, DEAD };
class CSpriteBat :
    public CSprite
{
private:
    stateAni s;
public:
    CSpriteBat(CRectangle r, Uint32 time);
    void LoadAnimations(char* idle, char* fly, char* off, char* die);
    void BetterHitTest(CSprite& s);
    stateAni GetStateAni() const { return s; }
    void KillBat() { s = DIE; }
    void PlayerDetected();
protected:
    virtual void OnDraw(CGraphics* g);
};

