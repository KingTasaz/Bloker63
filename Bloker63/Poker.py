import random
import matplotlib.pyplot as plt

import Bloker63
from Bloker63.Cards import *


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


def printHandDetails(Hand: Deck) -> None:
    print(Hand)

    if len(Hand.h_Pairs) > 0:
        has3 = False

        print("- Pairs:")
        for pair in Hand.h_Pairs:
            print(f"   - ({len(pair)}):", *[repr(c) for c in pair])

            if len(pair) >= 3:
                has3 = True

        if len(Hand.h_Pairs) >= 2 and has3:
            print(f"- Contains a Full House")

    if len(Hand.h_Straights) > 0:
        print("- Straights:")
        for pair in Hand.h_Straights:
            print(f"   - ({len(pair)}):", *[repr(c) for c in pair])

    if len(Hand.h_Flushes) > 0:
        print("- Flushes:")
        for pair in Hand.h_Flushes:
            print(f"   - ({len(pair)}):", *[repr(c) for c in pair])

    for f in Hand.h_Flushes:
        if _getStraightLength(f) >= 5:
            print(f"- Contains a Straight Flush")
            if f[-1].rank == 14:
                print(f"- Contains a Royal Flush")


def RunAnalysis(trials=10000):
    standardDeck = CreateStandardDeck()

    River = Deck()

    for card in standardDeck.drawN(7):
        River.add(card)

    AnalyzeHand(River)
    printHandDetails(River)

