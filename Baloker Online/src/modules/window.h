#pragma once

#include "SDL3/SDL.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;
} Window;

int Window_Init(Window* w);
void Window_HandleEvents(Window* w);
void Window_Update(Window* w);
void Window_Render(Window* w);
void Window_Destroy(Window* w);

void Window_StartFrame();
void Window_EndFrame();
void Window_Debug();
