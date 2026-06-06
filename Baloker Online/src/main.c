#include <stdio.h>

#include "SDL3/SDL_main.h"

#include "modules/window.h"
#include "modules/cards.h"


int main(int argc, char **argv)
{
    Window w = {};

    if (Window_Init(&w) != 0) {
        return 1;
    }

    Deck *test = CreateStandardDeck();
    shuffleDeck(test);
    printf("cardCount = %d\n", test->cardCount);
    printCard(test->Cards[0]);
    printCard(test->Cards[1]);
    printCard(test->Cards[2]);

    while (w.running)
    {
        Window_StartFrame();

        Window_HandleEvents(&w);
        Window_Update(&w);
        Window_Render(&w);

        Window_EndFrame();
    }

    Window_Destroy(&w);

    return 0;
}

