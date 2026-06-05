import json

from Bloker63.Cards import *


# -----------------------------------------------
#       ~                              ~
#                   Variables
#       ~                              ~
# -----------------------------------------------
fp = open("Bloker63/Jokers.json")
jokerData = json.load(fp)
fp.close()


# -----------------------------------------------
#       ~                              ~
#                   Classes
#       ~                              ~
# -----------------------------------------------
class Joker(Card):
    def __init__(self, ID: int = 0):
        super().__init__(rank=Ranks.index("Joker"))

        self.jokerID: int = ID

        data = jokerData[ID]
        self.Name = data["name"]
        self._facePath = data["face"]

        # FLAGS
        self._f_actingCard: bool = False

    def __repr__(self):
        if self._f_actingCard:
            return Card.__repr__(self)

        return f"[JOKER] {self.Name}"


# -----------------------------------------------
#       ~                              ~
#                 Functions
#       ~                              ~
# -----------------------------------------------
def CreateJokerDeck() -> Deck:
    d = Deck()

    for i in range(len(jokerData)):
        if i == 0:
            continue

        j = Joker(jokerData[i]["id"])
        d.add(j)

    return d


__all__ = ["Joker", "CreateJokerDeck"]
