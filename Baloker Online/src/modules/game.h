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

extern Chip bigBlindChip;
extern Chip smallBlindChip;
extern Chip turnOrderChip;

int InitGame(int numPlayers, SDL_Renderer *renderer);
Player *GetLocalPlayer();
void StartGameLoop();
void CloseGame();

int GetNumPlayers();
char *GetPlayerName(int id);
Player *GetPlayer(int id);
int getPot();
int getRaise();

void CallAction(enum PlayerAction a);

