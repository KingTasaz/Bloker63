#include <stdio.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"

#include "../baloker.h"
#include "window.h"
#include "UI.h"
#include "cards.h"
#include "game.h"

SDL_Time Elapsed;

float FPS = 0;
float Delta = 1000;

// Testing Variables
SDL_Surface *surface = NULL;
SDL_Texture *texture_background = NULL;
SDL_Texture *texture_title = NULL;
char *img_background = "assets/Background.png";
char *img_title = "assets/Title.png";

int Window_Init(Window* w)
{
    // Initialize Window
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        perror("SDL Failed to Initialize");
        return 1;
    }

    w->window = SDL_CreateWindow(
        "Baloker Online :D",
        width, height,
        0
    );

    w->renderer = SDL_CreateRenderer(w->window, NULL);
    w->running = 1;

    // Initialize Variables
    surface = SDL_LoadPNG(img_background);  // TODO: check for surface failures
    texture_background = SDL_CreateTextureFromSurface(w->renderer, surface);
    surface = SDL_LoadPNG(img_title);
    texture_title = SDL_CreateTextureFromSurface(w->renderer, surface);

    SDL_DestroySurface(surface);    // done with it, already made texture

    // Initialize UI
    initUI(w->renderer);

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
                    case SDLK_R:
                        shuffleDeck(mainDeck);
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

        if (mouseCollideCard(mx, my, mainDeck->Cards[i])) {
            card->target_scale = cardHighlightScale;
        } else {
            card->target_scale = 1.0;
        }

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }

    PlayerHand *myHand = GetLocalPlayer()->Hand;

    // Update My Hand
    for (int i = 0; i < myHand->handCount; i++) {
        Card *card = &myHand->Hand[i];

        if (mouseCollideCard(mx, my, myHand->Hand[i])) {
            card->target_scale = cardHighlightScale;
        } else {
            card->target_scale = 1.0;
        }

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }
    for (int i = 0; i < myHand->riverCount; i++) {
        Card *card = &myHand->River[i];

        if (mouseCollideCard(mx, my, myHand->River[i])) {
            card->target_scale = cardHighlightScale;
        } else {
            card->target_scale = 1.0;
        }

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }
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
    SDL_RenderTexture(w->renderer, texture_title, NULL, &dst_rect);

    // Draw All Cards
    for (int i = 0; i < mainDeck->cardCount; i++) {
        drawCard(w->renderer, mainDeck->Cards[i]);
    }

    PlayerHand *myHand = GetLocalPlayer()->Hand;
    for (int i = 0; i < myHand->handCount; i++) {
        drawCard(w->renderer, myHand->Hand[i]);
    }
    for (int i = 0; i < myHand->riverCount; i++) {
        drawCard(w->renderer, myHand->River[i]);
    }    

    SDL_RenderPresent(w->renderer);
}

void Window_Destroy(Window* w)
{
    SDL_DestroyRenderer(w->renderer);
    SDL_DestroyWindow(w->window);
    SDL_Quit();
}
