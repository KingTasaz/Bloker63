import pygame
import math
import random

import Bloker63

_Init = False


WHITE = (255, 255, 255)
PURPLE = (121, 115, 140)
BLACK = (0, 0, 0)

o = 1920 // 2 - Bloker63.cardSize[0] // 2, -710


class Window:
    width = 1920
    height = 1080

    def __init__(self):
        global _Init

        if _Init:
            raise RuntimeError("Only 1 window may be created.")

        _Init = True

        # === PYGAME ===
        pygame.display.init()
        pygame.mixer.init()

        self.window = pygame.display.set_mode((self.width, self.height), pygame.DOUBLEBUF, 32)
        self.clock = pygame.time.Clock()

        pygame.display.set_caption("Bloker63")

        # === Audio ===
        pygame.mixer_music.load("Bloker63/Assets/Audio/balatromain.mp3")
        pygame.mixer_music.set_volume(0.5)
        pygame.mixer_music.play(fade_ms=5000, loops=-1)

        pygame.mixer.Sound("Bloker63/Assets/Audio/titleA.mp3").play()
        pygame.mixer.Sound("Bloker63/Assets/Audio/intro.ogg").play(fade_ms=2000)

        self.audio_button_hover = pygame.mixer.Sound("Bloker63/Assets/Audio/hover.mp3")
        self.audio_button_click = pygame.mixer.Sound("Bloker63/Assets/Audio/click.ogg")
        self.audio_start = pygame.mixer.Sound("Bloker63/Assets/Audio/start.ogg")

        self.audio_card1 = pygame.mixer.Sound("Bloker63/Assets/Audio/card1.ogg")
        self.audio_card2 = pygame.mixer.Sound("Bloker63/Assets/Audio/card2.ogg")
        self.audio_cardFail = pygame.mixer.Sound("Bloker63/Assets/Audio/Fail.mp3")
        self.audio_cardClear = pygame.mixer.Sound("Bloker63/Assets/Audio/cards_clear.ogg")

        # === Buttons ===
        self.button_Play = Bloker63.UI.Button(1920 // 2 - 230 * 1.5 // 2, 700, 230 * 1.5, 100 * 1.5,
                                              "Bloker63/Assets/Play.png")

        self.button_Draw = Bloker63.UI.Button(1920 // 2 - 230 * 1.5 // 2 - 170 * 1.5, 800, 230 * 1.5, 100 * 1.5,
                                              "Bloker63/Assets/Draw.png")

        self.button_Clear = Bloker63.UI.Button(1920 // 2 - 230 * 1.5 // 2 + 170 * 1.5, 800, 230 * 1.5, 100 * 1.5,
                                              "Bloker63/Assets/Clear.png")

        # === DATA ===
        self.image_background = pygame.transform.scale(
            pygame.image.load("Bloker63/Assets/Background.png").convert_alpha(), (self.width, self.height)
        )
        self.image_title = pygame.transform.scale(
            pygame.image.load("Bloker63/Assets/Title.png").convert_alpha(), (self.width, self.height)
        )

        self._dots: list[Bloker63.UI.Dot] = [Bloker63.UI.Dot() for _ in range(50)]

        self.black = pygame.Surface((self.width, self.height))
        self.black.fill(BLACK)

        self.Jokers = Bloker63.Jokers.CreateJokerDeck()
        self.Hand = Bloker63.Cards.Deck()

        # === Variables ===
        self.run = True

        self._mainMenu = True
        self._menuFadeTimer = 0
        self._delta = 0
        self._size = 0

    def _moveCardsInHand(self):
        if self.Hand.Size < 4:
            Bloker63.cardSize = Bloker63.cardSize0
        elif self.Hand.Size < 6:
            Bloker63.cardSize = Bloker63.cardSize1
        else:
            Bloker63.cardSize = Bloker63.cardSize2

        cx, cy = self.width // 2, self.height // 2
        left = cx - Bloker63.cardSize[0] // 2

        if self.Hand.Size >= 8:
            cy -= self.height // 4

        cards = min(self.Hand.Size, 7)

        left -= (Bloker63.cardSize[0] // 2 + 50 // 2) * (cards - 1)

        for i in range(self.Hand.Size):
            j = i

            if j == 7:
                cy += self.height // 4 + 70
            j %= 7

            self.Hand.Cards[i].tx = left + j * (Bloker63.cardSize[0] + 50)
            self.Hand.Cards[i].ty = cy - Bloker63.cardSize[1] // 1.5

    def _DrawToHand(self, amount=1):
        if self.Hand.Size >= Bloker63.maxDraw:
            self.audio_cardFail.play()
            return

        for _ in range(amount):
            card = self.Jokers.draw()

            if card is None:
                self.audio_cardFail.play()
                return

            if random.randint(0, 1) == 1:
                self.audio_card1.play()
            else:
                self.audio_card2.play()

            self.Hand.add(card)
            self._moveCardsInHand()

    def _ClearHand(self):
        for card in self.Hand.Cards:
            card.tx, card.ty = o
        self.Jokers.combine(self.Hand)
        self.Jokers.shuffle()
        self.audio_cardClear.play()

    def _removeJoker(self, slot: int):
        if slot >= self.Hand.Size:
            return

        card = self.Hand.Cards.pop(slot)
        self.Hand.Size -= 1

        card.tx, card.ty = o
        self.Jokers.add(card)
        self.Jokers.shuffle()
        self.audio_cardClear.play()

        self._moveCardsInHand()

    def _drawJokerByID(self, id: int):
        for i, joker in enumerate(self.Jokers.Cards):
            if joker.jokerID == id:
                break
        else:
            return

        self.Jokers.Cards.pop(i)
        self.Jokers.Size -= 1

        self.Hand.add(joker)
        self.audio_card1.play()
        self._moveCardsInHand()

    def _pollEvents(self):
        m = pygame.mouse.get_pos()

        for event in pygame.event.get():
            match event.type:
                case pygame.QUIT:
                    self.run = False
                case pygame.KEYDOWN:
                    match event.key:
                        case pygame.K_LEFT:
                            self._DrawToHand()
                        case pygame.K_a:
                            self._DrawToHand()
                        case pygame.K_SPACE:
                            self._DrawToHand()
                        case pygame.K_RETURN:
                            self._DrawToHand()
                        case pygame.K_RIGHT:
                            self._ClearHand()
                        case pygame.K_d:
                            self._ClearHand()
                        case pygame.K_BACKSPACE:
                            self._ClearHand()
                        case pygame.K_x:
                            self._DrawToHand(999)
                        case pygame.K_1:
                            self._removeJoker(0)
                        case pygame.K_2:
                            self._removeJoker(1)
                        case pygame.K_3:
                            self._removeJoker(2)
                        case pygame.K_4:
                            self._removeJoker(3)
                        case pygame.K_5:
                            self._removeJoker(4)
                        case pygame.K_6:
                            self._removeJoker(5)
                        case pygame.K_7:
                            self._removeJoker(6)
                        case pygame.K_8:
                            self._removeJoker(7)
                        case pygame.K_0:
                            self._removeJoker(-1)
                        case pygame.K_j:
                            self._drawJokerByID(62)
                        case pygame.K_b:
                            self._drawJokerByID(20)

                case pygame.MOUSEBUTTONDOWN:
                    if event.button == pygame.BUTTON_LEFT:
                        if self.button_Play.collidepoint(m) and not self.button_Play.clicked:
                            self.audio_button_click.play()
                            self.button_Play.clicked = True

                            if self._menuFadeTimer < 5000:
                                self._menuFadeTimer *= -1
                            else:
                                self._menuFadeTimer = -1000
                        elif self.button_Draw.collidepoint(m):
                            self._DrawToHand()
                        elif self.button_Clear.collidepoint(m):
                            self._ClearHand()

    def _draw(self):
        m = pygame.mouse.get_pos()

        self.window.blit(self.image_background, (0, 0))

        for dot in self._dots:
            dot.tick()
            dot.draw(self.window, PURPLE)

        if self.button_Play.clicked and self._menuFadeTimer > 0:
            self._mainMenu = False

        if self._mainMenu:      # Draw Main Menu
            self.window.blit(self.image_title, (0, 25*math.sin(self._delta / 1000)))
            self.button_Play.draw(self.window)
        else:                   # Draw Main Game
            self.button_Draw.draw(self.window)
            self.button_Clear.draw(self.window)

            for card in self.Jokers.Cards:
                if card.y > o[1]:
                    Bloker63.UI.drawCard(self.window, card)
            for card in self.Hand.Cards:
                Bloker63.UI.drawCard(self.window, card)

        if self._menuFadeTimer < 5000:
            self.black.set_alpha(255 * 2.178**-(self._menuFadeTimer / 1000)**2)
            self.window.blit(self.black, (0, 0))

    def Update(self) -> None:
        self._pollEvents()
        self._draw()

        pygame.display.flip()
        delta = self.clock.tick(60)

        self._delta += delta
        self._menuFadeTimer += delta

    def Close(self):
        pygame.quit()


__all__ = ["Window"]
