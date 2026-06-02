import random


# -----------------------------------------------
#       ~                              ~
#               Suits and Ranks
#       ~                              ~
# -----------------------------------------------
Suits = [
    "WILD",
    "Spades",
    "Hearts",
    "Clubs",
    "Diamonds"
]

Ranks = [
    "WILD",
    "Joker",
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
    "Queen",
    "King",
    "Ace"
]


# -----------------------------------------------
#       ~                              ~
#                   Classes
#       ~                              ~
# -----------------------------------------------
class Card:
    def __init__(self, suit: int = 0, rank: int = 0):
        self.suit: int = suit
        self.rank: int = rank

    def __lt__(self, other):
        return self.suit * 15 + self.rank < other.suit * 15 + other.rank

    def __repr__(self):
        if self.rank == self.suit == 0:
            return "Wild"

        if self.rank == 0:
            return f"Wild {Suits[self.suit]}".removesuffix("s")

        if self.suit == 0:
            return f"Wild {Ranks[self.rank]}"

        return f"{Ranks[self.rank]} of {Suits[self.suit]}"


class Deck:
    def __init__(self):
        self.Size = 0
        self.Cards: list[Card] = []
        self._drawn: list[Card] = []

    def add(self, card: Card, addToDiscardPile=False) -> None:
        if addToDiscardPile:
            self._drawn.append(card)
        else:
            self.Cards.append(card)

        self.Size += 1

    def clear(self):
        self.Cards.clear()
        self._drawn.clear()
        self.Size = 0

    def combine(self, other) -> None:
        """Put all cards from 'other' Deck into this deck"""

        self.Size += other.Size
        self.Cards.extend(other.Cards)
        self.Cards.extend(other._drawn)

        other.clear()

    def shuffle(self) -> None:
        random.shuffle(self.Cards)

    def sort(self, reverse=False) -> None:
        self.Cards.sort(reverse=reverse)

    def draw(self) -> Card:
        card = self.Cards.pop(random.randint(0, len(self.Cards)-1))
        self._drawn.append(card)
        return card

    def reset(self) -> None:
        self.Cards.extend(self._drawn)
        self._drawn.clear()

    def __repr__(self):
        if len(self.Cards) > 5:
            return f"Deck ({len(self.Cards)}/{self.Size}):\n - " + "\n - ".join([repr(c) for c in self.Cards[:5]])

        return f"Deck ({len(self.Cards)}/{self.Size}):\n - " + "\n - ".join([repr(c) for c in self.Cards])


# -----------------------------------------------
#       ~                              ~
#                 Functions
#       ~                              ~
# -----------------------------------------------
def CreateStandardDeck() -> Deck:
    d = Deck()

    for suit in range(1, 5):
        for rank in range(2, 15):
            c = Card(suit, rank)
            d.add(c)

    return d


__all__ = ["Ranks", "Suits", "Card", "Deck", "CreateStandardDeck"]
