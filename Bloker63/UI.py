import pygame

from Bloker63.Cards import *
from Bloker63.Jokers import *


_imageCache: dict[str: str] = {}

cardSize = (300 // 2, 417 // 2)


def drawCard(destination: pygame.Surface, card: Card, pos: tuple[int | float, int | float]) -> None:
    image = _imageCache.get(card._facePath, None)

    if image is None:
        image = pygame.transform.scale(pygame.image.load(card._facePath), cardSize)
        _imageCache[card._facePath] = image

    destination.blit(image, pos)


def getScreenCenter() -> tuple[float, float]:
    return 1920 // 1.5 // 2 - cardSize[0] // 2, 1080 // 1.5 // 2 - cardSize[1] // 2


__all__ = ["drawCard", "getScreenCenter"]
