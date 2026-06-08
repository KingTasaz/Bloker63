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
    960 - 750,
    960 - 500,
    960 - 250,
    960,
    960 + 250,
    960 + 500,
    960 + 750
};

int PlayerCY[] = {
    1000,
    200,
    200,
    200,
    200,
    200,
    200,
    200
};

int ChipX[] = {
    240         - 50,
    880 - 750   - 30,
    880 - 500   - 30,
    880 - 250   - 30,
    880         - 30,
    880 + 250   - 30,
    880 + 500   - 30,
    880 + 750   - 30
};

int ChipY[] = {
    840,
    160,
    160,
    160,
    160,
    160,
    160,
    160
};

int TurnChipX[] = {
    330         - 50,
    880 - 750   + 30,
    880 - 500   + 30,
    880 - 250   + 30,
    880         + 30,
    880 + 250   + 30,
    880 + 500   + 30,
    880 + 750   + 30
};

int TurnChipY[] = {
    820,
    30,
    30,
    30,
    30,
    30,
    30,
    30
};

Chip bigBlindChip = {0};
Chip smallBlindChip = {0};
Chip turnOrderChip = {0};
SDL_Surface *tempSurface;

char *playerNames[maxPlayers];

// Game Variables
int playerCount;

enum RoundStage stage = WAITING;
enum PlayerAction action = NONE;

int smallBlind;
int bigBlind;
int turn;       // whose turn is it?
int lastCall;

int Pot[maxPlayers];
int Raise;

int myHandType = -1;
Card myBestHand[5];

float timer;
int gameLoopFreeze = 0;


int InitGame(int numPlayers, SDL_Renderer *renderer)
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
        Players[i].folded = 0;
        Players[i].Chips = 100;
    }

    Init = 1;
    playerCount = numPlayers;

    // Player Name Defaults For Testing
    playerNames[0] = "LocalPlayer";
    playerNames[1] = "Rohan";
    playerNames[2] = "Jason";

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

void StartGameLoop()
{
    if (!Init) { return; }

    if (playerCount < 2) {
        printf("Warning. Starting game with less than 2 players.\n");
    }

    // initialize variables
    stage = WAITING;

    smallBlind = 0;
    bigBlind = (smallBlind + 1) % playerCount;
    turn = (bigBlind + 1) % playerCount;
    lastCall = bigBlind;

    bigBlindChip.tx = ChipX[bigBlind];
    bigBlindChip.ty = ChipY[bigBlind];
    smallBlindChip.tx = ChipX[smallBlind];
    smallBlindChip.ty = ChipY[smallBlind];
    turnOrderChip.tx = TurnChipX[turn];
    turnOrderChip.ty = TurnChipY[turn];

    for (int i = 0; i < maxPlayers; i++) { Pot[i] = 0; }
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

void CallAction(enum PlayerAction a)
{
    action = a;
}

int getPot() {
    int sum = 0;
    for (int i = 0; i < maxPlayers; i++) { sum += Pot[i]; }
    return sum;
}

int getRaise() { return Raise; }

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
    if (action == RAISE) {
        lastCall = turn - 1;
        if (lastCall == -1) { lastCall = playerCount -1; }
    }
    
    if (turn == lastCall) {
        stage++;
        gameLoopFreeze = 50;
        action = NONE;
        turnOrderChip.tx = -200;
        turnOrderChip.ty = height / 2;
        return;
    }

    action = NONE;
    int loopDetect = turn;

    do {
        turn++;
        turn %= playerCount;
    } while (Players[turn].folded);

    int AllFolded = 1;
    for (int i = 1; i < playerCount; i++) {
        int p = (turn + i) % playerCount;
        if (!Players[p].folded) {
            AllFolded = 0;
            break;
        }
    }

    if (AllFolded) {
        stage = SHOWDOWN;
        turnOrderChip.tx = -200;
        turnOrderChip.ty = height / 2;
        printf("Everyone but 1 folded.\n");
        return;
    }

    turnOrderChip.tx = TurnChipX[turn];
    turnOrderChip.ty = TurnChipY[turn];

    printf("Current Player: %s\n", playerNames[turn]);
}

void startBetRound()
{
    action = NONE;
    Raise = 0;
    
    turn = (bigBlind + 1) % playerCount;
    lastCall = bigBlind;

    turnOrderChip.tx = TurnChipX[turn];
    turnOrderChip.ty = TurnChipY[turn];

    printf("--- BETTING ROUND STARTED ---\n");
    printf("Current Player: %s\n", playerNames[turn]);
}

void doBetRoundTick()
{
    switch (action) {
        case NONE:
            break;

        case CHECK:
            if (Raise == 0) {
                printf(" -> Check.\n");
                startNextPlayerAction();
            } else {
                action = NONE;
            } break;

        case RAISE:
            Raise += 1;
            Pot[0] += 1;
            printf(" -> Raise to %d\n", Raise);
            startNextPlayerAction();
            break;

        case CALL:
            printf(" -> Call for %d\n", Raise);
            startNextPlayerAction();
            break;

        case FOLD:
            Players[turn].folded = 1;
            printf(" -> Fold.\n");
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
                printf("--- STARTING ROUND. ---\n");
                printf("Dealing Hands\n");
                break;

            case DEAL:     // Deal out cards
                dealCardToAllPlayers();
                dealCardToAllPlayers();

                myHandType = GetBestPokerHand(GetLocalPlayer()->Hand, myBestHand);
                
                stage = BUYIN;
                printf("Buy In\n");
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

                myHandType = GetBestPokerHand(GetLocalPlayer()->Hand, myBestHand);

                stage = BETFLOP;
                printf("Flop.\n");
                startBetRound();

                turn = (bigBlind + 1) % playerCount;
                gameLoopFreeze = 50;
                break;

            case BETFLOP:
                doBetRoundTick();
                break;

            case JOKER:
                stage = FINAL;  // jokers arent implemented yet
                printf("Skipping jokers, Final river\n");
                break;

            case BETJOKER:
                break;

            case FINAL:
                dealCardToRiver();
                dealCardToRiver();

                myHandType = GetBestPokerHand(GetLocalPlayer()->Hand, myBestHand);

                stage = BETFINAL;
                startBetRound();

                gameLoopFreeze = 50;
                break;

            case BETFINAL:
                doBetRoundTick();
                break;

            case SHOWDOWN:
                printf("Showdown.\n");

                stage = CLEANUP;    // showdown not implemented yet
                break;
        }

        SDL_Delay(20);
    }

    return 0;
}
