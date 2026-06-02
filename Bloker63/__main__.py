import Bloker63

def main() -> None:
    window = Bloker63.Window.Window()

    while window.run:
        window.Update()

    window.Close()


if __name__ == '__main__':
    main()
