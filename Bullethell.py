import pygame, screeninfo
import time
import Play_state, Entities


def print_text(window, text, y):
    text = font.render(text, True, (0, 0, 0))
    text_centre = text.get_rect()
    text_centre.center = (width / 2, y)
    window.blit(text, text_centre)

def new_highscore(score):
    file = open('Highscore.txt', 'r')
    lines = []
    lines.append(file.readline())
    not_written = True
    if lines[0] and int(lines[0]) >= score:
        text = 'Your score'
    else:
        text = 'New record!'
        not_written = False
        lines.insert(0, str(score) + '\n')
    for line in file:
        if not_written and score > int(line):
            lines.append(str(score) + '\n')
            not_written = False
        lines.append(line)
    if not_written and len(lines) < 10:
        lines.append(str(score) + '\n')
    file.close()
    file = open('Highscore.txt', 'w')
    for i in range(0, min(len(lines), 10)):
        file.write(lines[i])
    file.close()
    return text


file = open('Highscore.txt', 'a')
file.close()
pygame.init()
screen = screeninfo.get_monitors()[0]
width, height = screen.width, screen.height
window = pygame.display.set_mode((width, height))
icon = pygame.image.load('Images/Icon.png')
pygame.display.set_icon(icon)
pygame.display.set_caption('Bullethell')

background = pygame.image.load('Images/Background.png')
background = pygame.transform.scale(background, (width, width))
font = pygame.font.Font(None, 72)
button_color = (150, 255, 255)
playerImg = pygame.image.load('Images/Player.png')

state = 1
frames = 0
score = 0
text = ''
player = Entities.Player(playerImg)
enemies = []
x = 0
y = 0
clock = pygame.time.Clock()
while state:
    clock.tick(60)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break

    if state == 1:
        window.blit(background, (0, 0))
        pygame.draw.rect(window, button_color,
                         pygame.Rect(width / 2 - width / 12, height / 2 - height / 7, width / 6, height / 10))
        pygame.draw.rect(window, button_color,
                         pygame.Rect(width / 2 - width / 12, height / 2, width / 6, height / 10))
        pygame.draw.rect(window, button_color,
                         pygame.Rect(width / 2 - width / 12, height / 2 + height / 7, width / 6, height / 10))
        print_text(window, 'Play', height / 2 - height / 7 + height / 20)
        print_text(window, 'Highscore', height / 2 + height / 20)
        print_text(window, 'Quit', height / 2 + height / 7 + height / 20)
        mouse_pressed = pygame.mouse.get_pressed()
        if frames:
            frames -= 1
        elif mouse_pressed[0]:
            mouse_pos = pygame.mouse.get_pos()
            if width / 2 - width / 12 <= mouse_pos[0] <= width / 2 + width / 12:
                if height / 2 - height / 7 <= mouse_pos[1] <= height / 2 - height / 7 + height / 10:
                    player.player.set_to_start()
                    state = 2
                elif height / 2 <= mouse_pos[1] <= height / 2 + height / 10:
                    state = 4
                elif height / 2 + height / 7 <= mouse_pos[1] <= height / 2 + height / 7 + height / 10:
                    state = 0

    if state == 2:
        window.blit(background, (0, 0))
        window.blit(font.render(str(score), True, (0, 0, 0)), (0, 0))
        x, y = Play_state.check_pressed()
        player.move(window, x, y)
        if Play_state.enemy_action(enemies, frames, player, window):
            frames = 0
            state = 3
            text = new_highscore(score)
        if frames % 10 == 0:
            score += 10
        frames += 1

    if state == 3:
        window.blit(background, (0, 0))
        player.draw(window)
        for enemy in enemies:
            enemy.draw_down(window)
        for enemy in enemies:
            enemy.draw_up(window)
        pygame.draw.rect(window, (130, 130, 130),
                         pygame.Rect(width / 2 - width / 8, height/4, width / 4, height / 2))
        pygame.draw.rect(window, button_color,
                         pygame.Rect(width / 2 - width / 12, height/2 + height/9, width / 6, height / 10))
        print_text(window, text, height / 4 + height / 10)
        print_text(window, str(score), height / 2)
        print_text(window, 'Ok', height / 2 + height / 9 + height / 20)
        mouse_pressed = pygame.mouse.get_pressed()
        if mouse_pressed[0]:
            mouse_pos = pygame.mouse.get_pos()
            if width / 2 - width / 12 <= mouse_pos[0] <= width / 2 + width / 12 and \
                    height/2 + height/9 <= mouse_pos[1] <= height/2 + height/9 + height / 10:
                        state = 1
                        frames = 10
                        score = 0

    if state == 4:
        window.blit(background, (0, 0))
        leaderboard = open('Highscore.txt', 'r')
        y = height/30
        i = 1
        for line in leaderboard:
            window.blit(font.render(str(i) + '. ' + line.strip(), True, (0, 0, 0)),
                        (width/30, y))
            y += height/12
            i += 1
        pygame.draw.rect(window, button_color,
                         pygame.Rect(width / 2 - width / 12, height - height / 8, width / 6, height / 10))
        print_text(window, 'Main screen', height - height / 8 + height / 20)
        mouse_pressed = pygame.mouse.get_pressed()
        if mouse_pressed[0]:
            mouse_pos = pygame.mouse.get_pos()
            if width / 2 - width / 12 <= mouse_pos[0] <= width / 2 + width / 12 and \
                    height - height / 8 <= mouse_pos[1] <= height - height / 8 + height / 10:
                        state = 1
        leaderboard.close()

    pygame.display.update()

pygame.quit()