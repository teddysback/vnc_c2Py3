import pygame
import random
import time
import numpy as np

run = True
scenario = 1

BLACK = pygame.Color(  0,  0,  0)
WHITE = pygame.Color(255,255,255)

min = 255**2; max = 255**3
width = 800; height = 600
anonymous_dim = (int(width/2), int(width/2))

pygame.init()
screen = pygame.display.set_mode((width,height))
clock = pygame.time.Clock()


anonymous = pygame.image.load("anonymous.png").convert()
anonymous = pygame.transform.scale(anonymous, anonymous_dim)
anonymous.set_colorkey(WHITE)
#anonymous = pygame.transform.flip(anonymous, True, True)

screen_array_1 = np.random.uniform(low=min, high=max, size=(width,height))
screen_array_2 = np.random.uniform(low=min, high=max, size=(width,height))
screen_array_3 = np.random.uniform(low=min, high=max, size=(width,height))

screen1 = pygame.surfarray.make_surface(screen_array_1)
screen2 = pygame.surfarray.make_surface(screen_array_2)
screen3 = pygame.surfarray.make_surface(screen_array_3)

clr = 1000000000
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            print('pula')
            run = False
            
            break

    if scenario == 1:
        screen.blit(screen1, (0,0))
        scenario = scenario << 1
        #print(scenario)
        #pygame.surfarray.blit_array(screen, screen_array_1)
        #clr = pygame.colordict.THECOLORS['aquamarine1']
    elif scenario == 2:
        screen.blit(screen2, (0,0))
        scenario = scenario << 1
    else:
        screen.blit(screen3, (0,0))
        scenario = 1

    
    screen.blit(anonymous, (int(width/2 - width/4), int(height/2 - height/4)))
    

    pygame.display.flip()
    clock.tick(20)
pygame.quit()