#include <stdio.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "../baloker.h"
#include "window.h"
#include "UI.h"
#include "cards.h"
#include "game.h"
#include "poker.h"

#define rect9Size 16, 16, 15, 15, 1

SDL_Time Elapsed;

float FPS = 0;
float Delta = 0;

// Variables
SDL_Surface *surface = NULL;
SDL_Texture *texture_background = NULL;
SDL_Texture *texture_title = NULL;
SDL_Texture *texture_rect = NULL;
const char *img_background = "assets/Background.png";
const char *img_title = "assets/Title.png";

SDL_Texture *Text_Title = NULL;
SDL_Texture *Text_Version = NULL;

SDL_Texture *button_check = NULL;
SDL_Texture *button_call = NULL;
SDL_Texture *button_call_broke = NULL;
SDL_Texture *button_fold = NULL;
SDL_Texture *button_raise = NULL;

const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
const SDL_Color WHITE = { 255, 255, 255, SDL_ALPHA_OPAQUE };
const SDL_Color GRAY = { 67, 67, 67, SDL_ALPHA_OPAQUE };
const SDL_Color RED = { 255, 60, 60, SDL_ALPHA_OPAQUE};

char localPlayerMoneyText[30];

int Window_Init(Window* w)
{
    // Initialize Window
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        printf("SDL Failed to Initialize\n");
        return 1;
    }

    if (!TTF_Init()) {
        printf("Font Failed to Initialize");
        return 1;
    }

    w->window = SDL_CreateWindow(
        "Baloker Online :D",
        width, height,
        0
    );

    w->renderer = SDL_CreateRenderer(w->window, NULL);
    w->running = 1;

    // Load Images
    surface = SDL_LoadPNG(img_background);  // TODO: check for surface failures
    texture_background = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG(img_title);
    texture_title = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG("assets/9Rect_l16_r16_t15_b15.png");
    texture_rect = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    // Initialize UI
    initUI(w->renderer);

    // Load static text
    surface = TTF_RenderText_Blended(BalFontSmall, TITLE, 0, BLACK);
    Text_Title = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = TTF_RenderText_Blended(BalFontSmall, VERSION, 0, BLACK);
    Text_Version = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    // Load buttons
    surface = SDL_LoadPNG("assets/button_check.png");
    button_check = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_call.png");
    button_call = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_raise.png");
    button_raise = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_fold.png");
    button_fold = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_call_fail.png");
    button_call_broke = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    return 0;
}

void Window_HandleEvents(Window* w)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_EVENT_QUIT:
                w->running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_X:
                        gameState->action = CHECK;
                        break;
                    case SDLK_R:
                        gameState->action = RAISE;
                        break;
                    case SDLK_C:
                        gameState->action = CALL;
                        break;
                    case SDLK_F:
                        gameState->action = FOLD;
                        break;
                } break;
        }
    }
}

void Window_StartFrame()
{
    Elapsed = SDL_GetTicks();
}

void Window_EndFrame()
{
    Elapsed = SDL_GetTicks() - Elapsed;

    SDL_Time remaining = Elapsed - Target_MS;

    if (remaining > 0) {
        SDL_Delay(remaining);
    }

    Delta = (float)Elapsed;
    FPS = 1.0 / (float)Elapsed;

    if (Delta > 100) {
        Delta = 100;
    }
}

void Window_Debug()
{
    printf("FPS: %f\n", FPS);
}

void Window_Update(Window* w)
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    // Update Main Deck Cards
    for (int i = 0; i < mainDeck->cardCount; i++) {
        Card *card = &mainDeck->Cards[i];

        card->target_scale = card->default_scale;

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }

    // Update Player Hands Hand
    for (int p = 0; p < gameState->playerCount; p++)
    {
        PlayerHand *Hand = GetPlayer(p)->Hand;

        for (int i = 0; i < Hand->handCount; i++) {
            Card *card = &Hand->Hand[i];

            if (mouseCollideCard(mx, my, Hand->Hand[i])) {
                card->target_scale = card->default_scale * cardHighlightScale;
            } else {
                card->target_scale = card->default_scale;
            }

            card->x += (card->tx - card->x) * Delta / 100;
            card->y += (card->ty - card->y) * Delta / 100;
            card->scale += (card->target_scale - card->scale) * Delta / 100;
        }
    }

    // Update my river
    PlayerHand *myHand = GetLocalPlayer()->Hand;
    for (int i = 0; i < myHand->riverCount; i++) {
        Card *card = &myHand->River[i];

        if (mouseCollideCard(mx, my, myHand->River[i])) {
            card->target_scale = card->default_scale * cardHighlightScale;
        } else {
            card->target_scale = card->default_scale;
        }

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }

    // Update Chips
    bigBlindChip.x += (bigBlindChip.tx - bigBlindChip.x) * Delta / 100;
    bigBlindChip.y += (bigBlindChip.ty - bigBlindChip.y) * Delta / 100;

    smallBlindChip.x += (smallBlindChip.tx - smallBlindChip.x) * Delta / 100;
    smallBlindChip.y += (smallBlindChip.ty - smallBlindChip.y) * Delta / 100;

    turnOrderChip.x += (turnOrderChip.tx - turnOrderChip.x) * Delta / 100;
    turnOrderChip.y += (turnOrderChip.ty - turnOrderChip.y) * Delta / 100;
}

void _renderMainMenu(Window *w)
{
    SDL_FRect dst_rect;

    dst_rect.x = 0.0f;
    dst_rect.y = 0.0f;
    dst_rect.w = width;
    dst_rect.h = height;
}

void _renderSettings(Window *w);
void _renderJoinGame(Window *w);
void _renderGameLobby(Window *w);
void _renderInGame(Window *w)
{
    SDL_FRect dst_rect;

    // Player Info
    dst_rect.x = 220;
    dst_rect.y = 870;
    dst_rect.w = 220;
    dst_rect.h = 300;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    drawText(               // name
        w->renderer,
        BalFontSmall,
        GetPlayerName(0),
        BLACK,
        330, 900,
        1
    );

    snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "$%d", GetLocalPlayer()->Chips);
    drawText(               // money
        w->renderer,
        BalFontSmall,
        localPlayerMoneyText,
        BLACK,
        330, 960,
        1
    );

    // Other Player Info
    for (int p = 1; p < gameState->playerCount; p++) {
        dst_rect.x = PlayerCX[p] - 100;
        dst_rect.y = PlayerCY[p] - 120;
        dst_rect.w = 200;
        dst_rect.h = 100;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

        drawText(               // name
            w->renderer,
            BalFontSmall,
            GetPlayerName(p),
            BLACK,
            PlayerCX[p], PlayerCY[p] - 100,
            1
        );

        snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "$%d", GetPlayer(p)->Chips);
        drawText(               // money
            w->renderer,
            BalFontSmall,
            localPlayerMoneyText,
            BLACK,
            PlayerCX[p], PlayerCY[p] - 70,
            1
        );
    }

    // Corner Text
    dst_rect.w = Text_Title->w;
    dst_rect.h = Text_Title->h;
    dst_rect.x = 5;
    dst_rect.y = 5;
    SDL_RenderTexture(w->renderer, Text_Title, NULL, &dst_rect);

    dst_rect.w = Text_Version->w;
    dst_rect.h = Text_Version->h;
    dst_rect.x = 5;
    dst_rect.y = dst_rect.h + 5;
    SDL_RenderTexture(w->renderer, Text_Version, NULL, &dst_rect);

    // Draw cards in deck
    for (int i = 0; i < mainDeck->cardCount; i++) {
        drawCard(w->renderer, mainDeck->Cards[i]);
    }

    // Draw local hand
    PlayerHand *myHand = GetLocalPlayer()->Hand;
    for (int i = 0; i < myHand->handCount; i++) {
        drawCard(w->renderer, myHand->Hand[i]);
    }
    for (int i = 0; i < myHand->riverCount; i++) {
        drawCard(w->renderer, myHand->River[i]);
    }    
    if (GetLocalPlayer()->folded) {
        drawText(
            w->renderer, BalFontSmall,
            "Folded", RED,
            PlayerCX[0], PlayerCY[0] - 150,
            1
        );
    }

    // Draw other players
    for (int p = 1; p < gameState->playerCount; p++)
    {
        Player *plr = GetPlayer(p);

        for (int i = 0; i < plr->Hand->handCount; i++)
        {
            drawCard(w->renderer, plr->Hand->Hand[i]);
        }

        if (plr->folded) {
            drawText(
                w->renderer, BalFontSmall,
                "Folded", RED,
                PlayerCX[p], PlayerCY[p] - 20,
                1
            );
        }
    }

    // Round Information Screen
    dst_rect.x = width - 350;
    dst_rect.y = height - 500;
    dst_rect.w = 350 - 5;
    dst_rect.h = 550;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    int cx = width - 350 + (350 - 5) / 2;
    drawText(
        w->renderer, BalFontSmall,
        "Round Info", BLACK,
        cx, height - 450, 1
    );

    snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "Pot: $%d", getTotalPot());
    drawText(
        w->renderer, BalFontSmall,
        localPlayerMoneyText, BLACK,
        width - 350 + 10, height - 400, 0
    );
    snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "Raise: $%d", gameState->Raise);
    drawText(
        w->renderer, BalFontSmall,
        localPlayerMoneyText, BLACK,
        width - 350 + 10, height - 400 + 30, 0
    );
    snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "Cards in Deck: %d", mainDeck->cardCount);
    drawText(
        w->renderer, BalFontSmall,
        localPlayerMoneyText, BLACK,
        width - 350 + 10, height - 400 + 60, 0
    );

    // Hand Label
    dst_rect.x = width / 2 - 250;
    dst_rect.y = height - 380;
    dst_rect.w = 250 * 2;
    dst_rect.h = 75;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    int hand = GetLocalPlayer()->Hand->handType;

    if (hand >= 0 && hand < 16) {
        snprintf(localPlayerMoneyText, sizeof(localPlayerMoneyText), "Your Hand: %s", HandNames[hand]);
        drawText(w->renderer, BalFontSmall, localPlayerMoneyText, BLACK,
                    width / 2, height - 380 + 75 / 2, 1);
    } else {
        drawText(w->renderer, BalFontSmall, "Empty Hand", BLACK,
                    width / 2, height - 380 + 75 / 2, 1);
    }

    // Turn Buttons
    if (gameState->turn == 0) { // if (is my turn)
        float mx, my;
        SDL_GetMouseState(&mx, &my);

        dst_rect.x = width / 2 - 400 - 100;
        dst_rect.y = height - 380;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (gameState->Raise > gameState->myRaise) {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            if (gameState->Raise - gameState->myRaise > GetLocalPlayer()->Chips) {
                SDL_RenderTexture(w->renderer, button_call, NULL, &dst_rect);
                drawText(
                    w->renderer, BalFontSmall, 
                    "All In", BLACK,
                    dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
                );
            } else {
                SDL_RenderTexture(w->renderer, button_call, NULL, &dst_rect);
                drawText(
                    w->renderer, BalFontSmall, 
                    "Call", BLACK,
                    dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
                );
            }
        } else {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            SDL_RenderTexture(w->renderer, button_check, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Check", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );            
        }

        dst_rect.x = width / 2 + 400 - 100;
        dst_rect.y = height - 380;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (GetLocalPlayer()->Chips - gameState->Raise + gameState->myRaise > 0) {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            SDL_RenderTexture(w->renderer, button_raise, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Raise", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );
        } else {
            SDL_RenderTexture(w->renderer, button_call_broke, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Broke Ahh", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );
        }

        dst_rect.x = width / 2 + 400 - 100;
        dst_rect.y = height - 270;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
            drawGlow(w->renderer, dst_rect, 30, 20);
        }

        SDL_RenderTexture(w->renderer, button_fold, NULL, &dst_rect);
        drawText(
            w->renderer, BalFontSmall, 
            "Fold", BLACK,
            dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
        );
    }

    // Chips
    drawChip(w->renderer, bigBlindChip);
    drawChip(w->renderer, smallBlindChip);
    drawChip(w->renderer, turnOrderChip);
}

void Window_Render(Window* w)
{
    SDL_SetRenderDrawColor(w->renderer, 0, 0, 0, 255);
    SDL_RenderClear(w->renderer);

    // Draw Title Screen
    SDL_FRect dst_rect;

    dst_rect.x = 0.0f;
    dst_rect.y = 0.0f;
    dst_rect.w = width;
    dst_rect.h = height;
    SDL_RenderTexture(w->renderer, texture_background, NULL, &dst_rect);

    _renderInGame(w);    

    SDL_RenderPresent(w->renderer);
}

void Window_Destroy(Window* w)
{
    // SDL_ShowSimpleMessageBox(
    //     SDL_MESSAGEBOX_ERROR,
    //     "An Error Occured!",
    //     "Standard Error Message.",
    //     NULL
    // );

    SDL_DestroyRenderer(w->renderer);
    SDL_DestroyWindow(w->window);
    SDL_Quit();
}
