#include <stdio.h>

#include "SDL3/SDL_main.h"

#include "modules/window.h"


int main(int argc, char **argv)
{
    Window w = {};

    if (Window_Init(&w) != 0) {
        return 1;
    }

    while (w.running)
    {
        Window_StartFrame();

        Window_HandleEvents(&w);
        Window_Update(&w);
        Window_Render(&w);

        Window_EndFrame();
        Window_Debug();
    }

    Window_Destroy(&w);

    return 0;
}

