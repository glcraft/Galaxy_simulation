#pragma once
#include <Star.h>
#include <SDL2/SDL.h>
class Draw
{
public:
    virtual void init()=0;
    virtual void update(Star::range alive_galaxy, SDL_Event* sdlevent)=0;
    virtual void render()=0;
};