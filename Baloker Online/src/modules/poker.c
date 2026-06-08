#include "poker.h"
#include "cards.h"

#define LoopRanks int rank = 0; rank < numRanks; rank++
#define LoopSuits int suit = 0; suit < numSuits; suit++
#define LoopCards int card = 0; card < numCards; card++

#define cardBufferSize 20

char *HandNames[] = {
    "High Card",
    "Pair",
    "Two Pair",
    "Three of a Kind",
    "Straight",
    "Flush",
    "Full House",
    "Four of a Kind",
    "Straight Flush",
    "Royal Flush",
    "Five of a Kind",
    "Flush House",
    "Flush Five"
};

void copyHandIntoBuffer(Card card[cardBufferSize], PlayerHand *hand)
{
    int i = 0;

    for (int h = 0; h < hand->handCount; h++) {
        card[h] = hand->Hand[h];
        i = h + 1;
    }

    for (int h = 0; h < hand->riverCount; h++) {
        card[i+h] = hand->River[h];
    }
}

void sortCards(Card cards[cardBufferSize], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int min_idx = i;
        int vmin = cards[min_idx].rank;

        for (int j = i + 1; j < n; j++)
        {
            int vj = cards[j].rank;

            if (vj >= vmin)
            {
                min_idx = j;
                vmin = vj;
            }
        }

        Card temp = cards[i];
        cards[i] = cards[min_idx];
        cards[min_idx] = temp;
    }
}

void copyBest5(Card best[cardBufferSize], Card bestHand[5], int max)
{
    if (max >= 1) { bestHand[0] = best[0]; }
    if (max >= 2) { bestHand[1] = best[1]; }
    if (max >= 3) { bestHand[2] = best[2]; }
    if (max >= 4) { bestHand[3] = best[3]; }
    if (max >= 5) { bestHand[4] = best[4]; }
}

void Unhighlight(PlayerHand *hand)
{
    for (int h = 0; h < hand->handCount; h++) {
        hand->Hand[h].highlighted = 0;
    }

    for (int h = 0; h < hand->riverCount; h++) {
        hand->River[h].highlighted = 0;
    }
}

// highlight all cards with given rank
void HighlightRank(Card bestHand[5], int Rank)
{
    for (int i = 0; i < 5; i++) {
        if (bestHand[i].rank == Rank) {
            bestHand[i].highlighted = 1;
        }
    }
}

// highlight all cards with given suit
void HighlightSuit(Card bestHand[5], int Suit)
{
    for (int i = 0; i < 5; i++) {
        if (bestHand[i].suit == Suit) {
            bestHand[i].highlighted = 1;
        }
    }
}

// highlight all cards with given suit and rank
void HighlightSuitRank(Card bestHand[5], int Suit, int Rank)
{
    for (int i = 0; i < 5; i++) {
        if (bestHand[i].suit == Suit && bestHand[i].rank == Rank) {
            bestHand[i].highlighted = 1;
        }
    }
}

void HighlightAll(Card bestHand[5])
{
   for (int i = 0; i < 5; i++) {
        bestHand[i].highlighted = 1;
    } 
}

void CloneHighlights(PlayerHand *hand, Card bestHand[5], int n)
{
    for (int i = 0; i < n; i++) {
        for (int h = 0; h < hand->handCount; h++) {
            if (hand->Hand[h].ID == bestHand[i].ID) {
                hand->Hand[h].highlighted = bestHand[i].highlighted;
            }
        }
        for (int h = 0; h < hand->riverCount; h++) {
            if (hand->River[h].ID == bestHand[i].ID) {
                hand->River[h].highlighted = bestHand[i].highlighted;
            }
        }
    }
}

// Detectors
int detectFlushFive(PlayerHand *hand, Card bestHand[5])
{    
    int numCards = hand->handCount + hand->riverCount;

    Card totalCards[cardBufferSize];
    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(totalCards, hand);

    int highestRank = -1;
    int highestSuit = -1;

    for (LoopRanks) {
        for (LoopSuits) {
            int count = 0;
            int loopList[cardBufferSize];

            for (LoopCards) {
                if (totalCards[card].rank == rank && totalCards[card].suit == suit) {
                    loopList[count] = card;
                    count++;
                }
            }

            if (count >= 5) {
                highestRank = rank;
                highestSuit = suit;

                for (int i = 0; i < count; i++) {
                    solvedCards[i] = totalCards[loopList[i]];
                }
            }
        }
    }

    if (highestRank == -1) { return 0; }

    // sortCards(solvedCards); Dont need to sort it... its a flush five.
    copyBest5(solvedCards, bestHand, numCards);
    HighlightSuitRank(bestHand, highestSuit, highestRank);

    return 1;
}

int detectFlushHouse(PlayerHand *hand, Card bestHand[5]);

int detectFiveKind(PlayerHand *hand, Card bestHand[5]);

int detectRoyalFlush(PlayerHand *hand, Card bestHand[5]);

int detectStraightFlush(PlayerHand *hand, Card bestHand[5]);

int detectFourKind(PlayerHand *hand, Card bestHand[5]);

int detectFullHouse(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int highestPair = -1;
    int secondHighestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 3 && solvedCards[i].rank > highestPair) {
            highestPair = solvedCards[i].rank;
        }
    }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) { continue; }

        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 2 && solvedCards[i].rank > secondHighestPair) {
            secondHighestPair = solvedCards[i].rank;
        }
    }

    if (highestPair == -1 || secondHighestPair == -1) { return 0; }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) {
            solvedCards[i].rank = 999;
        }
        if (solvedCards[i].rank == secondHighestPair) {
            solvedCards[i].rank = 888;
        }
    }

    sortCards(solvedCards, numCards);

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == 999) {
            solvedCards[i].rank = highestPair;
        }
        if (solvedCards[i].rank == 888) {
            solvedCards[i].rank = secondHighestPair;
        }
    }

    sortCards(solvedCards, 5);
    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);
    HighlightRank(bestHand, secondHighestPair);

    return 1;
}

int detectFlush(PlayerHand *hand, Card bestHand[5]);

int detectStraight(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int IDs[5];
    int foundStraight = 0;

    sortCards(solvedCards, numCards);

    for (int i = numCards - 1; i >= 0; i--) {
        int length = 1;

        int start = solvedCards[i].rank;
        IDs[0] = solvedCards[i].ID;

        int found = 1;
        while (found) {
            int j = i - length;
            if (j == -1) { break; }
            if (length == 5) { break; }
            if (solvedCards[j].rank == start - length + 1) { continue; }
            if (solvedCards[j].rank == start - length || (solvedCards[j].rank == 12 && start - length == -1)) {
                IDs[length] = solvedCards[j].ID;
                length++;
            } else {
                found = 0;
            }
        }

        if (length == 5) {
            foundStraight = 1;
            break;
        }
    }

    if (!foundStraight) { return 0; }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < numCards; j++) {
            if (IDs[i] == solvedCards[j].ID) {
                bestHand[i] = solvedCards[j];
            }
        }
    }

    HighlightAll(bestHand);
    return 1;
}

int detectThreeKind(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 3 && solvedCards[i].rank > highestPair) {
            highestPair = solvedCards[i].rank;
        }
    }

    if (highestPair == -1) { return 0; }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) {
            solvedCards[i].rank = 999;
        }
    }

    sortCards(solvedCards, numCards);

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == 999) {
            solvedCards[i].rank = highestPair;
        }
    }

    sortCards(solvedCards, 5);
    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);

    return 1;
}

int detectTwoPair(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int highestPair = -1;
    int secondHighestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 2 && solvedCards[i].rank > highestPair) {
            highestPair = solvedCards[i].rank;
        }
    }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank >= highestPair) { continue; }

        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 2 && solvedCards[i].rank > secondHighestPair) {
            secondHighestPair = solvedCards[i].rank;
        }
    }

    if (highestPair == -1 || secondHighestPair == -1) { return 0; }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) {
            solvedCards[i].rank = 999;
        }
        if (solvedCards[i].rank == secondHighestPair) {
            solvedCards[i].rank = 888;
        }
    }

    sortCards(solvedCards, numCards);

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == 999) {
            solvedCards[i].rank = highestPair;
        }
        if (solvedCards[i].rank == 888) {
            solvedCards[i].rank = secondHighestPair;
        }
    }

    sortCards(solvedCards, 5);
    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);
    HighlightRank(bestHand, secondHighestPair);

    return 1;
}

int detectPair(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }
        }

        if (count == 2 && solvedCards[i].rank > highestPair) {
            highestPair = solvedCards[i].rank;
        }
    }

    if (highestPair == -1) { return 0; }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) {
            solvedCards[i].rank = 999;
        }
    }

    sortCards(solvedCards, numCards);

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == 999) {
            solvedCards[i].rank = highestPair;
        }
    }

    sortCards(solvedCards, 5);
    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);

    return 1;
}

int detectHigh(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    if (numCards == 0) { return 0; }

    Card totalCards[cardBufferSize];
    copyHandIntoBuffer(totalCards, hand);

    sortCards(totalCards, numCards);
    copyBest5(totalCards, bestHand, numCards);
    bestHand[0].highlighted = 1;

    return 1;
}

// Global
int GetBestPokerHand(PlayerHand *hand, Card bestHand[5])
{
    Unhighlight(hand);
    for (int i = 0; i < 5; i++) {
        bestHand[i].highlighted = 0;
    }

    int score = -1;

    int numCards = hand->handCount + hand->riverCount;
    if (numCards > 5) { numCards = 5; }

    if (detectFlushFive(hand, bestHand)) {
        score = 12;
    } else if (detectStraight(hand, bestHand)) {
        score = 4;
    } else if (detectThreeKind(hand, bestHand)) {
        score = 3;
    } else if (detectTwoPair(hand, bestHand)) {
        score = 2;
    } else if (detectPair(hand, bestHand)) {
        score = 1;
    } else if (detectHigh(hand, bestHand)) {
        score = 0;
    } 

    printf("Hand Score: %d\n", score);

    if (score != -1) {
        hand->handType = score * 100 + bestHand[0].rank;
        CloneHighlights(hand, bestHand, numCards);
    }

    printCard(bestHand[0]);

    return score;
}
