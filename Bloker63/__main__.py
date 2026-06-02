import sys
import Bloker63

def main() -> None:
    window = Bloker63.Window.Window()

    while window.run:
        window.Update()

    window.Close()


def main2() -> None:
    import Bloker63.Poker as P

    P.RunAnalysis()


if __name__ == '__main__':
    statMode = "-pokerStats" in sys.argv

    statMode = False

    if statMode:
        main2()
    else:
        main()
