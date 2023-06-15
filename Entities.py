import pygame, screeninfo
import time, random
import library


class Player:
    def __init__(self, image):
        random.seed(time.time())
        library.set_screen_size(width, height)
        d = width / 25
        self.player = library.Player(int(d))
        self.image = pygame.transform.scale(image, (d, d))

    def move(self, window, x, y):
        self.player.Move(x, y)
        window.blit(self.image, self.player.get_position())

    def draw(self, window):
        window.blit(self.image, self.player.get_position())


class Ray:
    def __init__(self, type, image, rotation):
        self.enemy = type
        d = width / 30
        image = pygame.transform.scale(image, (d, d))
        self.image = pygame.transform.rotate(image, rotation)
        self.rotation = rotation

    def draw_down(self, window):
        rays = self.enemy.to_draw_rays()
        n = rays[0][0]
        for i in range(1, n):
            pygame.draw.line(window, self.enemy.rays_color(), rays[2*i - 1], rays[2*i], 6)

    def draw_up(self, window):
        position = self.enemy.get_position()
        if self.rotation == 45:
            position[0] -= width / 60
            position[1] -= width / 60
        if self.rotation == 315:
            position[1] -= width / 60
            position[0] += width / 360
        if self.rotation == 135:
            position[0] -= 5 * width / 360
        window.blit(self.image, position)


class ChainBalls:
    def __init__(self, type, image):
        self.enemy = type
        d = width / 18
        self.image = pygame.transform.scale(image, (d, d))

    def draw_down(self, window):
        chain = self.enemy.get_chain()
        pygame.draw.line(window, (150, 210, 210), (chain[0], chain[1]), (chain[2], chain[3]), 6)

    def draw_up(self, window):
        balls = self.enemy.get_balls()
        for i in range(self.enemy.get_n()):
            window.blit(self.image, (balls[i*2], balls[i*2 + 1]))


class Cannon:
    def __init__(self, type, bullet, cannon, b_r, rand):
        self.enemy = type
        d = width / 20
        if rand > 1:
            cannon = pygame.transform.rotate(cannon, 90)
        self.cannon = pygame.transform.scale(cannon, (d, d))
        self.bullet = pygame.transform.scale(bullet, (b_r * 2, b_r * 2))

    def draw_down(self, window):
        n = self.enemy.get_n()
        bullets = self.enemy.get_bullets()
        for i in range(n):
            window.blit(self.bullet, (bullets[i*2], bullets[i*2 + 1]))

    def draw_up(self, window):
        window.blit(self.cannon, self.enemy.get_position())


screen = screeninfo.get_monitors()[0]
width, height = screen.width, screen.height