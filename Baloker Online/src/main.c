#include <stdio.h>

#include "SDL3/SDL_main.h"

#include "modules/window.h"
#include "modules/cards.h"
#include "modules/game.h"


int main(int argc, char **argv)
{
    Window w = {};

    InitGame(3);

    if (Window_Init(&w) != 0) {
        return 1;
    }

    StartRound();

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

    return 0;
}

