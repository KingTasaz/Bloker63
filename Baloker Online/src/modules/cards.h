#pragma once

#define cardWidth 225
#define cardHeight 312
#define cardHighlightScale 1.2

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
    int seed;       // for mildly random rotation i think
    int ID;         // position of texture in the cache
    int flipped;    // is the card upside down?
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
    Card Hand[maxHandSize];
    Card River[maxRiverSize];

    int handCount;
    int riverCount;
} PlayerHand;


Deck *CreateStandardDeck();
PlayerHand *CreateHand();
void printCard(Card card);
void shuffleDeck(Deck* deck);
Card drawFromDeck(Deck* deck);
void addToDeck(Deck* deck, Card card);
void addToHand(PlayerHand* player, Card card);
void addToRiver(PlayerHand* player, Card card);
void Deck_Destroy(Deck* deck);
void Hand_Destroy(PlayerHand* hand);
