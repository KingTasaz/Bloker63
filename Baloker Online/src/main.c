#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#include "SDL3/SDL_main.h"

#include "modules/window.h"
#include "modules/cards.h"
#include "modules/game.h"
#include "modules/network.h"


int main(int argc, char **argv)
{
    Window w = {0};

    if (Window_Init(&w) != 0) {
        return 1;
    }

    InitGame(w.renderer);

    HANDLE netThread = CreateThread(NULL, 0, NetworkThread, NULL, 0, NULL);

    while (w.running)
    {
        Window_StartFrame();

        Window_HandleEvents(&w);
        Window_Update(&w);
        Window_Render(&w);

        Window_EndFrame();
    }

    CloseGame();
    Window_Destroy(&w);
    DestroyClient();

    return 0;
}

