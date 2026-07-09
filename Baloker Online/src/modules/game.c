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

LocalPlayer = -1;

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

Chip bigBlindChip = {0};
Chip smallBlindChip = {0};
Chip turnOrderChip = {0};
SDL_Surface *tempSurface;

char playerNames[maxPlayers][USERNAMEMAXLENGTH];

// Game Variables
float timer;
int gameLoopFreeze = 0;

int GameReady = 0;


int InitGame(SDL_Renderer *renderer)
{
    if (Init) { return 1; }

    gameState = calloc(sizeof(GameState_t), 1);
    mainDeck = CreateStandardDeck();

    Players = malloc(sizeof(Player) * maxPlayers);
    for (int i = 0; i < maxPlayers; i++) {
        Players[i].ID = i;
        Players[i].Hand = CreateHand();
        Players[i].folded = 0;
        Players[i].Chips = 0;
        Players[i].myRaise = 0;
        Players[i].targetRaiseAmt = 10;
    }

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
    GameReady = 1;
    return 0;
}

Player *GetLocalPlayer()
{
    if (!Init) { return NULL; }
    return &Players[LocalPlayer];
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

    gameState->smallBlind = 0;
    gameState->bigBlind = (gameState->smallBlind + 1) % gameState->playerCount;
    gameState->stage = WAITING;

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

int GetPlrPosIDFromSlot(int slot) {
    if (slot < LocalPlayer)
        return slot + 1;

    if (slot == LocalPlayer)
        return 0;

    return slot;
}

// MARK: ASYNC
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
    float cx = WIDTH / 2;
    float cy = HEIGHT * 0.85;

    int numCards = lp->Hand->handCount;
    float left = cx - (cardWidth / 2 + 25) * (numCards - 1);

    for (int i = 0; i < numCards; i++) {
        lp->Hand->Hand[i].tx = left + (cardWidth + 50) * i;
        lp->Hand->Hand[i].ty = cy;
    }

    // Move cards in River
    cy = HEIGHT * 0.5;

    numCards = lp->Hand->riverCount;
    left = cx - (cardWidth / 2 + 25) * (numCards - 1);

    for (int i = 0; i < numCards; i++) {
        lp->Hand->River[i].tx = left + (cardWidth + 50) * i;
        lp->Hand->River[i].ty = cy;
    }

    // Move cards for other players
    for (int p = 1; p < gameState->playerCount; p++)
    {
        cx = (float)PlayerCX[GetPlrPosIDFromSlot(p)];
        cy = (float)PlayerCY[GetPlrPosIDFromSlot(p)] - 20;

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
        
        if (plr == LocalPlayer) {
            card.flipped = 0;
        } else {
            card.default_scale = 0.25;
        }

        addToHand(Players[plr].Hand, card);

        ReorganizeCardPositions();
        SDL_Delay(100);
    }

    GetBestPokerHand(GetLocalPlayer()->Hand, GetLocalPlayer()->BestHand, true);
}

// Delay: 100ms
void dealCardToRiver()
{
    if (mainDeck->cardCount == 0) { return; }

    Card card = drawFromDeck(mainDeck);
    card.flipped = 0;

    for (int p = 0; p < gameState->playerCount; p++)
        addToRiver(Players[p].Hand, card);

    ReorganizeCardPositions();
    SDL_Delay(100);

    GetBestPokerHand(GetLocalPlayer()->Hand, GetLocalPlayer()->BestHand, true);
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
        turnOrderChip.ty = HEIGHT / 2;
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
        turnOrderChip.ty = HEIGHT / 2;
        printf("Everyone but 1 folded.\n");
        return;
    }

    turnOrderChip.tx = TurnChipX[GetPlrPosIDFromSlot(gameState->turn)];
    turnOrderChip.ty = TurnChipY[GetPlrPosIDFromSlot(gameState->turn)];

    // printf("Current Player: %s\n", playerNames[turn]);
}

void startBetRound()
{
    gameState->action = NONE;
    gameState->Raise = 0;
    
    for (int p = 0; p < gameState->playerCount; p++) {
        Players[p].myRaise = 0;
    }

    int lC = gameState->bigBlind;
    int det = lC;
    while (Players[lC].folded || Players[lC].Chips == 0) {
        lC = (lC + 1) % gameState->playerCount;

        if (lC == det) {
            printf("Everyone is folded? Or broke?\n");
            break;
        }
    }

    int turn = (gameState->bigBlind + 1) % gameState->playerCount;
    det = turn;
    while (Players[turn].folded || Players[turn].Chips == 0) {
        turn = (turn + 1) % gameState->playerCount;

        if (turn == det) {
            printf("Everyone is folded? Or broke?\n");
            break;
        }
    }
    
    gameState->turn = turn;
    gameState->lastCall = lC;

    turnOrderChip.tx = TurnChipX[GetPlrPosIDFromSlot(turn)];
    turnOrderChip.ty = TurnChipY[GetPlrPosIDFromSlot(turn)];

    // printf("--- BETTING ROUND STARTED ---\n");
    // printf("Current Player: %s\n", playerNames[turn]);
}

void PutInPot(int plr, int amount, int isRaise)
{
    if (isRaise) {
        gameState->Raise += amount;
        gameState->Pot[0] += amount;

        Players[plr].Chips -= (gameState->Raise - Players[plr].myRaise);
        if (Players[plr].Chips < 0)
            Players[plr].Chips = 0;

        Players[plr].myRaise = gameState->Raise;

        return;
    }

    // else: calling or joker effect
    gameState->Pot[0] += amount;
    Players[plr].Chips -= amount;
    Players[plr].myRaise = gameState->Raise;
}

void doBetRoundTick()
{
    switch (gameState->action) {
        case NONE:
            break;

        case CHECK:
            if (gameState->Raise == 0 || Players[gameState->turn].myRaise == gameState->Raise) {
                // printf(" -> Check.\n");
                startNextPlayerAction();
            } else {
                gameState->action = NONE;
            } break;

        case RAISE:
            if (Players[gameState->turn].Chips <= 0) { return; }

            PutInPot(gameState->turn, Players[gameState->turn].targetRaiseAmt, true);
            printf("Player %d: Raise for %d\n", gameState->turn, Players[gameState->turn].targetRaiseAmt);
            startNextPlayerAction();
            break;

        case CALL:
            int amnt = gameState->Raise - Players[gameState->turn].myRaise;
            int chips = Players[gameState->turn].Chips;

            printf("Player %d: Call for %d (Has %d)\n", gameState->turn, amnt, chips);

            if (amnt > chips) {
                PutInPot(gameState->turn, chips, false);
            } else {
                PutInPot(gameState->turn, amnt, false);
            }

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

void ShowAllCards() {
    for (int p = 0; p < gameState->playerCount; p++) {
        for (int c = 0; c < Players[p].Hand->handCount; c++) {
            Players[p].Hand->Hand[c].flipped = 0;
        }
    }
}

void DetermineWinner() {
    printf("DETERMINING WINNER\n");

    int bestHand = -1;
    int bestHandType = -1;
    int bestPlrID = -1;

    int Tie = 0;
    int Tied[maxPlayers];

    for (int p = 0; p < gameState->playerCount; p++) {
        printf("%s: ", GetPlayerName(p));

        GetBestPokerHand(Players[p].Hand, Players[p].BestHand, false);
        int hand = Players[p].Hand->handScore;

        printf("-> %d\n", hand);

        if (hand > bestHand) {
            bestHand = hand;
            bestHandType = Players[p].Hand->handType;
            bestPlrID = p;
            Tie = 0;
            Tied[0] = p;
        } else if (hand == bestHand) {
            Tied[++Tie] = p;
        }
    }

    gameState->WinningHand = bestHandType;

    if (Tie == 0) {
        printf("Hand Winner: %s\n", GetPlayerName(bestPlrID));
        Players[bestPlrID].Chips += getTotalPot();

        gameState->Winner = bestPlrID;
        return;
    }

    gameState->Winner = 0;
    gameState->Tie = true;

    printf("There was a tie.\n");
    int winnings = getTotalPot() / Tie;

    for (int i = 0; i < Tie; i++)
        Players[Tied[i]].Chips += winnings;
}

void CleanupRound() {
    for (int p = 0; p < gameState->playerCount; p++) {
        Players[p].Hand->handCount = 0;
        Players[p].Hand->riverCount = 0;
        Players[p].folded = 0;
    }

    gameState->bigBlind = (gameState->bigBlind + 1) % gameState->playerCount;
    gameState->smallBlind = (gameState->smallBlind + 1) % gameState->playerCount;

    Deck_Destroy(mainDeck);
    mainDeck = CreateStandardDeck();
}

void PrepareNextRound() {
    printf("Preparing next round...\n");

    gameState->turn = (gameState->bigBlind + 1) % gameState->playerCount;
    gameState->lastCall = gameState->bigBlind;

    gameState->Winner = -1;
    gameState->Tie = false;
    gameState->WinningHand = -1;

    bigBlindChip.tx = ChipX[GetPlrPosIDFromSlot(gameState->bigBlind)];
    bigBlindChip.ty = ChipY[GetPlrPosIDFromSlot(gameState->bigBlind)];
    smallBlindChip.tx = ChipX[GetPlrPosIDFromSlot(gameState->smallBlind)];
    smallBlindChip.ty = ChipY[GetPlrPosIDFromSlot(gameState->smallBlind)];
    turnOrderChip.tx = TurnChipX[GetPlrPosIDFromSlot(gameState->turn)];
    turnOrderChip.ty = TurnChipY[GetPlrPosIDFromSlot(gameState->turn)];

    for (int i = 0; i < maxPlayers; i++) { gameState->Pot[i] = 0; }
    gameState->Raise = 0;

    // Shuffle the deck
    mainDeck = CreateStandardDeck();
    shuffleDeck(mainDeck);
}

void BurnAllCards()
{
    for (int p = 0; p < gameState->playerCount; p++)
    {
        for (int i = 0; i < Players[p].Hand->handCount; i++)
        {
            Players[p].Hand->Hand[i].burning = true;
        }

        for (int i = 0; i < Players[p].Hand->riverCount; i++)
        {
            Players[p].Hand->River[i].burning = true;
        }
    }
}

int GameLoop(void *data)
{
    int showdownA = false;
    int cleanupA = false;

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
                PrepareNextRound();
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

                PutInPot(gameState->smallBlind, 1, false);
                Players[gameState->smallBlind].myRaise = 1;
                PutInPot(gameState->bigBlind, 2, true);

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
                showdownA = false;
                startBetRound();

                gameLoopFreeze = 50;
                break;

            case BETFINAL:
                doBetRoundTick();
                break;

            case SHOWDOWN:
                // printf("Showdown.\n");

                if (!showdownA) {
                    ShowAllCards();
                    showdownA = true;
                    gameLoopFreeze = 50;
                    break;
                }

                DetermineWinner();

                gameState->stage = CLEANUP;    // showdown not implemented yet
                cleanupA = false;
                gameLoopFreeze = 50 * 2;
                break;

            case CLEANUP:
                if (!cleanupA) {
                    BurnAllCards();
                    cleanupA = true;
                    gameLoopFreeze = 50;
                    break;
                }

                CleanupRound();
                gameLoopFreeze = 50;
                gameState->stage = WAITING;
        }

        SDL_Delay(20);
    }

    return 0;
}
