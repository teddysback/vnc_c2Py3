import vncmodule#amt as vncmodule
import time
import pygame
import numpy as np
import getpass

from pixelbuffer import PixelBuffer
from multiprocessing import Process
import input_box

done = False
f = None
#
# Default callbacks
#
def server_init_cb(width, height, desktop_name, red_max, green_max, blue_max, red_shift, green_shift, blue_shift,big_endian):
    global f
    #print('sleep 5s')
    #time.sleep(5)
    print("[SrvInit_CB] conn established succesfully")    
    f = PixelBuffer((width,height), desktop_name, (red_max,green_max,blue_max), (red_shift,green_shift,blue_shift),big_endian)
    print('sleep 5s')
    #time.sleep(5)
    
    
    #input()
    

def sts_cb(code, status, more_status):
    global done
    print("[Sts_CB] \n\terror code:%d \n\t%s \n\t%s\n"%(code, status, more_status))

   

    done = True
    
    
def cred_cb():
    print("[Cred_CB]")
    #pass2 = input("Please enter password:")
    #pass2 += '\0'
        
    ret = (None, 'P@sswo2;')
    #ret = (None, pass2)   
    #screen = pygame.display.set_mode((240,100))
    #pass2 = input_box.ask(screen, "PASSWORD")
    #print('am scris.. {}'.format(pass2))
    #ret = (None, ret)   
    return ret
# <!> Default callbacks

###
def printy():
    global f
    
    # [!] Aici ar trebui sa facem o verificare 
    #     daca s-a incarcat buffer ul dupa init
    while f == None:
        time.sleep(1)   

    pygame.init()
    #pygame.font.Font("FreeSansBold.ttf", 14).render(u'\u2654')
    #print('w{} h{}'.format(f.width,f.height))
    screen = pygame.display.set_mode((f.width,f.height))#, pygame.OPENGL)
    clock = pygame.time.Clock()
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:                
                run = False
            
                break
        if f.was_resized:
            screen = pygame.display.set_mode((f.width,f.height))
            f.was_resized = False
        #screen1 = pygame.surfarray.make_surface(data)
            
        #pygame.transform.flip(screen1, False, True)
        #pygame.transform.rotate(screen1, 90)

        #screen.blit(screen1, (0,0))
        try:
            pygame.surfarray.blit_array(screen, f.buffer)
        except:
            pass
        pygame.display.flip()
      
        clock.tick(20)  
       
    time.sleep(0.1)
    pygame.quit()

#
# Desktop update callbacks
#
def img_rect_cb(top_left_xy, bot_right_xy, pixel_data, data_len):
    global f

    if not f.update_rectangle(top_left_xy, bot_right_xy, pixel_data, data_len):
        print("eroare update_buffer")
    

def fill_rect_cb(top_left_xy, bot_right_xy, pixel_data):
    global f    
 
    if not f.fill_rectangle(top_left_xy, bot_right_xy, pixel_data):
        print("eroare update_buffer2")


def copy_rect_cb(destination, source):
    print("[copy rect cb]")
    if not f.copy_rectangle(destination, source):
        print("eroare update_buffer3")

#
# Other callbacks
#
def dsk_res_cb(width_height):
    global f
    
    f.resize_screen(width_height)




if __name__ == '__main__':
    #if vncmodule.init_sdk():
    #    print("eroare")
    if vncmodule.init_cb():
        print("eroare2")

    cb_dict = {
        'ServerInitCallback':server_init_cb, 
        'StatusCallback':sts_cb,
        'CredentialsCallback':cred_cb,
        'ImageRectangleCallback':img_rect_cb,
        'CopyRectangleCallback':copy_rect_cb,
        'FillRectangleCallback':fill_rect_cb,
        'DesktopResizeCallback':dsk_res_cb
        }
    vncmodule.register_cb(cb_dict)
    vncmodule.VNCViewerCreate(False)
    vncmodule.VNCViewerStart("10.220.200.129") # daca e server gresit nu face nimic!


    #vncmodule.VNCSetProperty()
    #time.sleep(5)
    #print(vncmodule.VNCGetProperty())
  
    printy()
    
    try: 
        while(done == False):       
            time.sleep(0.01)            
    except KeyboardInterrupt:
        pass
