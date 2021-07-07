import vncmodule
from pixelbuffer import PixelBuffer
#import input_box

class RFB(object):
    ''' 
    
    '''
    def __init__(self):
        self.buffer = None
        self.callback_dict = {
            'ServerInitCallback':server_init_cb, 
            'StatusCallback':status_cb,
            'CredentialsCallback':credential_cb,
            'ImageRectangleCallback':img_rect_cb,
            'CopyRectangleCallback':copy_rect_cb,
            'FillRectangleCallback':fill_rect_cb,
            'DesktopResizeCallback':desktop_resize_cb
            }

    #
    # Default CALLBACKS
    #
    def server_init_cb(width, height, desktop_name, red_max, green_max, blue_max, red_shift, green_shift, blue_shift,big_endian):
        ''' Create a pixel buffer with the necessary info
        '''
        self.buffer = PixelBuffer((width,height), desktop_name, (red_max,green_max,blue_max), (red_shift,green_shift,blue_shift),big_endian)
    

    def status_cb(code, status, more_status):
        ''' Called when there is a issue
        '''
        global done # !!!!!
        print("[Sts_CB] \n\terror code:%d \n\t%s \n\t%s\n"%(code, status, more_status))

        done = True
    
    
    def credential_cb():
        ''' Input a password
        '''
        screen = pygame.display.set_mode((240,100))
        try:
            password = input_box.ask(screen, "PASSWORD")
        except:
            password = (None, 'P@sswo2;')

        ret = (None, password)   
        return ret
    # <!> Default callbacks


    #
    # Desktop CALLBACKS
    #
    def img_rect_cb(top_left_xy, bot_right_xy, pixel_data, data_len):
        ''' Called when there is an image rectangle update
        '''
        if not self.buffer.update_rectangle(top_left_xy, bot_right_xy, pixel_data, data_len):
            print("eroare update_buffer")
    

    def fill_rect_cb(top_left_xy, bot_right_xy, pixel_data):
        ''' Called when there is an rectangle update
        '''
        if not self.buffer.fill_rectangle(top_left_xy, bot_right_xy, pixel_data):
            print("eroare update_buffer2")


    def copy_rect_cb(destination, source):
        ''' Called when there is an image rectangle copy
        '''
        if not self.buffer.copy_rectangle(destination, source):
            print("eroare update_buffer3")


    def dsk_res_cb(self,width_height):
        self.buffer.resize_screen(width_height)

