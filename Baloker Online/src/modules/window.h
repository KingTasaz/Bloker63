#pragma once

#include "SDL3/SDL.h"

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;
} Window;

typedef enum {
    MainMenu,
    HostSetup,
    JoinLobby,
    LobbyWaiting,
    InGame
} WindowState;

extern WindowState Menu;

extern char LoginStatus[50];
extern int loginDebounce;
extern int loadLobbyDebounce;
extern int joinLobbyDebounce;
extern char JoinCode[4];

int Window_Init(Window* w);
void Window_HandleEvents(Window* w);
void Window_Update(Window* w);
void Window_Render(Window* w);
void Window_Destroy(Window* w);

void Window_StartFrame();
void Window_EndFrame();
void Window_Debug();
