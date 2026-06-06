#pragma once

#include "cards.h"

extern Deck *mainDeck;

typedef struct {
    int ID;
    PlayerHand *Hand;
} Player;

int InitGame();
Player *GetLocalPlayer();
void StartRound();
void CloseGame();

