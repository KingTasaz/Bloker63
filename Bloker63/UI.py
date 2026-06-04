import pygame
import random

import Bloker63
from Bloker63.Cards import *
from Bloker63.Jokers import *


_imageCache: dict[str: str] = {}


class Button:
    def __init__(self, x, y, w, h, image: str):
        self.pos = (x, y)

        self.rect = pygame.rect.Rect(x, y, w, h)

        self.image = pygame.image.load(image).convert_alpha()
        self.image = pygame.transform.scale(self.image, (w, h))

        self.collidepoint = self.rect.collidepoint

        self.hover = False
        self.clicked = False

    def center(self, x, y):
        self.pos = (x, y)
        self.rect.move(x - self.rect.w // 2, y - self.rect.h // 2)

    def draw(self, screen: pygame.Surface):
        screen.blit(self.image, self.pos)


class Dot:
    def __init__(self):
        self.x, self.y = 0, 0
        self.r = random.randint(5, 10)

        self.vx, self.vy = 0, 0
        self.ax, self.ay = 0, 0

        self.spawn()

    def spawn(self):
        seed = random.randint(1, 2)

        if seed == 1:
            self.x = -100
            self.vx = random.randint(0, 10)
            self.y = random.randint(-100, int(1080 + 100))
            self.vy = random.randint(-10, 10)
        elif seed == 2:
            self.x = 1920 + 100
            self.vx = random.randint(-10, 0)
            self.y = random.randint(-100, int(1080 + 100))
            self.vy = random.randint(-10, 10)

    def tick(self):
        self.ax += random.randint(-2, 2)
        self.ay += random.randint(-2, 2)

        self.vx += self.ax * 1/5
        self.vy += self.ay * 1/5

        self.x += self.vx * 1/20
        self.y += self.vy * 1/20

        if self.x < -100 or self.x > 2000 or self.y < -100 or self.y > 1500:
            self.spawn()

    def draw(self, screen: pygame.Surface, colour):
        pygame.draw.circle(screen, colour, (self.x, self.y), self.r)


def drawCard(destination: pygame.Surface, card: Card) -> None:
    image = _imageCache.get(card._facePath, None)

    if image is None:
        image = pygame.transform.scale(pygame.image.load(card._facePath), Bloker63.cardSize).convert_alpha()
        _imageCache[card._facePath] = image

    card.x += (card.tx - card.x) / 10
    card.y += (card.ty - card.y) / 10

    destination.blit(image, (card.x, card.y))


def getScreenCenter() -> tuple[float, float]:
    return 1920 // 1.5 // 2 - Bloker63.cardSize[0] // 2, 1080 // 1.5 // 2 - Bloker63.cardSize[1] // 2


__all__ = ["drawCard", "getScreenCenter", "Button", "Dot"]
