#pragma once

#include "cards.h"

extern Deck *mainDeck;

extern int PlayerCX[];
extern int PlayerCY[];

typedef struct {
    int ID;
    int folded;
    int Chips;
    PlayerHand *Hand;
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

int InitGame();
Player *GetLocalPlayer();
void StartRound();
void CloseGame();

int GetNumPlayers();
char *GetPlayerName(int id);
Player *GetPlayer(int id);

void CallAction(enum PlayerAction a);

