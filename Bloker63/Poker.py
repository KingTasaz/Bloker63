import random
import matplotlib.pyplot as plt

import Bloker63
from Bloker63.Cards import *


HandNames = [
    "Pair",
    "Two Pair",
    "Three of a Kind",
    "Straight",
    "Flush",
    "Full House",
    "Four of a Kind",
    "Straight Flush",
    "Royal Flush"
]

Occurances = [0] * len(HandNames)


def _getStraightLength(orderedHand: list[Card]) -> int:
    longest = 0

    for i in range(len(orderedHand) - 1):
        length = 0
        rank = orderedHand[i].rank
        for j in range(i+1, len(orderedHand)):
            if orderedHand[j].rank == rank + 1:
                length += 1
                rank += 1
                longest = max(longest, length)
            else:
                break

    return longest


def AnalyzeHand(Hand: Deck) -> None:
    # Find Pairs
    Hand.h_Pairs.clear()
    for rank in RankIDs:
        pair = []

        for card in Hand:
            if card.rank == rank or card.rank == 0:
                pair.append(card)

        if len(pair) > 1:
            Hand.h_Pairs.append(pair)

    # Find Straights
    Hand.h_Straights.clear()
    for start in RankIDs:
        if start > 10:
            break # if wraparounds are allowed then just %15 + 2 or smth

        straight = []

        for rank in range(start, 15):
            for card in Hand:
                if card.rank == rank or (card.rank == 0 and card not in straight):
                    straight.append(card)
                    break
            else:
                break

        if len(straight) >= 5:  # Five can be changed e.g. Four Fingers
            Hand.h_Straights.append(straight)

    # Find Flushes
    Hand.h_Flushes.clear()
    for suit in SuitIDs:
        flush = []

        for card in Hand:
            if card.suit == suit or card.suit == 0:
                flush.append(card)

        if len(flush) >= 5: # again, can be changed
            flush.sort()
            Hand.h_Flushes.append(flush)


def ScoreHand(Hand: Deck) -> None:
    bools = [False] * len(HandNames)

    if len(Hand.h_Pairs) > 0:
        has3 = False

        for pair in Hand.h_Pairs:
            if len(pair) == 2:
                bools[HandNames.index("Pair")] = True
            elif len(pair) == 3:
                bools[HandNames.index("Three of a Kind")] = True
            elif len(pair) == 4:
                bools[HandNames.index("Four of a Kind")] = True
            elif len(pair) > 5:
                print("WARNING: UNHANDLED 5+ of a Kind")

            if len(pair) >= 3:
                has3 = True

        if len(Hand.h_Pairs) >= 2:
            bools[HandNames.index("Two Pair")] = True
            if has3:
                bools[HandNames.index("Full House")] = True

    if len(Hand.h_Straights) > 0:
        bools[HandNames.index("Straight")] = True

    if len(Hand.h_Flushes) > 0:
        bools[HandNames.index("Flush")] = True

    for f in Hand.h_Flushes:
        if _getStraightLength(f) >= 5:
            bools[HandNames.index("Straight Flush")] = True
            if f[-1].rank == 14:
                bools[HandNames.index("Royal Flush")] = True

    for i in range(len(bools)):
        if bools[i]:
            Occurances[i] += 1


def RunAnalysis(trials=50000):
    standardDeck = CreateStandardDeck()

    River = Deck()

    for i in range(trials):
        if i % (trials // 10) == 0:
            print(f"{i / trials * 100:.2f}%")

        for card in standardDeck.drawN(7):
            River.add(card)

        AnalyzeHand(River)
        ScoreHand(River)

        standardDeck.combine(River)

    plt.bar(HandNames, [i / trials for i in Occurances])
    plt.title(f"Poker Hand Analysis (Vanilla) - 7 Cards - {trials} Trials")

    print(trials)
    for i in range(len(HandNames)):
        print(HandNames[i], Occurances[i])
        plt.text(HandNames[i], Occurances[i] / trials, f"{Occurances[i] / trials * 100:.5f}%")

    plt.show()
