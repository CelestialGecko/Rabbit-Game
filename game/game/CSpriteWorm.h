#pragma once
enum wormAni { STATIC, WARN, ATTACK, ATTCKIDLE, DRILLDOWN, FOLLOW, DIEA, DEATH };
class CSpriteWorm :
    public CSprite
{
private:
    wormAni w;
    CSprite* player;
    char* ani;
    float* vol;
    CVector originalPos;
    bool* attack;
    bool* attRight;
    bool* gameReset;
    // this looks like funky
    std::vector<CSprite*>* solidObstcles;
    Uint16 aniChange = 0;
    CSoundPlayer wormSound;
    CRectangle hitBox;
public:
    CSpriteWorm(CRectangle r, Uint32 time, CSprite* p, float* vol, bool* att, bool* rL, bool* re, std::vector<CSprite*>* sO);
    ~CSpriteWorm();
    char BetterHitTest(CSprite& p);
    wormAni GetStateAni() const { return w; }
    void KillBat() { w = DIEA; }
    bool PlayerDetected(CSprite* p, bool l);
    void UpdateWorm(CSprite* p);
    void SetWormAnimation(char* pPropName, int fps = 10, int nIndexStart = 0, int numFrames = -1);
    void FollowPlayer(std::vector<CSprite*>* b, CSprite*p);
    CVector WormDisplacement(CSprite* p);
    bool WormAttack(CSprite* p);
    void ResetWorm();
protected:
    virtual void OnUpdate(Uint32 nGameTime, Uint32 deltaTime);
};

