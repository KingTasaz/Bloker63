#include <stdlib.h>
#include <math.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"

#include "cards.h"
#include "UI.h"

int cachedCardCount = 0;
SDL_FRect cardRect;

SDL_Texture *Deck_Standard = NULL;



inline getCardImagePath(char **path, Card card)
{
    SDL_asprintf(path, "assets/Cards/%i_%i.png", card.rank, card.suit);
}

void initUI(SDL_Renderer *renderer)
{
    Deck *standardDeck = CreateStandardDeck();  // TODO: check for deck creation failure

    char *imgPath = NULL;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;

    int _loaded = 0;

    for (int i = 0; i < standardDeck->cardCount; i++)
    {
        Card card = standardDeck->Cards[i];

        getCardImagePath(&imgPath, card);
        surface = SDL_LoadPNG(imgPath);

        if (!surface) {
            printf("Failed to find %i_%i.png\n", standardDeck->Cards[i].rank, standardDeck->Cards[i].suit);
            continue;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!texture) {
            printf("Texture failed to load on");
            printCard(card);
        }

        UItextureCache[card.ID] = texture;
        //printf("Loaded [%d] = %p\n", card.ID, (void *)UItextureCache[card.ID]);

        _loaded++;
    }

    surface = SDL_LoadPNG("assets/Cards/DECK.png");
    Deck_Standard = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_free(imgPath);
    SDL_DestroySurface(surface);

    printf("Successfully Loaded %d Card Images\n", _loaded);
}

void drawCard(SDL_Renderer *renderer, Card card)
{
    cardRect.w = card.flipped ? cardWidth : cardWidth * card.scale;
    cardRect.h = card.flipped ? cardHeight : cardHeight * card.scale;

    cardRect.x = card.x - cardRect.w / 2;
    cardRect.y = card.y - cardRect.h / 2;

    SDL_FlipMode flip = {0};

    SDL_Texture *tex = card.flipped ? Deck_Standard : UItextureCache[card.ID];
    double angle = card.flipped ? 0 : 3.5 * sin((double)(SDL_GetTicks() + card.seed) / 1000);
    
    if (!SDL_RenderTextureRotated(
        renderer,
        tex,
        NULL, &cardRect,
        angle, NULL, flip
    )) {
        //printf("SDL error: %s\n", SDL_GetError());
    }
}

int mouseCollideCard(float x, float y, Card card)
{
    return collideRect(
        x, y,
        card.x - cardWidth * card.scale / 2, card.y - cardHeight * card.scale / 2,
        cardWidth * card.scale,
        cardHeight * card.scale
    );
}
