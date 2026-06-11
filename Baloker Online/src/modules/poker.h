#pragma once

#include "cards.h"

extern char *HandNames[13];


/*
Takes in a player's hand (Hand + River) and an empty list.

Returns `int` to be used for indexing the hand names,
`-1` if no hand found (i.e. empty hand)

Sets `PlayerHand.handType` to the type of poker hand (for displaying its name).
Sets `Playerhand.handScore` to the score of the hand.
Formula: (type * 1,000,000) + (card0 * 1,000) + (card1)
where card0 and card1 are the highest (and second highest, if applicable) card ranks that appear in the hand.

`bestHand` is made of the 5 best cards that can be played (For tiebreakaing).

This will automatically highlight the scored cards.
*/
int GetBestPokerHand(PlayerHand *hand, Card bestHand[5]);
