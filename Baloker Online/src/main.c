#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
    #define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DBG_NEW

    #define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define calloc(c,s) _calloc_dbg(c,s, _NORMAL_BLOCK, __FILE__, __LINE__)
    #define realloc(p,s) _realloc_dbg(p,s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <stdio.h>

#include "SDL3/SDL_main.h"

#include "modules/window.h"
#include "modules/cards.h"
#include "modules/game.h"


int main(int argc, char **argv)
{
    #ifdef _DEBUG
        printf("Launching in DEBUG mode\n");
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    Window w = {0};

    if (Window_Init(&w) != 0) {
        return 1;
    }

    InitGame(8, w.renderer);

    StartGameLoop();

    while (w.running)
    {
        Window_StartFrame();

        Window_HandleEvents(&w);
        Window_Update(&w);
        Window_Render(&w);

        Window_EndFrame();
    }

    #ifdef _DEBUG
        if (_CrtDumpMemoryLeaks()) {
            printf("Leaks found!\n");
        } else {
            printf("No leaks found.\n");
        }
    #endif

    CloseGame();
    Window_Destroy(&w);

    return 0;
}

