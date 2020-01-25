#pragma once
#include <Star.h>
#include <SDL2/SDL.h>
class Draw
{
public:
    virtual void init()=0;
    virtual void event(SDL_Event* sdlevent)=0;
    //template<int N>
    //virtual void update(Star<N>::range alive_galaxy)=0;
    virtual void render()=0;
};