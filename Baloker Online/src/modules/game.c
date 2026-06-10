#include <stdlib.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_thread.h"

#include "game.h"
#include "cards.h"
#include "UI.h"
#include "poker.h"
#include "../baloker.h"

// Local Variables
int Init = 0;
Player *Players = NULL;
Deck *mainDeck = NULL;
int CLOSE = 0;
SDL_Thread *gameThread = NULL;
int GameLoop(void *data);

int PlayerCX[] = {
    330,
    190,
    450,
    710,
    970,
    190 + 130,
    450 + 130,
    710 + 130,
    970 + 130
};

int PlayerCY[] = {
    1000,
    200,
    200,
    200,
    200,
    330,
    330,
    330,
    330,
};

int ChipX[] = {
    240 - 50,
    190 - 110,
    450 - 110,
    710 - 110,
    970 - 110,
    190 - 110 + 130,
    450 - 110 + 130,
    710 - 110 + 130,
    970 - 110 + 130
};

int ChipY[] = {
    840,
    200 - 60,
    200 - 60,
    200 - 60,
    200 - 60,
    330 - 60,
    330 - 60,
    330 - 60,
    330 - 60,
};

int TurnChipX[] = {
    330 - 50,
    190 - 50,
    450 - 50,
    710 - 50,
    970 - 50,
    190 - 50 + 130,
    450 - 50 + 130,
    710 - 50 + 130,
    970 - 50 + 130
};

int TurnChipY[] = {
    820,
    30,
    30,
    30,
    30,
    160,
    160,
    160,
    160
};

GameState_t *gameState;

Chip bigBlindChip = {0};
Chip smallBlindChip = {0};
Chip turnOrderChip = {0};
SDL_Surface *tempSurface;

char *playerNames[maxPlayers];

// Game Variables
float timer;
int gameLoopFreeze = 0;


int InitGame(int numPlayers, SDL_Renderer *renderer)
{
    if (Init) { return 1; }

    // Game State
    gameState = malloc(sizeof(GameState_t));
    gameState->playerCount = numPlayers;

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
        Players[i].folded = 0;
        Players[i].Chips = 100;
    }

    // Player Name Defaults For Testing
    playerNames[0] = "LocalPlayer";
    playerNames[1] = "Rohan";
    playerNames[2] = "Jason";
    playerNames[3] = "Ali";
    playerNames[4] = "Austin";
    playerNames[5] = "Oliver";
    playerNames[6] = "Julia";
    playerNames[7] = "Seb";

    // Player Chips
    tempSurface = SDL_LoadPNG("assets/Big_Blind.png");
    bigBlindChip.tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_DestroySurface(tempSurface);

    tempSurface = SDL_LoadPNG("assets/Small_Blind.png");
    smallBlindChip.tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_DestroySurface(tempSurface);

    tempSurface = SDL_LoadPNG("assets/Arrow.png");
    turnOrderChip.tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_DestroySurface(tempSurface);

    Init = 1;
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
    if (id > gameState->playerCount) { return NULL; }
    return &Players[id];
}

void StartGameLoop()
{
    if (!Init) { return; }

    if (gameState->playerCount < 2) {
        printf("Warning. Starting game with less than 2 players.\n");
    }

    // initialize variables
    gameState->stage = WAITING;

    gameState->smallBlind = 0;
    gameState->bigBlind = (gameState->smallBlind + 1) % gameState->playerCount;
    gameState->turn = (gameState->bigBlind + 1) % gameState->playerCount;
    gameState->lastCall = gameState->bigBlind;

    bigBlindChip.tx = ChipX[gameState->bigBlind];
    bigBlindChip.ty = ChipY[gameState->bigBlind];
    smallBlindChip.tx = ChipX[gameState->smallBlind];
    smallBlindChip.ty = ChipY[gameState->smallBlind];
    turnOrderChip.tx = TurnChipX[gameState->turn];
    turnOrderChip.ty = TurnChipY[gameState->turn];

    for (int i = 0; i < maxPlayers; i++) { gameState->Pot[i] = 0; }
    gameState->Raise = 0;
    gameState->myRaise = 0;

    // Shuffle the deck
    shuffleDeck(mainDeck);

    CLOSE = 0;
    gameThread = SDL_CreateThread(GameLoop, "GameLoop", NULL);
}

char *GetPlayerName(int id)
{
    if (!Init) { return NULL; }

    if (id >= gameState->playerCount)
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

    for (int i = 0; i < gameState->playerCount; i++) {
        Hand_Destroy(Players[i].Hand);
    }

    free(Players);
    free(gameState);

    Deck_Destroy(mainDeck);
}

int getTotalPot() {
    int sum = 0;
    for (int i = 0; i < maxPlayers; i++) { sum += gameState->Pot[i]; }
    return sum;
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
    for (int p = 1; p < gameState->playerCount; p++)
    {
        cx = (float)PlayerCX[p];
        cy = (float)PlayerCY[p] - 20;

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
    for (int i = 0; i < gameState->playerCount; i++) {
        if (mainDeck->cardCount == 0) { continue; }

        int plr = (gameState->smallBlind + i) % gameState->playerCount;

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

    GetBestPokerHand(GetLocalPlayer()->Hand, gameState->myBestHand);
}

// Delay: 100ms
void dealCardToRiver()
{
    if (mainDeck->cardCount == 0) { return; }

    Card card = drawFromDeck(mainDeck);
    card.flipped = 0;
    addToRiver(GetLocalPlayer()->Hand, card);

    ReorganizeCardPositions();
    SDL_Delay(100);

    GetBestPokerHand(GetLocalPlayer()->Hand, gameState->myBestHand);
}

void startNextPlayerAction()
{
    if (gameState->action == RAISE) {
        gameState->lastCall = gameState->turn - 1;
        if (gameState->lastCall == -1) { gameState->lastCall = gameState->playerCount - 1; }
    }
    
    if (gameState->turn == gameState->lastCall) {
        gameState->stage++;
        gameLoopFreeze = 50;
        gameState->action = NONE;
        turnOrderChip.tx = -200;
        turnOrderChip.ty = height / 2;
        return;
    }

    gameState->action = NONE;
    int loopDetect = gameState->turn;

    do {
        gameState->turn++;
        gameState->turn %= gameState->playerCount;
    } while (Players[gameState->turn].folded);

    int AllFolded = 1;
    for (int i = 1; i < gameState->playerCount; i++) {
        int p = (gameState->turn + i) % gameState->playerCount;
        if (!Players[p].folded) {
            AllFolded = 0;
            break;
        }
    }

    if (AllFolded) {
        gameState->stage = SHOWDOWN;
        turnOrderChip.tx = -200;
        turnOrderChip.ty = height / 2;
        printf("Everyone but 1 folded.\n");
        return;
    }

    turnOrderChip.tx = TurnChipX[gameState->turn];
    turnOrderChip.ty = TurnChipY[gameState->turn];

    // printf("Current Player: %s\n", playerNames[turn]);
}

void startBetRound()
{
    gameState->action = NONE;
    gameState->Raise = 0;
    gameState->myRaise = 0;
    
    gameState->turn = (gameState->bigBlind + 1) % gameState->playerCount;
    gameState->lastCall = gameState->bigBlind;

    turnOrderChip.tx = TurnChipX[gameState->turn];
    turnOrderChip.ty = TurnChipY[gameState->turn];

    // printf("--- BETTING ROUND STARTED ---\n");
    // printf("Current Player: %s\n", playerNames[turn]);
}

void doBetRoundTick()
{
    switch (gameState->action) {
        case NONE:
            break;

        case CHECK:
            if (gameState->Raise == 0) {
                // printf(" -> Check.\n");
                startNextPlayerAction();
            } else {
                gameState->action = NONE;
            } break;

        case RAISE:
            gameState->Raise += 1;
            gameState->Pot[0] += 1;
            // printf(" -> Raise to %d\n", Raise);
            startNextPlayerAction();
            break;

        case CALL:
            // printf(" -> Call for %d\n", Raise);
            startNextPlayerAction();
            break;

        case FOLD:
            Players[gameState->turn].folded = 1;
            // printf(" -> Fold.\n");
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

        switch (gameState->stage) {
            case WAITING:
                gameState->stage = DEAL;
                // printf("--- STARTING ROUND. ---\n");
                // printf("Dealing Hands\n");
                break;

            case DEAL:     // Deal out cards
                dealCardToAllPlayers();
                dealCardToAllPlayers();
                
                gameState->stage = BUYIN;
                // printf("Buy In\n");
                startBetRound();

                gameLoopFreeze = 50;
                break;

            case BUYIN:     // Buy-ins
                doBetRoundTick();
                break;

            case FLOP:     // Deal out flop
                dealCardToRiver();
                dealCardToRiver();
                dealCardToRiver();

                gameState->stage = BETFLOP;
                // printf("Flop.\n");
                startBetRound();

                gameState->turn = (gameState->bigBlind + 1) % gameState->playerCount;
                gameLoopFreeze = 50;
                break;

            case BETFLOP:
                doBetRoundTick();
                break;

            case JOKER:
                gameState->stage = FINAL;  // jokers arent implemented yet
                // printf("Skipping jokers, Final river\n");
                break;

            case BETJOKER:
                break;

            case FINAL:
                dealCardToRiver();
                dealCardToRiver();

                gameState->stage = BETFINAL;
                startBetRound();

                gameLoopFreeze = 50;
                break;

            case BETFINAL:
                doBetRoundTick();
                break;

            case SHOWDOWN:
                // printf("Showdown.\n");

                gameState->stage = CLEANUP;    // showdown not implemented yet
                break;
        }

        SDL_Delay(20);
    }

    return 0;
}
