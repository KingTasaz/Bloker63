#include <stdlib.h>
#include <math.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "cards.h"
#include "UI.h"
#include "noise.h"

int cachedCardCount = 0;
SDL_FRect cardRect;

SDL_Surface *Deck_StandardS = NULL;
SDL_Texture *Deck_Standard = NULL;

SDL_Texture *Card_Highlight = NULL;
SDL_Texture *Card_Highlight2 = NULL;
SDL_Texture *BurningTexture = NULL;

TTF_Font *BalFontSmall = NULL;
SDL_Surface *Text_tempSurface = NULL;
SDL_Texture *Text_temp = NULL;

uint8_t *burnNoise;

#define glowSize 25

#define glowYellow 255, 252, 88
#define glowLime 157, 255, 88
#define glowOrange 245, 198, 12
#define glowCol glowLime

#define MUL8(c,m) ((uint8_t)((c)*(m)/255))


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
        //SDL_DestroySurface(surface);

        if (!texture) {
            printf("Texture failed to load on");
            printCard(card);
        }

        UIsurfaceCache[card.ID] = surface;
        UItextureCache[card.ID] = texture;
        //printf("Loaded [%d] = %p\n", card.ID, (void *)UItextureCache[card.ID]);

        _loaded++;
    }

    // Load deck image
    surface = SDL_LoadPNG("assets/Cards/DECK.png");
    Deck_StandardS = surface;
    Deck_Standard = SDL_CreateTextureFromSurface(renderer, surface);

    // Load glow image
    surface = SDL_LoadPNG("assets/glow.png");
    Card_Highlight = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureColorMod(Card_Highlight, glowCol);
    Card_Highlight2 = SDL_CreateTextureFromSurface(renderer, surface);
    //SDL_SetTextureColorMod(Card_Highlight2, glowYellow);

    SDL_free(imgPath);
    SDL_DestroySurface(surface);

    printf("Successfully Loaded %d Card Images\n", _loaded);

    // Load Fonts
    BalFontSmall = TTF_OpenFont("assets/balatro.ttf", 24);

    if (!BalFontSmall) {
        printf("Failed to load BalFont\n");
    }

    burnNoise = malloc(cardWidth * cardHeight);
    BurningTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        cardWidth,
        cardHeight
    );
}

void updateBurnTexture(Card card, SDL_Surface *source)
{
    FillTexture(burnNoise, card.seed);

    void *pixels;
    int pitch;

    SDL_LockTexture(BurningTexture, NULL, &pixels, &pitch);

    uint8_t *src_pixels = source->pixels;
    uint8_t *dst_pixels = pixels;

    float threshold = card.burnProgress * 255.0f;

    for (int y = 0; y < cardHeight; y++)
    {
        uint32_t *dst_row = (uint32_t *)(dst_pixels + y * pitch);

        int src_y = (y * source->h) / cardHeight;
        uint32_t *src_row = (uint32_t *)(src_pixels + src_y * source->pitch);

        for (int x = 0; x < cardWidth; x++)
        {
            int src_x = (x * source->w) / cardWidth;

            int noise_idx = y * cardWidth + x;
            uint8_t n = burnNoise[noise_idx];

            if (n < threshold)
            {
                dst_row[x] = 0;
                continue;
            }

            float edge = fabsf((float)n - threshold);

            uint32_t src = src_row[src_x];

            uint8_t r = src >> 24;
            uint8_t g = src >> 16;
            uint8_t b = src >> 8;
            uint8_t a = src;

            uint8_t tr = r;
            uint8_t tg = g;
            uint8_t tb = b;

            if (edge < 6.0f) {        // bright hot edge
                tr = 255;
                tg = 255;
                tb = 200;
            }
            else if (edge < 25.0f) {   // orange
                tr = 255;
                tg = 120;
                tb = 0;
            }
            else if (edge < 50.0f) {  // dark brown
                tr = 120;
                tg = 40;
                tb = 10;
            }

            if (edge < 50.0f) {
                float t = 1.0f - edge / 50.0f;

                r = r + (tr - r) * t;
                g = g + (tg - g) * t * t;
                b = b + (tb - b) * t * t * t;
            }

            dst_row[x] = (r<<24) | (g<<16) | (b<<8) | a;
        }
    }

    SDL_UnlockTexture(BurningTexture);
}

void drawCard(SDL_Renderer *renderer, Card card)
{
    cardRect.w = cardWidth * card.scale;
    cardRect.h = cardHeight * card.scale;

    cardRect.x = card.x - cardRect.w / 2;
    cardRect.y = card.y - cardRect.h / 2;

    SDL_FlipMode flip = {0};

    SDL_Texture *tex = card.flipped ? Deck_Standard : UItextureCache[card.ID];
    double angle = card.flipped ? 0 : 3.5 * sin((double)(SDL_GetTicks() + card.seed) / 1000);

    if (card.burning) {
        SDL_Surface *surf = card.flipped ? Deck_StandardS : UIsurfaceCache[card.ID];
        updateBurnTexture(card, surf);
        tex = BurningTexture;
    }

    if (card.highlighted && !card.burning) {
        SDL_FRect glowRect = {0};
        glowRect.x = cardRect.x - glowSize;
        glowRect.y = cardRect.y - glowSize * 1.4;
        glowRect.w = cardRect.w + glowSize * 2;
        glowRect.h = cardRect.h + glowSize * 2 * 1.4;

        SDL_RenderTextureRotated(
            renderer,
            Card_Highlight,
            NULL, &glowRect,
            angle, NULL, flip
        );
    }
    
    SDL_RenderTextureRotated(
        renderer,
        tex,
        NULL, &cardRect,
        angle, NULL, flip
    );
}

void drawGlow(SDL_Renderer *renderer, SDL_FRect rect, int sizex, int sizey) {
    SDL_FRect glowRect = {0};
    glowRect.x = rect.x - sizex;
    glowRect.y = rect.y - sizey;
    glowRect.w = rect.w + sizex * 2;
    glowRect.h = rect.h + sizey * 2;

    SDL_RenderTexture(
        renderer,
        Card_Highlight2,
        NULL, &glowRect
    );
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *Text, SDL_Color colour, int x, int y, int centered)
{
    Text_tempSurface = TTF_RenderText_Blended(font, Text, 0, colour);
    Text_temp = SDL_CreateTextureFromSurface(renderer, Text_tempSurface);

    SDL_FRect pos = centered ? (SDL_FRect){x-Text_temp->w/2, y-Text_temp->h/2, Text_temp->w, Text_temp->h} 
                             : (SDL_FRect){x, y, Text_temp->w, Text_temp->h};
    SDL_RenderTexture(renderer, Text_temp, NULL, &pos);

    SDL_DestroySurface(Text_tempSurface);
    SDL_DestroyTexture(Text_temp);
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

void drawChip(SDL_Renderer *renderer, Chip chip)
{
    SDL_FRect pos = {
        chip.x, chip.y, chip.tex->w, chip.tex->h
    };

    SDL_RenderTexture(renderer, chip.tex, NULL, &pos);
}
