#include "SDL3/SDL_stdinc.h"
#include <stdlib.h>
#include <stdio.h>
#include "cards.h"

#include "../baloker.h"

const char *SuitNames[numSuits] = {
    "Spades",
    "Hearts",
    "Clubs",
    "Diamonds"
};

const char *RankNames[numRanks] = {
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Ten",
    "Jack",
    "King",
    "Queen",
    "Ace"
};

Deck *CreateDeck()
{
    Deck *deck = malloc(sizeof(Deck));  // TODO: Check for malloc failure
    deck->cardCount = 0;
    return deck;
}

PlayerHand *CreateHand()
{
    PlayerHand *Hand = malloc(sizeof(PlayerHand));
    Hand->handCount = 0;
    Hand->riverCount = 0;

    return Hand;
}


Deck *CreateStandardDeck()
{
    Deck *deck = CreateDeck();

    for (int suit = 0; suit < numSuits; suit++)
    {
        for (int rank = 0; rank < numRanks; rank++)
        {
            int idx = suit * numRanks + rank;

            Card card = {0};
            card.rank = rank;
            card.suit = suit;

            card.target_scale = 1.0;
            card.default_scale = 1.0;
            card.scale = 1.0;

            card.tx = DeckPosX;
            card.ty = DeckPosY;

            card.x = card.tx;
            card.y = card.ty;

            card.seed = SDL_rand(99999);
            card.flipped = 1;
            card.highlighted = 0;
            card.ID = idx;

            deck->Cards[idx] = card;
            deck->cardCount += 1;
        }
    }

    return deck;
}

void printCard(Card card)
{
    printf("%s of %s\n", RankNames[card.rank], SuitNames[card.suit]);
}

void shuffleDeck(Deck* deck)
{
    for (int i = deck->cardCount - 1; i > 0; i--)
    {
        int j = SDL_rand(1000) % (i + 1);

        // swap cards[i] and cards[j]
        Card temp = deck->Cards[i];
        deck->Cards[i] = deck->Cards[j];
        deck->Cards[j] = temp;
    }
}

Card drawFromDeck(Deck* deck)
{
    Card card = deck->Cards[deck->cardCount-1];
    deck->cardCount -= 1;
    return card;
}

void addToDeck(Deck* deck, Card card)
{
    if (deck->cardCount < maxDeckSize)
    {
        deck->Cards[deck->cardCount] = card;
        deck->cardCount += 1;
    }
}

void addToHand(PlayerHand* player, Card card)
{
    if (player->handCount < maxHandSize)
    {
        player->Hand[player->handCount] = card;
        player->handCount += 1;
    }
}

void addToRiver(PlayerHand* player, Card card)
{
    if (player->riverCount < maxRiverSize)
    {
        player->River[player->riverCount] = card;
        player->riverCount += 1;
    }
}

void Deck_Destroy(Deck* deck)
{
    free(deck);
}

void Hand_Destroy(PlayerHand* hand)
{
    free(hand);
}

