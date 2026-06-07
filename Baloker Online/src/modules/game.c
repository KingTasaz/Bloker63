#include <stdlib.h>

#include "SDL3/SDL.h"
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

int PlayerCX[] = {
    384,
    576,
    768,
    960,
    1152,
    1344,
    1536
};

int PlayerCY[] = {
    150,
    150,
    150,
    150,
    150,
    150,
    150
};

char *playerNames[maxPlayers];

// Game Variables
int playerCount;

enum RoundStage stage = WAITING;
enum PlayerAction action = NONE;

int smallBlind;
int bigBlind;
int turn;       // whose turn is it?
int lastCall;

int Pot;
int Raise;

float timer;
int gameLoopFreeze = 0;


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

    // Player Name Defaults For Testing
    playerNames[0] = "KingTasaz";
    playerNames[1] = "Rohan";
    playerNames[2] = "Jason";

    return 0;
}

Player *GetLocalPlayer()
{
    if (!Init) { return NULL; }
    return &Players[0];
}

Player *GetPlayer(int id)
{
    if (!Init) { return NULL; }
    if (id > playerCount) { return NULL; }
    return &Players[id];
}

void StartRound()
{
    if (!Init) { return; }

    if (playerCount < 2) {
        printf("Warning. Starting round with less than 2 players.\n");
    }

    // initialize variables
    stage = WAITING;

    smallBlind = 0;
    bigBlind = (smallBlind + 1) % playerCount;
    turn = (bigBlind + 1) % playerCount;
    lastCall = bigBlind;

    Pot = 0;
    Raise = 0;

    // Shuffle the deck
    shuffleDeck(mainDeck);

    CLOSE = 0;
    gameThread = SDL_CreateThread(GameLoop, "GameLoop", NULL);
}

int GetNumPlayers()
{
    if (!Init) { return 0; }

    return playerCount;
}

char *GetPlayerName(int id)
{
    if (!Init) { return NULL; }

    if (id >= playerCount)
    {
        return NULL;
    }

    return playerNames[id];
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

/*
Below are all the functions related to the ASYNCHRONOUS game loop.

All functions there should only be called from each other.
*/

void ReorganizeCardPositions()
{
    // Move deck cards back to deck
    for (int i = 0; i < mainDeck->cardCount; i++) {
        mainDeck->Cards[i].tx = DeckPosX;
        mainDeck->Cards[i].ty = DeckPosY;
        mainDeck->Cards[i].target_scale = 1.0;
    }

    // Move cards in hand
    Player *lp = GetLocalPlayer();
    float cx = width / 2;
    float cy = height * 0.85;

    int numCards = lp->Hand->handCount;
    float left = cx - (cardWidth / 2 + 25) * (numCards - 1);

    for (int i = 0; i < numCards; i++) {
        lp->Hand->Hand[i].tx = left + (cardWidth + 50) * i;
        lp->Hand->Hand[i].ty = cy;
    }

    // Move cards in River
    cy = height * 0.5;

    numCards = lp->Hand->riverCount;
    left = cx - (cardWidth / 2 + 25) * (numCards - 1);

    for (int i = 0; i < numCards; i++) {
        lp->Hand->River[i].tx = left + (cardWidth + 50) * i;
        lp->Hand->River[i].ty = cy;
    }

    // Move cards for other players
    for (int p = 1; p < playerCount; p++)
    {
        cx = (float)PlayerCX[p];
        cy = (float)PlayerCY[p];

        numCards = Players[p].Hand->handCount;
        left = cx - (cardWidth / 2 + 25) * (numCards - 1) * 0.1;

        for (int i = 0; i < numCards; i++) {
            Players[p].Hand->Hand[i].tx = left + (cardWidth + 50) * i * 0.1;
            Players[p].Hand->Hand[i].ty = cy + SDL_rand(6) - 3;
        }
    }
}


// Call this ONLY from GameLoop
// Delay: 100ms
void dealCardToAllPlayers()
{
    for (int i = 0; i < playerCount; i++) {
        int plr = (smallBlind + i) % playerCount;

        Card card = drawFromDeck(mainDeck);
        
        if (plr == 0) {
            card.flipped = 0;
        } else {
            card.default_scale = 0.25;
        }

        addToHand(Players[plr].Hand, card);

        ReorganizeCardPositions();
        SDL_Delay(100);
    }
}

// Delay: 100ms
void dealCardToRiver()
{
    Card card = drawFromDeck(mainDeck);
    card.flipped = 0;
    addToRiver(GetLocalPlayer()->Hand, card);

    ReorganizeCardPositions();
    SDL_Delay(100);
}

void startNextPlayerAction()
{
    if (turn == lastCall) {
        stage++;
        gameLoopFreeze = 50;
        action = NONE;
        return;
    }

    if (action == RAISE) {
        lastCall = turn - 1;
        if (lastCall == -1) { lastCall = playerCount -1; }
    }

    action = NONE;

    int loopDetect = turn;

    do {
        turn++;
        turn %= playerCount;

        if (turn == loopDetect) {
            stage = SHOWDOWN;
            return;
        }
    } while (Players[turn].folded);
}


void doBetRoundTick()
{
    switch (action) {
        case NONE:
            break;

        case CHECK:
            if (Raise == 0) {
                startNextPlayerAction();
            } break;

        case RAISE:
            Raise += 1;
            startNextPlayerAction();
            break;

        case CALL:
            startNextPlayerAction();
            break;

        case FOLD:
            Players[turn].folded = 1;
            startNextPlayerAction();
            break;
    }
}


int GameLoop(void *data)
{
    while (!CLOSE)
    {
        if (gameLoopFreeze > 0) {
            SDL_Delay(20);
            gameLoopFreeze--;
            continue;
        }

        timer = (float)SDL_GetTicks() / 1000;

        switch (stage) {
            case WAITING:
                stage = DEAL;
                break;

            case DEAL:     // Deal out cards
                dealCardToAllPlayers();
                dealCardToAllPlayers();
                stage = BUYIN;
                gameLoopFreeze = 50;
                break;

            case BUYIN:     // Buy-ins
                doBetRoundTick();
                break;

            case FLOP:     // Deal out flop
                dealCardToRiver();
                dealCardToRiver();
                dealCardToRiver();
                stage = BETFLOP;
                turn = (bigBlind + 1) % playerCount;
                gameLoopFreeze = 50;
                break;

            case BETFLOP:
                doBetRoundTick();
                break;

            case JOKER:
                stage = FINAL;  // jokers arent implemented yet
                break;

            case BETJOKER:
                break;

            case FINAL:
                dealCardToRiver();
                dealCardToRiver();
                stage = BETFINAL;
                gameLoopFreeze = 50;
                break;

            case BETFINAL:
                doBetRoundTick();
                break;

            case SHOWDOWN:
                stage = CLEANUP;    // showdown not implemented yet
                break;
        }

        SDL_Delay(20);
    }

    return 0;
}
