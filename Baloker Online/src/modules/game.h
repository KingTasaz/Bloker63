#pragma once

#include "cards.h"
#include "UI.h"

extern Deck *mainDeck;

extern int PlayerCX[];
extern int PlayerCY[];

typedef struct {
    int ID;
    int folded;
    int Chips;
    PlayerHand *Hand;
    Card BestHand[5];
} Player;

enum PlayerAction {
    NONE,   // waiting for player to select an action
    FOLD,
    CHECK,
    CALL,
    RAISE
};

enum RoundStage {
    WAITING = 0,
    DEAL,
    BUYIN,
    FLOP,
    BETFLOP,
    JOKER,
    BETJOKER,
    FINAL,
    BETFINAL,
    SHOWDOWN,
    CLEANUP
};

typedef struct {
    int playerCount;

    // Turn orders
    int smallBlind;
    int bigBlind;
    int turn;
    int lastCall;

    int Pot[maxPlayers];
    int Raise;
    int myRaise;

    enum RoundStage stage;
    enum PlayerAction action;

    Card myBestHand[5];
} GameState_t;

extern GameState_t *gameState;

extern Chip bigBlindChip;
extern Chip smallBlindChip;
extern Chip turnOrderChip;

int InitGame(int numPlayers, SDL_Renderer *renderer);
Player *GetLocalPlayer();
void StartGameLoop();
void CloseGame();

char *GetPlayerName(int id);
Player *GetPlayer(int id);
int getTotalPot();

void CallAction(enum PlayerAction a);

