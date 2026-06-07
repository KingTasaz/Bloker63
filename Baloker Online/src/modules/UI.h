#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "../baloker.h"
#include "cards.h"

typedef struct {
    float x, y;
    float tx, ty;
    SDL_Texture *tex;
} Chip;

extern TTF_Font *BalFontSmall;

SDL_Texture *UItextureCache[cardTextureCacheSize];

void initUI(SDL_Renderer *renderer);
void drawCard(SDL_Renderer *renderer, Card card);
int mouseCollideCard(float x, float y, Card card);
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *Text, SDL_Color colour, int x, int y, int centered);
void drawChip(SDL_Renderer *renderer, Chip chip);
