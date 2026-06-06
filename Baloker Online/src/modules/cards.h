#pragma once

#define cardWidth 225
#define cardHeight 312

#define maxDeckSize 52
#define maxHandSize 12
#define maxRiverSize 12

#define numSuits 4
#define numRanks 13

extern const char *SuitNames[numSuits];
extern const char *RankNames[numRanks];

typedef struct {
    int rank;
    int suit;
    int isJoker;
    float x, y;
    float tx, ty;   // target x and y
    float scale;
    float target_scale;
} Card;

typedef struct {
    Card Cards[maxDeckSize];
    int cardCount;
} Deck;

typedef struct {
    Card *Hand;
    Card *River;

    int handCount;
    int riverCount;
} PlayerHand;


Deck *CreateStandardDeck();
void printCard(Card card);
void shuffleDeck(Deck* deck);
