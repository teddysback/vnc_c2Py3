import numpy as np


class PixelBuffer(object):
    '''    
    Initialized at ServerInitCallback: width, height, desktopName, pServerNativePixelFormat
    Copy desktop updates
    '''
       
    def __init__(self, width_height, desktop_name, rgb_max, rgb_shift, big_endian):                
        self.desktop_name = desktop_name
        self.width, self.height = width_height
        self.red_max, self.green_max, self.blue_max = rgb_max
        self.red_shift, self.green_shift, self. blue_shift = rgb_shift

        # ESENTIAL #
        self.buffer = np.zeros((self.width,self.height))
        if big_endian == 0:
            self.big_endian = False
        else:
            self.big_endian = True
        
        self.was_resized = False # [!] should use Observer Pattern

        # to be used
        self.bits_per_pixel = None
        self.depth = None
        self.true_colour = None
       

    def __str__(self):       
        pixel_info = '\nPixel Buffer [{}]'.format(self.desktop_name)
        pixel_info += '\n * width [{}] x height [{}]'.format(self.width, self.height)
        pixel_info += '\n * red max   [{}] shift [{}]'.format(self.red_max, self.red_shift)
        pixel_info += '\n * green max [{}] shift [{}]'.format(self.green_max, self.green_shift)
        pixel_info += '\n * blue max  [{}] shift [{}]'.format(self.blue_max, self.blue_shift)
        pixel_info += '\n * big_endian [{}]'.format(self.big_endian)
        pixel_info += '\n * ...'

        return pixel_info

    #could make one func for upate buffer, and 3 private!!
    def update_rectangle(self,tx_ty, bx_by, pixel_data, data_len): #[!] data_len not used yet
        '''
        Interpret pixel data and update Pixel Buffer submatrix with the new pixel info

        tx_ty: (top left x, top left y)
        bx_by: (bot left x, bot left y)
        pixel_data: submatrix to be interpreted 
        data_len: !!! not used yet !!! check and fix you dumdum
        '''
        top_left_x, top_left_y = tx_ty
        bot_left_x, bot_left_y = bx_by
        
        red = (np.right_shift(pixel_data, self.red_shift) & self.red_max) * 255 / self.red_max
        red = red.astype(np.int32)
        red = np.left_shift(red, 16) # ! fara hardcodari !

        green = (np.right_shift(pixel_data, self.green_shift) & self.green_max) * 255 / self.green_max
        green = green.astype(np.int32)
        green = np.left_shift(green, 8) # ! fara hardcodari !

        blue = (np.right_shift(pixel_data, self.blue_shift) & self.blue_max) * 255 / self.blue_max
        blue = blue.astype(np.int32)
        blue = np.left_shift(blue, 0) # ! fara hardcodari !??

        temp = red + green + blue
        temp = np.fliplr(temp)
        temp = np.rot90(temp)
        self.buffer[top_left_x:bot_left_x , top_left_y:bot_left_y] = temp
        
        # mini hack
        if top_left_x > self.width * 3/4 and (top_left_y > self.height * 37/100):# or bot_left_y < self.height * 66/100 ):
            #self._remove_watermark()
            pass 

        return True
      
    ## LEGIT - endianess solved    
    def fill_rectangle(self,tx_ty, bx_by, pixel_data): 
        '''Fill rectangle at a given pos in Pixel Buffer with a single colour

        tx_ty: (top left x, top left y)
        bx_by: (bot left x, bot left y)
        pixel_data: bytearray to be interpreted 
        '''
        top_left_x, top_left_y = tx_ty
        bot_left_x, bot_left_y = bx_by
        
        if self.big_endian:
            pixel_data = int.from_bytes(pixel_data, byteorder='big')
        else:
            pixel_data = int.from_bytes(pixel_data, byteorder='little')
        
        red = ((pixel_data >> self.red_shift) & self.red_max) * 255 / self.red_max        
        red = int(red) << 16
        green = ((pixel_data >> self.green_shift) & self.green_max) * 255 / self.green_max
        green = int(green) << 8
        blue = ((pixel_data >> self.blue_shift) & self.blue_max) * 255 / self.blue_max
        blue = int(blue) << 0

        self.buffer[top_left_x:bot_left_x , top_left_y:bot_left_y] = red + green + blue

        return True

    def copy_rectangle(destination, source):#NOT TESTED
        '''Copy submatrix within the Pixel Buffer

        destination: (dest->topLeft.x, dest->topLeft.y, dest->bottomRight.x, dest->bottomRight.y)
                     The rectangle on the VNC server desktop that is being
                     updated - that is, the destination of the copy.
        source: (pSource.x, pSource.y)
                The top-left corner of the source of the copy.  The area to
                be copied is the same as the area of *pDestination.
        '''
        dest_top_x, dest_top_y, dest_bot_x, dest_bot_y = destination
        src_x, src_y = source

        x = dest_bot_x - dest_top_x
        y = dest_bot_y - dest_top_y

        self.buffer[dest_top_x:dest_bot_x, dest_top_y:dest_bot_y] = self.buffer[src_x:src_x + x,src_y:src_y + y] 
        
        return True

    def _remove_watermark(self):
        '''
        paint something over the RealVNC watermark
        (private method)
        '''
        bx = self.width                 #tx = self.width * 3/4;
        by = self.height * 2/3          #ty = self.height * 2/5; 

        self.buffer[bx-320:bx,by-260:by] = self.buffer[bx-320:bx,0:260]
       

    def resize_screen(self, new_width_height):        
        self.width, self.height = new_width_height
        self.buffer = np.zeros((self.width,self.height))
        self.was_resized = True
        #self.width = new_width
        #self.hight = new_hight

    #def printy(self):
    #    pygame.init()
    #    #pygame.font.Font("FreeSansBold.ttf", 14).render(u'\u2654')
    #    #print('w{} h{}'.format(f.width,f.height))
    #    screen = pygame.display.set_mode((self.pixel_buffer.width,self.pixel_buffer.height))#, pygame.OPENGL)
    #    clock = pygame.time.Clock()
    #    retries = 70
    #    while True:
    #        for event in pygame.event.get():
    #            if event.type == pygame.QUIT:                
    #                run = False            
    #                print('we shall quit')
             
    #        if self.pixel_buffer.was_resized:
    #            screen = pygame.display.set_mode((self.pixel_buffer.width,self.pixel_buffer.height))
    #            self.pixel_buffer.was_resized = False
    #        #screen1 = pygame.surfarray.make_surface(data)
            
    #        #pygame.transform.flip(screen1, False, True)
    #        #pygame.transform.rotate(screen1, 90)

    #        #screen.blit(screen1, (0,0))
    #        try:
    #            pygame.surfarray.blit_array(screen, self.pixel_buffer.buffer)
    #        except:
    #            pass
    #        pygame.display.flip()
      
    #        clock.tick(20)  
       
    #    time.sleep(0.1)
    #    pygame.quit()

##########################################


##########################################

#data2 = 0
#def printy2():
#    while data2 is 0:
#        time.sleep(1)
#    pygame.init()
#    screen = pygame.display.set_mode((64,64))
#    clock = pygame.time.Clock()
#    while True:
#        for event in pygame.event.get():
#            if event.type == pygame.QUIT:                
#                run = False
            
#                break

#        #screen1 = pygame.surfarray.make_surface(data)
            
#        #pygame.transform.flip(screen1, False, True)
#        #pygame.transform.rotate(screen1, 90)

#        #screen.blit(screen1, (0,0))
#        try:
#            pygame.surfarray.blit_array(screen, data2)
#        except:
#            pass
#        pygame.display.flip()
#        clock.tick(20)       
#    time.sleep(0.1)
#    pygame.quit()

#if __name__ == '__main__':
#    pass