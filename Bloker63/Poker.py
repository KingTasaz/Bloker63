import random
import matplotlib.pyplot as plt

import Bloker63
from Bloker63.Cards import *


def containsPair(hand: Deck) -> bool:
    for cardA in hand.Cards:
        for cardB in hand.Cards:
            if cardA.rank == cardB.rank and cardA is not cardB:
                return True
    return False

def containsTwoPair(hand: Deck) -> bool:
    pair1A = None
    pair1B = None

    for cardA in hand.Cards:
        for cardB in hand.Cards:
            if cardA.rank == cardB.rank and cardA is not cardB:
                pair1A = cardA
                pair1B = cardB
                break

    if pair1A is None:
        return False

    for cardA in hand.Cards:
        for cardB in hand.Cards:
            if cardA.rank == cardB.rank and cardA is not cardB:
                if cardA is not pair1A and cardA is not pair1B and cardB is not pair1A and cardB is not pair1B:
                    return True

    return False


def containsThreePair(hand: Deck) -> bool:
    for cardA in hand.Cards:
        for cardB in hand.Cards:
            for cardC in hand.Cards:
                if cardA is cardB or cardA is cardC or cardB is cardC:
                    continue

                if cardA.rank == cardB.rank == cardC.rank:
                    return True
    return False


def containsStraight(hand: Deck) -> bool:
    for card0 in hand.Cards:
        startRank = card0.rank

        good = True

        for i in range(startRank+1, startRank+5):
            for card1 in hand.Cards:
                if card1.rank == i:
                    break
            else:
                good = False
                break

        if good:
            return True

    return False


def containsFlush(hand: Deck) -> bool:
    for suit in range(1, 5):
        count = 0

        for card in hand.Cards:
            if card.suit == suit:
                count += 1

        if count >= 5:
            return True

    return False


def containsFullHouse(hand: Deck) -> bool:
    return containsThreePair(hand) and containsTwoPair(hand)


def containsFourPair(hand: Deck) -> bool:
    for cardA in hand.Cards:
        for cardB in hand.Cards:
            for cardC in hand.Cards:
                for cardD in hand.Cards:
                    if cardA is cardB or cardA is cardC or cardA is cardD:
                        continue
                    if cardB is cardC or cardB is cardD or cardC is cardD:
                        continue

                    if cardA.rank == cardB.rank == cardC.rank == cardD.rank:
                        return True

    return False


def containsStraightFlush(hand: Deck) -> bool:
    for card0 in hand.Cards:

        cards = [card0]
        startRank = card0.rank
        good = True

        for i in range(startRank+1, startRank+5):
            for card1 in hand.Cards:
                if card1.rank == i:
                    cards.append(card1)
                    break
            else:
                good = False
                break

        if good:
            d = Deck()
            d.Cards = cards

            return containsFlush(d)

    return False


def containsRoyalFlush(hand: Deck) -> bool:
    for card0 in hand.Cards:

        if card0.rank != 10:
            continue

        cards = [card0]
        startRank = card0.rank
        good = True

        for i in range(startRank+1, startRank+5):
            for card1 in hand.Cards:
                if card1.rank == i:
                    cards.append(card1)
                    break
            else:
                good = False
                break

        if good:
            d = Deck()
            d.Cards = cards

            return containsFlush(d)

    return False


def RunAnalysis(trials=10000):
    standardDeck = CreateStandardDeck()

    Hands = [
        "High Card",
        "Pair",
        "Two Pair",
        "Three Pair",
        "Straight",
        "Flush",
        "Full House",
        "Four Pair",
        "Straight Flush",
        "Royal Flush"
    ]

    HandOutcomes = [0] * len(Hands)

    River = Deck()

    for i in range(trials):
        if i % 500 == 0:
            print(f"{round(i/trials*100)}%")

        for card in standardDeck.drawN(7):
            River.add(card)

        if containsRoyalFlush(River):
            HandOutcomes[9] += 1
        elif containsStraightFlush(River):
            HandOutcomes[8] += 1
        elif containsFourPair(River):
            HandOutcomes[7] += 1
        elif containsFullHouse(River):
            HandOutcomes[6] += 1
        elif containsFlush(River):
            HandOutcomes[5] += 1
        elif containsStraight(River):
            HandOutcomes[4] += 1
        elif containsThreePair(River):
            HandOutcomes[3] += 1
        elif containsTwoPair(River):
            HandOutcomes[2] += 1
        elif containsPair(River):
            HandOutcomes[1] += 1
        else:
            HandOutcomes[0] += 1

        standardDeck.combine(River)

    print(f"Finished Analysing {trials} Hands:")
    for i in range(len(Hands)):
        print(f" - {Hands[i]}: {HandOutcomes[i]} ({HandOutcomes[i] / trials * 100:.5f}%)")


    plt.bar(Hands, HandOutcomes)
    plt.show()
