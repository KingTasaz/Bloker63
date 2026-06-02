import pygame

_Init = False


WHITE = (255, 255, 255)
PURPLE = (121, 115, 140)
BLACK = (0, 0, 0)


class Window:
    width = 1920 // 1.5
    height = 1080 // 1.5

    def __init__(self):
        global _Init

        if _Init:
            raise RuntimeError("Only 1 window may be created.")

        _Init = True

        # === PYGAME ===
        pygame.display.init()
        pygame.mixer.init()

        self.window = pygame.display.set_mode((self.width, self.height))
        self.clock = pygame.time.Clock()

        pygame.display.set_caption("Bloker63")

        # === DATA ===
        self.image_background = pygame.transform.scale(
            pygame.image.load("Bloker63/Assets/Background.png"), (self.width, self.height)
        )

        # === Variables ===
        self.run = True

    def _pollEvents(self):
        for event in pygame.event.get():
            match event.type:
                case pygame.QUIT:
                    self.run = False

    def _draw(self):
        self.window.blit(self.image_background, (0, 0))

    def Update(self) -> None:
        self._pollEvents()
        self._draw()

        pygame.display.flip()
        self.clock.tick(60)

    def Close(self):
        pygame.quit()


__all__ = ["Window"]
