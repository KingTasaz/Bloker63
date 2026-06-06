#include "SDL3/SDL_stdinc.h"
#include <stdlib.h>
#include <stdio.h>
#include "cards.h"

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
    Deck *deck = malloc(sizeof(Deck));
    deck->cardCount = 0;
    return deck;
}


Deck *CreateStandardDeck()
{
    Deck *deck = CreateDeck();

    for (int suit = 0; suit < numSuits; suit++)
    {
        for (int rank = 0; rank < numRanks; rank++)
        {
            Card card = {0};
            card.rank = rank;
            card.suit = suit;
            card.scale = 1.0;
            card.target_scale = 1.0;

            int idx = suit * numRanks + rank;
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

