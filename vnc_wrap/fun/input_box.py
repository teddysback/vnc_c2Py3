import pygame, pygame.font, pygame.event, pygame.draw, string
from pygame.locals import *
import numbers

MAX_PASSWORD_LENGTH = 8

IB_RETURN    = 0xC000
IB_BACKSPACE = 0xC001
IB_ESCAPE    = 0xC002

IB_DELETE    = 0XC03
IB_LARROW    = 0XC04
IB_RARROW    = 0XC05


def get_key():
    ''' Return unicode char and handle backspace/return keys
    '''
    while 1:
        event = pygame.event.poll()
        if event.type == KEYDOWN:
            if event.key == K_RETURN:
                return IB_RETURN
            if event.key == K_BACKSPACE:
                return IB_BACKSPACE
            if event.key == K_ESCAPE:
                return IB_ESCAPE
            
            if event.key == K_DELETE:
                return IB_DELETE
            if event.key == K_LEFT:
                return IB_LARROW
            if event.key == K_RIGHT:
                return IB_RARROW
            
            if event.key < 32 or 127 <= event.key:
                return event.key
            return event.unicode
        else:
            pass

def display_box(screen, message):
    '''Print a message in a box in the middle of the screen
    '''
    default_font = pygame.font.get_default_font()
    font_object = pygame.font.Font(default_font, 15) 
    
    pygame.draw.rect(screen, (255,0,0),
                    ((screen.get_width() / 2) - 100,
                    (screen.get_height() / 2) - 10,
                    200,20), 0)
    pygame.draw.rect(screen, (255,255,255),
                    ((screen.get_width() / 2) - 102,
                    (screen.get_height() / 2) - 12,
                    204,24), 1)
    if len(message) != 0:
        screen.blit(font_object.render(message, 1, (255,255,255)),
                ((screen.get_width() / 2) - 100, (screen.get_height() / 2) - 10))
    pygame.display.flip()

def ask(screen, question):
    '''ask(screen, question) -> answer
    '''
    pygame.font.init()
    current_string = []
    display_string = ['_',]
    index = 0

    display_box(screen, question + ": " + "".join(current_string))
    while 1:
        in_key = get_key()
        
        #
        #   [!] index max min 0, max max len-1
        # 
      
        if in_key == IB_BACKSPACE and index > 0:
            current_string = current_string[0:-1]
            display_string = display_string[:index-1] + display_string[index:]
            index -= 1

        elif in_key == IB_DELETE and index:
            print(current_string)
            print(index)

            #del current_string[index]
            current_string = current_string[:index-1] + current_string[index+1:]
            display_string = display_string[:index-1] + display_string[index+1:]
            #del display_string[index]
            index -= 1
        elif in_key == IB_LARROW and index > 0:
            index -= 1
            
            a = display_string[index-1]
            display_string[index-1] = display_string[index]
            display_string[index] = display_string[index-1]
            

        elif in_key == IB_RARROW and index < len(current_string):
            index += 1
        elif in_key == IB_RETURN:
            break
        elif in_key == IB_ESCAPE:
            current_string = []
            display_string = []
            break        
        elif isinstance(in_key, int):
            print('IIII {}'.format(in_key))

        elif len(display_string) <= MAX_PASSWORD_LENGTH:
            print(in_key)
            current_string.append(in_key)
            display_string.insert(index, in_key)  #('*')
            index += 1

        display_box(screen, question + ": " + "".join(display_string) + "i:" + str(index))
    pygame.quit()
    return "".join(current_string)


def main():
    screen = pygame.display.set_mode((240,100))
    print('This is what you entered: [{}]'.format(ask(screen, "PASSWORD")))

if __name__ == '__main__': 
    main()

############
#!/usr/bin/python

#import pygame

#pygame.init()
#screen_size=(800,60)
#disp=pygame.display.set_mode(screen_size, pygame.DOUBLEBUF)
#msg=u""
#clock=pygame.time.Clock()
#default_font=pygame.font.get_default_font()
#font=pygame.font.SysFont(default_font,16)

#disp.fill((240,240,240,255))
#pygame.display.flip()
#while(not pygame.event.pump()):
#    for event in pygame.event.get():
#        print (event)
#        if event.type == pygame.QUIT:
#            pygame.quit()
#            break
#        if event.type == pygame.KEYDOWN:
#            msg+=event.unicode
#            disp.fill((240,240,240,255))
#            disp.blit(font.render(msg,True,(30,30,30,255)),(0,0))
#            pygame.display.flip()
#    clock.tick(24)