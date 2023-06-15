import pygame, screeninfo
import time, random
import Entities
import library


def check_pressed():
    x = 0
    y = 0
    keys = pygame.key.get_pressed()
    if keys[pygame.K_a]:
        x -= 1
    if keys[pygame.K_d]:
        x += 1
    if keys[pygame.K_s]:
        y -= 1
    if keys[pygame.K_w]:
        y += 1
    return x, y


def enemy_action(enemies, frames, player, window):
    collision = False
    if not frames % 180:
        new_wave(enemies, player, min(int(frames / 540) + 3, 10))
    for enemy in enemies:
        if enemy.enemy.action(player.player):
            collision = True
        enemy.draw_down(window)
    for enemy in enemies:
        enemy.draw_up(window)
    print(type(enemies[0].enemy))
    return collision


def new_wave(enemies, player, n):
    enemies.clear()
    used = [0] * 10
    i = 0
    angle = 0
    while i < n:
        rand = random.randint(0, 9)
        if used[rand]:
            continue
        used[rand] = 1
        if rand == 0:
            rand = random.randint(0, 1)
            enemies.append(Entities.Ray(library.HorizontalRay(player.player, int(width / 60), rand),
                                        PrismImg, 180 + rand * 180))
        elif rand == 1:
            rand = random.randint(0, 1)
            enemies.append(Entities.Ray(library.VerticalRay(player.player, int(width / 60), rand),
                                        PrismImg, 270 + rand * 180))
        elif rand == 2:
            rand = random.randint(0, 1)
            if rand:
                angle = 135
            else:
                angle = 225
            enemies.append(Entities.Ray(library.DiagonalRay(player.player, int(width / 60), rand), PrismImg, angle))
        elif rand == 3:
            rand = random.randint(2, 3)
            if rand == 2:
                angle = 315
            else:
                angle = 45
            enemies.append(Entities.Ray(library.DiagonalRay(player.player, int(width / 60), rand), PrismImg, angle))
        elif rand == 4:
            enemies.append(Entities.ChainBalls(library.OneBall(int(width / 6), int(width / 36)), BallImg))
        elif rand == 5:
            enemies.append(Entities.ChainBalls(library.TwoBalls(int(width / 6), int(width / 36)), BallImg))
        elif rand == 6:
            rand = random.randint(0, 1)
            enemies.append(Entities.Cannon(library.SimpleCannon(rand, int(width / 40),
                                                                int(width / 60), int(width / 120)),
                                           BulletImg, CannonImg, int(width / 60), rand))
        elif rand == 7:
            rand = random.randint(2, 3)
            enemies.append(Entities.Cannon(library.SimpleCannon(rand, int(width / 40),
                                                                int(width / 60), int(width / 120)),
                                           BulletImg, CannonImg, int(width / 60), rand))
        elif rand == 8:
            rand = random.randint(0, 1)
            enemies.append(Entities.Cannon(library.TripleCannon(rand, int(width / 40),
                                                                int(width / 60), int(width / 120)),
                                           BulletImg, CannonImg, width / 60, rand))
        elif rand == 9:
            rand = random.randint(2, 3)
            enemies.append(Entities.Cannon(library.TripleCannon(rand, int(width / 40),
                                                                int(width / 60), int(width / 120)),
                                           BulletImg, CannonImg, width / 60, rand))
        i += 1


BallImg = pygame.image.load('Images/Ball.png')
PrismImg = pygame.image.load('Images/Prism.png')
BulletImg = pygame.image.load('Images/Bullet.png')
CannonImg = pygame.image.load('Images/Cannon.png')
screen = screeninfo.get_monitors()[0]
width, height = screen.width, screen.height