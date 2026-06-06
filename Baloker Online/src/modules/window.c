#include <stdio.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"

#include "../baloker.h"
#include "window.h"

SDL_Time Elapsed;

float FPS = 0;
float Delta = 1000;

int Window_Init(Window* w)
{
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
}

void Window_Render(Window* w)
{
    SDL_SetRenderDrawColor(w->renderer, 0, 0, 0, 255);
    SDL_RenderClear(w->renderer);

    // Draw

    SDL_RenderPresent(w->renderer);
}

void Window_Destroy(Window* w)
{
    SDL_DestroyRenderer(w->renderer);
    SDL_DestroyWindow(w->window);
    SDL_Quit();
}
