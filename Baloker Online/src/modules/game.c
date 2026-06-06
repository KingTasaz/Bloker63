#include <stdlib.h>

#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_thread.h"

#include "game.h"
#include "cards.h"
#include "../baloker.h"

// Local Variables
int Init = 0;
Player *Players = NULL;
Deck *mainDeck = NULL;
int CLOSE = 0;
SDL_Thread *gameThread = NULL;
int GameLoop(void *data);

// Game Variables
int playerCount;
int stage = 0;      // what stage of the round is it?
int smallBlind;
int bigBlind;
int turn;       // whose turn is it?
float timer;


int InitGame(int numPlayers)
{
    if (Init) { return 1; }

    // Create deck
    mainDeck = CreateStandardDeck();

    // Create Players
    if (numPlayers > maxPlayers) {
        printf("Unable to create game with %d Players\n", numPlayers);
        return 1;
    }

    Players = malloc(sizeof(Player) * numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        Players[i].ID = i;
        Players[i].Hand = CreateHand();
    }

    Init = 1;
    playerCount = numPlayers;
    return 0;
}

Player *GetLocalPlayer()
{
    if (!Init) { return NULL; }
    return &Players[0];
}

void StartRound()
{
    if (!Init) { return; }

    if (playerCount < 2) {
        printf("Warning. Starting round with less than 2 players.\n");
    }

    // initialize variables
    stage = 0;
    smallBlind = 0;
    bigBlind = (smallBlind + 1) % playerCount;
    turn = (bigBlind + 1) % playerCount;

    // Shuffle the deck
    shuffleDeck(mainDeck);

    gameThread = SDL_CreateThread(GameLoop, "GameLoop", NULL);
}

// Call this ONLY from GameLoop
// Delay: 100ms
void dealCardToAllPlayers()
{
    for (int i = 0; i < playerCount; i++) {
        int plr = (smallBlind + i) % playerCount;

        Card card = drawFromDeck(mainDeck);
        card.flipped = 0;
        addToHand(Players[plr].Hand, card);

        SDL_Delay(100);
    }
}

void ReorganizeCardPositions()
{
    // Move deck cards back to deck
    for (int i = 0; i < mainDeck->cardCount; i++) {
        mainDeck->Cards[i].tx = width * 0.1;
        mainDeck->Cards[i].ty = height * 0.5;
    }

    // Show cards in hand
    Player *lp = GetLocalPlayer();
    float cx = width / 2;
    float cy = height * 6 / 7;

    int numCards = lp->Hand->handCount;
    float left = cx - (cardWidth / 2 + 25) * (numCards - 1);

    for (int i = 0; i < numCards; i++) {
        lp->Hand->Hand[i].tx = left + (cardWidth + 50) * i;
        lp->Hand->Hand[i].ty = cy - cardHeight / 2;
    }
}


int GameLoop(void *data)
{
    while (!CLOSE)
    {
        timer = (float)SDL_GetTicks() / 1000;

        switch (stage) {
            case 0:
                dealCardToAllPlayers();
                dealCardToAllPlayers();
                dealCardToAllPlayers();
                ReorganizeCardPositions();
                stage = 1;
                break;
        }

        //SDL_Delay(1000);
    }

    return 0;
}

void CloseGame()
{
    if (!Init) { return; }

    if (gameThread) {
        CLOSE = 1;
        SDL_WaitThread(gameThread, NULL);
        gameThread = NULL;
    }

    for (int i = 0; i < playerCount; i++) {
        Hand_Destroy(Players[i].Hand);
    }
    free(Players);

    Deck_Destroy(mainDeck);
}
