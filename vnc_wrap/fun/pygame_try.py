import pygame
import random
import time
import numpy as np



######### test why my array must be flipped n rot before bliting #########

data = np.zeros((64,64), np.uint32)
#data = data.astype(np.uint32)
#np.set_printoptions(formatter={'uint32':hex})
#print("aici")
#data[0][0] = pygame.Color(255, 0, 0)
#print(data[0][0])
#print(pygame.Color("#ff0000"))

for x in range(0,64):
    for y in range(0,64):
        if x < 20:
            data[x][y] = pygame.Color("#ff0000")
        elif 20 <= x <= 40:
            data[x][y] = pygame.Color("#00ff00")
        elif x > 40:
            data[x][y] = pygame.Color("#0000FF")
#print(data)

data = data >> 8
screen1 = pygame.surfarray.make_surface(data)

Y = np.arange(16).reshape(4,4)
print(Y)
print(Y[[[0],[3]],[0,3]])

pygame.init()
screen = pygame.display.set_mode((64,64))
clock = pygame.time.Clock()
while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:                
            run = False
            
            break

    
            
    #pygame.transform.flip(screen1, False, True)
    #pygame.transform.rotate(screen1, 90)

    #screen.blit(screen1, (0,0))
    pygame.surfarray.blit_array(screen,data)
    pygame.display.flip()
    clock.tick(20)
pygame.quit() 
######### test flipin and rot ##########
#data = np.random.randint(low=1, high=10, size=(4,4))
#print(data)
#print("\n\n")
#print(np.flipud(data))


######### test shift & masks ##########
#red_shift = 11;    red_max = 0x1F
#green_shift = 5;  green_max = 0x3F
#blue_shift = 0;    blue_max = 0x1F
#data = data.astype(np.uint32)

#red = (np.right_shift(data, red_shift) & red_max) * 255 / red_max 
##red = red.astype(np.uint32)
#red = np.left_shift(data, 16)
#print(red)

#green = (np.right_shift(data, green_shift) & green_max) * 255 / green_max 
##green = green.astype(np.uint32)
#green = np.left_shift(data, 8)
#print(green)    

#blue = (np.right_shift(data, blue_shift) & blue_max) * 255 / blue_max 
#blue = blue.astype(np.uint32)
##blue = np.left_shift(data, 8)
#print(blue)
##data = data.astype(np.uint32)
#data = red + green + blue

#print(data)
