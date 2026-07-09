#pragma once

#include "cards.h"
#include "UI.h"
#include "network.h"

extern Deck *mainDeck;

extern int PlayerCX[];
extern int PlayerCY[];

BalokerLobbyHeader currentLobby;
int lobbyOwnerIsMe;
extern char playerNames[maxPlayers][USERNAMEMAXLENGTH];

int GameReady;
int LocalPlayer;

typedef struct {
    int ID;
    int folded;
    int Chips;
    int myRaise;
    int targetRaiseAmt;
    PlayerHand *Hand;
    Card BestHand[5];
} Player;

extern Player *Players;

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

    int Winner;
    int Tie;
    int WinningHand;

    enum RoundStage stage;
    enum PlayerAction action;
} GameState_t;

GameState_t *gameState;

extern Chip bigBlindChip;
extern Chip smallBlindChip;
extern Chip turnOrderChip;

int InitGame(SDL_Renderer *renderer);
Player *GetLocalPlayer();
void StartGameLoop();
void CloseGame();

char *GetPlayerName(int id);
Player *GetPlayer(int id);
int getTotalPot();

void CallAction(enum PlayerAction a);

int GetPlrPosIDFromSlot(int slot);

