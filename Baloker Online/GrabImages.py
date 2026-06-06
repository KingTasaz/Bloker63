import requests

Suit = 3
SuitName = "Diamonds"

Ranks = {
    0: "2",
    1: "3",
    2: "4",
    3: "5",
    4: "6",
    5: "7",
    6: "8",
    7: "9",
    8: "10",
    9: "Jack",
    10: "King",
    11: "Queen",
    12: "Ace"
}

for rank in Ranks:
    name = Ranks[rank]

    url = f"https://balatrowiki.org/images/thumb/{name}_of_{SuitName}.png/361px-{name}_of_{SuitName}.png?d0e33?download"
    filename = f"assets/Cards/{rank}_{Suit}.png"

    print(f"Downloading {filename}...")

    response = requests.get(url)
    response.raise_for_status()  # raises error if download fails

    with open(filename, "wb") as f:
        f.write(response.content)

print("Done.")