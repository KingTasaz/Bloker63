#pragma once

#include "cards.h"

extern char *HandNames[13];


/*
Takes in a player's hand (Hand + River) and an empty list.

Returns `int` to be used for indexing the hand names,
`-1` if no hand found (i.e. empty hand)

Sets `PlayerHand.handType` to the score of the poker hand rank.
Scoring is based on hand type (e.g. Flush = 500) and
the highest ranked card that makes the type (e.g. 500 + 6 = 506).

`bestHand` is made of the 5 best cards that can be played (For tiebreakaing).

This will automatically highlight the scored cards.
*/
int GetBestPokerHand(PlayerHand *hand, Card bestHand[5]);
