#pragma once
// for the bat sprites to give them more properties
class CSpriteBat :
    public CSprite
{
private:
    char state;
public:
    CSpriteBat(CRectangle r, Uint32 time);
protected:
    virtual void OnDraw(CGraphics* g);
};

