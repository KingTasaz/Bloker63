#pragma once

#include "SDL3/SDL.h"

#include "../baloker.h"
#include "cards.h"

SDL_Texture *UItextureCache[cardTextureCacheSize];

void initUI(SDL_Renderer *renderer);
void drawCard(SDL_Renderer *renderer, Card card);
int mouseCollideCard(float x, float y, Card card);
