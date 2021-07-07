from pixelbuffer import PixelBuffer
import vncmodule as vnc
import time
import pygame


class VNCViewer(object):
    ''' Viewer Class
    '''
    #region ESSENTIAL Callbacks       
    def server_init_cb(self, width, height, desktop_name, red_max, green_max, blue_max, red_shift, green_shift, blue_shift,big_endian):
        print("[SrvInit_CB] conn established succesfully >%s<"%desktop_name )    
        
        self.pixel_buffer = PixelBuffer((width,height), desktop_name, (red_max,green_max,blue_max), (red_shift,green_shift,blue_shift),big_endian)
        self.__status_cb_called = False


    def status_cb(self, code, status, more_status):
        print("[Sts_CB] \n\terror code:%d \n\t%s \n\t%s\n"%(code, status, more_status))
        self.__status_cb_called = True

    def credentials_cb(self):
        print("[Cred_CB] >> %s <<"%self.ip)
        ret = (None, 'P@sswo2;')
        return ret
    #endregion

    #region DESKTOP UPDATE Callbacks
    def img_rect_cb(self, top_left_xy, bot_right_xy, pixel_data, data_len):
        if not self.pixel_buffer.update_rectangle(top_left_xy, bot_right_xy, pixel_data, data_len):
            print("eroare update_buffer")
    

    def fill_rect_cb(self, top_left_xy, bot_right_xy, pixel_data):
        if not self.pixel_buffer.fill_rectangle(top_left_xy, bot_right_xy, pixel_data):
            print("eroare update_buffer2")
            
    def copy_rect_cb(self, destination, source):
        if not self.pixel_buffer.copy_rectangle(destination, source):
            print("eroare update_buffer3")

    def dsk_res_cb(self, width_height):
        self.pixel_buffer.resize_screen(width_height)
    #endregion

    def __init__(self, ip):        
        self.ip = ip        
        self.callbacks = {
            'ServerInitCallback':       VNCViewer.server_init_cb, 
            'StatusCallback':           VNCViewer.status_cb,
            'CredentialsCallback':      VNCViewer.credentials_cb,
            'ImageRectangleCallback':   VNCViewer.img_rect_cb,
            'CopyRectangleCallback':    VNCViewer.copy_rect_cb,
            'FillRectangleCallback':    VNCViewer.fill_rect_cb,
            'DesktopResizeCallback':    VNCViewer.dsk_res_cb
        }
        self.__status_cb_called = False
        self.pixel_buffer = None
    
    #region Viewer Management
    def create_viewer(self):
        vnc.create_viewer(self.callbacks, self.ip, self)
                
    def start_viewer(self):
        if not vnc.start_viewer(self.ip) == 0:
            print('[ERROR] ListGetNode failed for [%s]'%self.ip)
            print('[ERROR] You entered a bad IP or list is corrupt')
            return
        print('[dbg] viewer started [%s]'%self.ip)
        #probably we'll need some flags

    def stop_viewer(self):
        vnc.stop_viewer(self.ip)

    def destroy_viewer(self):
        self.stop_viewer()
        retries = 7
        while (retries):
            if self.__status_cb_called:
                time.sleep(1)
                vnc.destroy_viewer(self.ip)
                return
            retries = retries - 1
            time.sleep(1)
        print('viewer NOT destroied')
    #endregion

    def printy(self):
        
        while self.pixel_buffer == None:
            time.sleep(1)   

        pygame.init()
        screen = pygame.display.set_mode((self.pixel_buffer.width,self.pixel_buffer.height))#, pygame.OPENGL)
        clock = pygame.time.Clock()
        run = True
        while run:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:                
                    run = False
                # handle MOUSEBUTTONUP
                #mouse_mask = 0x0
                #if event.type == pygame.MOUSEBUTTONDOWN:
                #    if event.button == 1:
                #        mouse_mask = mouse_mask | 0x01
                #    elif event.button == 3:
                #        mouse_mask = mouse_mask | 0x04
                #    else:
                #        print('NOT HANDLED:', event.button)
                #    vnc.mouse(self.ip, mouse_mask, event.pos[0], event.pos[1])
                #if event.type == pygame.MOUSEBUTTONUP:
                #   mouse_mask = 0
                #   vnc.mouse(self.ip, mouse_mask, event.pos[0], event.pos[1])
                       
                #vnc.mouse(self.ip, event.pos[0], event.pos[1])
                #print('[py] ok \n')

            if self.pixel_buffer.was_resized:
                screen = pygame.display.set_mode((self.pixel_buffer.width,self.pixel_buffer.height))
                self.pixel_buffer.was_resized = False
         
            #screen1 = pygame.surfarray.make_surface(data)            
            #pygame.transform.flip(screen1, False, True)
            #pygame.transform.rotate(screen1, 90)
            #screen.blit(screen1, (0,0))
            try:
                pygame.surfarray.blit_array(screen, self.pixel_buffer.buffer)
            except:
                pass
            pygame.display.flip()
      
            clock.tick(20)  
       
        time.sleep(0.1)
        pygame.quit()


class VNCViewerSDK(object):
    def __init__(self):
        self.viewer_list = []

    def __del__(self):
        # stop & destroy leftovers
        # vnc.free()
        pass

    def create_viewer(self, ip):
        viewer = VNCViewer(ip)
        self.viewer_list.append(viewer)

        viewer.create_viewer()
        return viewer

    def stop_viewer(self, viewer):
        viewer.stop_viewer()

    def destroy_viewer(self, viewer):
        viewer.destroy_viewer()
        
        self.viewer_list.remove(viewer)

    def list_viewers(self):
        nr = 0
        for view in self.viewer_list:
            print('\t viewer[%d] IP:[%s]'%(nr, view.ip))
            nr = nr + 1
    def test(self):
        vnc.mouse(self.viewer_list[0].ip)

   
 


if __name__ == '__main__':
    #v.init()
    pass