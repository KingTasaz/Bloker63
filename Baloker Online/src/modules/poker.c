#include "poker.h"
#include "cards.h"

#define LoopRanks int rank = 0; rank < numRanks; rank++
#define LoopSuits int suit = 0; suit < numSuits; suit++
#define LoopCards int card = 0; card < numCards; card++

#define cardBufferSize 20

int card0 = 0;
int card1 = 0;

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
        int max_idx = i;

        for (int j = i + 1; j < n; j++)
        {
            if (cards[j].rank > cards[max_idx].rank)
            {
                max_idx = j;
            }
        }

        Card temp = cards[i];
        cards[i] = cards[max_idx];
        cards[max_idx] = temp;
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
    // printf("Unhighlighting...\n");

    // printf("- Hand\n");
    for (int h = 0; h < hand->handCount; h++) {
        hand->Hand[h].highlighted = 0;
    }

    // printf("- River\n");
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
    // printf("SEARCHING for flush five\n");

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

int detectFlushHouse(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for full house\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    sortCards(solvedCards, numCards);

    int FoundFlushHouse = 0;

    int highestPair = -1;
    int secondHighestPair = -1;

    for (LoopSuits) {
        for (int i = 0; i < numCards; i++) {
            if (solvedCards[i].suit != suit) { continue; }
            int count = 1;

            for (int j = i + 1; j < numCards; j++) {
                if (solvedCards[j].suit != suit) { continue; }

                if (solvedCards[i].rank == solvedCards[j].rank) {
                    count += 1;
                }

                if (count == 3) { break; }
            }

            if (count == 3) {
                highestPair = solvedCards[i].rank;
                break;
            }
        }

        for (int i = 0; i < numCards; i++) {
            if (solvedCards[i].suit != suit) { continue; }
            if (solvedCards[i].rank == highestPair) { continue; }

            int count = 1;

            for (int j = i + 1; j < numCards; j++) {
                if (solvedCards[j].suit != suit) { continue; }

                if (solvedCards[i].rank == solvedCards[j].rank) {
                    count += 1;
                }

                if (count == 2) { break; }
            }

            if (count == 2) {
                secondHighestPair = solvedCards[i].rank;
                break;
            }
        }

        if (highestPair != -1 || secondHighestPair != -1) {
            FoundFlushHouse = 1;
            break;
        }
    }

    if (!FoundFlushHouse) { return 0; }

    int _a = 0;
    int _b = 0;

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair && _a < 3) {
            solvedCards[i].rank = 999;
            _a++;
        }
        if (solvedCards[i].rank == secondHighestPair && _b < 2) {
            solvedCards[i].rank = 888;
            _b++;
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

    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);
    HighlightRank(bestHand, secondHighestPair);

    return 1;
}

int detectFiveKind(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for three kind\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 5) { break; }
        }

        if (count == 5) {
            highestPair = solvedCards[i].rank;
            break;
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

int detectStraightFlush(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for straight\n");

    int numCards = hand->handCount + hand->riverCount;

    if (numCards < 5) { return 0; }

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int IDs[5];
    int foundStraight = 0;

    sortCards(solvedCards, numCards);

    // printf("- searching for straights...\n");
    for (int rank = numRanks - 1; rank >= 0; rank--) {
        // printf(" - - starting on %d\n", rank);

        int found = 0;

        for (int i = 0; i < 5; i++) {
            int target = rank - i;
            int targetSuit = solvedCards[i].suit;

            if (target == -1) { target = numRanks - 1; }

            // printf(" - - - checking for %d\n", target);

            int found_target = 0;

            for (int j = 0; j < numCards; j++) {
                if (solvedCards[j].rank == target && solvedCards[j].suit == targetSuit) {
                    // printf(" - - - found required card\n");
                    IDs[i] = solvedCards[j].ID;
                    found_target = 1;
                    found++;
                    break;
                }
            }

            if (!found_target) { break; }
        }

        if (found >= 5) {
            foundStraight = 1;
            break;
        }
    }

    if (!foundStraight) { return 0; }

    // printf("- straight found: combing IDs...\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < numCards; j++) {
            if (IDs[i] == solvedCards[j].ID) {
                bestHand[i] = solvedCards[j];
            }
        }
    }

    // printf(" - highlighting...\n");
    HighlightAll(bestHand);

    return 1;
}

int detectRoyalFlush(PlayerHand *hand, Card bestHand[5])
{
    if (detectStraightFlush(hand, bestHand)) {
        return bestHand[0].suit == 12 ? 1 : 0;
    }

    return 0;
}

int detectFourKind(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for three kind\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 4) { break; }
        }

        if (count == 4) {
            highestPair = solvedCards[i].rank;
            break;
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

    card0 = highestPair;
    return 1;
}

int detectFullHouse(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for full house\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    sortCards(solvedCards, numCards);

    int highestPair = -1;
    int secondHighestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 3) { break; }
        }

        if (count == 3) {
            highestPair = solvedCards[i].rank;
            break;
        }
    }

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair) { continue; }

        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 2) { break; }
        }

        if (count == 2) {
            secondHighestPair = solvedCards[i].rank;
            break;
        }
    }

    if (highestPair == -1 || secondHighestPair == -1) { return 0; }

    int _a = 0;
    int _b = 0;

    for (int i = 0; i < numCards; i++) {
        if (solvedCards[i].rank == highestPair && _a < 3) {
            solvedCards[i].rank = 999;
            _a++;
        }
        if (solvedCards[i].rank == secondHighestPair && _b < 2) {
            solvedCards[i].rank = 888;
            _b++;
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

    copyBest5(solvedCards, bestHand, numCards);
    HighlightRank(bestHand, highestPair);
    HighlightRank(bestHand, secondHighestPair);

    return 1;
}

int detectFlush(PlayerHand *hand, Card bestHand[5])
{
    int numCards = hand->handCount + hand->riverCount;

    if (numCards < 5) { return 0; }

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int foundFlush = 0;

    for (LoopSuits) {
        int count = 0;

        for (LoopCards) {
            if (solvedCards[card].suit == suit) {
                bestHand[count] = solvedCards[card];
                count++;

                if (count == 5) {
                    foundFlush = 1;
                    break;
                }
            }
        }

        if (foundFlush) { break; }
    }

    if (!foundFlush) { return 0; }

    HighlightAll(bestHand);

    return 1;
}

int detectStraight(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for straight\n");

    int numCards = hand->handCount + hand->riverCount;

    if (numCards < 5) { return 0; }

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);

    int IDs[5];
    int foundStraight = 0;

    sortCards(solvedCards, numCards);

    // printf("- searching for straights...\n");
    for (int rank = numRanks - 1; rank >= 0; rank--) {
        // printf(" - - starting on %d\n", rank);

        int found = 0;

        for (int i = 0; i < 5; i++) {
            int target = rank - i;
            if (target == -1) { target = numRanks - 1; }

            // printf(" - - - checking for %d\n", target);

            int found_target = 0;

            for (int j = 0; j < numCards; j++) {
                if (solvedCards[j].rank == target) {
                    // printf(" - - - found required card\n");
                    IDs[i] = solvedCards[j].ID;
                    found_target = 1;
                    found++;
                    break;
                }
            }

            if (!found_target) { break; }
        }

        if (found >= 5) {
            foundStraight = 1;
            break;
        }
    }

    if (!foundStraight) { return 0; }

    // printf("- straight found: combing IDs...\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < numCards; j++) {
            if (IDs[i] == solvedCards[j].ID) {
                bestHand[i] = solvedCards[j];
            }
        }
    }

    // printf(" - highlighting...\n");
    HighlightAll(bestHand);
    return 1;
}

int detectThreeKind(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for three kind\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 3) { break; }
        }

        if (count == 3) {
            highestPair = solvedCards[i].rank;
            break;
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

    card0 = highestPair;
    return 1;
}

int detectTwoPair(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for two pair\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int highestPair = -1;
    int secondHighestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 2) { break; }
        }

        if (count == 2) {
            highestPair = solvedCards[i].rank;
            break;
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

        if (count == 2) {
            secondHighestPair = solvedCards[i].rank;
            break;
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

    card0 = highestPair;
    card1 = secondHighestPair;
    return 1;
}

int detectPair(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for pair\n");

    int numCards = hand->handCount + hand->riverCount;

    Card solvedCards[cardBufferSize];
    copyHandIntoBuffer(solvedCards, hand);
    sortCards(solvedCards, numCards);

    int highestPair = -1;

    for (int i = 0; i < numCards; i++) {
        int count = 1;

        for (int j = i + 1; j < numCards; j++) {
            if (solvedCards[i].rank == solvedCards[j].rank) {
                count += 1;
            }

            if (count == 2) { break; }
        }

        if (count == 2) {
            highestPair = solvedCards[i].rank;
            break;
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

    card0 = highestPair;
    return 1;
}

int detectHigh(PlayerHand *hand, Card bestHand[5])
{
    // printf("SEARCHING for high card\n");

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
    printf("--- Scoring Player Hand ---\n");

    Unhighlight(hand);
    // printf("- bestHand\n");
    for (int i = 0; i < 5; i++) {
        bestHand[i].highlighted = 0;
    }

    int score = -1;
    hand->handType = -1;
    hand->handScore = -1;

    card0 = 0;
    card1 = 0;

    int numCards = hand->handCount + hand->riverCount;
    if (numCards > 5) { numCards = 5; }

    // printf("There are %d cards.\n", numCards);

    if (detectFlushFive(hand, bestHand)) {
        score = 12;
    } else if (detectFlushHouse(hand, bestHand)) {
        score = 11;
    } else if (detectFiveKind(hand, bestHand)) {
        score = 10;
    } else if (detectRoyalFlush(hand, bestHand)) {
        score = 9;
    } else if (detectStraightFlush(hand, bestHand)) {
        score = 8;
    } else if (detectFourKind(hand, bestHand)) {
        score = 7;
    } else if (detectFullHouse(hand, bestHand)) {
        score = 6;
    } else if (detectFlush(hand, bestHand)) {
        score = 5;
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

    hand->handType = score;

    printf("Hand Score: %s", HandNames[score]);

    if (score != -1) {
        hand->handScore = score * 1000000 + card0 * 1000 + card1;
        CloneHighlights(hand, bestHand, numCards);
    }

    printf(" (%d + %d + %d = %d)\n", score, card0, card1, hand->handScore);

    return score;
}
