import random

from pygments.styles.dracula import yellow

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
SuitIDs = range(1, 5)

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
RankIDs = range(2, 15)


# -----------------------------------------------
#       ~                              ~
#                   Classes
#       ~                              ~
# -----------------------------------------------
class Card:
    def __init__(self, suit: int = 0, rank: int = 0):
        self.suit: int = suit
        self.rank: int = rank

        self._facePath = f"Bloker63/Assets/Cards/{self.suit}_{self.rank}.png"

        self.x, self.y = 1920 // 1.5 // 2 - 150 // 2, -500
        self.tx, self.ty = 0, 0

    def setCard(self, suit: int = -1, rank: int = -1):
        suit = self.suit if suit == -1 else suit
        rank = self.rank if rank == -1 else rank

        self.suit = suit
        self.rank = rank

        self._facePath = f"Bloker63/Assets/Cards/{self.suit}_{self.rank}.png"

    def __int__(self):
        return self.suit * 15 + self.rank

    def __lt__(self, other):
        return int(self) < int(other)

    def __eq__(self, other):
        return int(self) == int(other)

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

        # Hand Details
        self.h_Pairs: list[list[Card]] = []
        self.h_Straights: list[list[Card]] = []
        self.h_Flushes: list[list[Card]] = []

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

    def draw(self) -> Card | None:
        if len(self.Cards) == 0:
            return None

        card = self.Cards.pop(random.randint(0, len(self.Cards) - 1))
        self._drawn.append(card)
        return card

    def drawN(self, n=1) -> Card | None:
        for _ in range(n):
            if len(self.Cards) == 0:
                return None

            card = self.Cards.pop(random.randint(0, len(self.Cards)-1))
            self._drawn.append(card)
            yield card

    def reset(self) -> None:
        self.Cards.extend(self._drawn)
        self._drawn.clear()

    def __iter__(self):
        for c in self.Cards:
            yield c

    def __repr__(self):
        if len(self.Cards) > 10:
            return f"Deck ({len(self.Cards)}/{self.Size}):\n - " + "\n - ".join([repr(c) for c in self.Cards[:10]])

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


__all__ = ["Ranks", "Suits", "Card", "Deck", "CreateStandardDeck",
           "SuitIDs", "RankIDs"]
