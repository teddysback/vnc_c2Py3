import unittest
import time
import vncmodule as vnc


init_called = False
sts_called = False
cred_called = False
sts_code = -1


def credentials(self):
    global cred_called
    cred_called = True
    ret = (None, 'P@sswo2;') # set with amt tool
    return ret

def wrong_credentials(self):
    global cred_called
    cred_called = True
    ret = (None, 'P@sswo2') # set with amt tool
    return ret

def init(self, width, height, desktop_name, red_max, green_max, blue_max, red_shift, green_shift, blue_shift,big_endian):
    global init_called
    init_called = True

def status(self, code, status, more_status):
    global sts_called
    global sts_code
    sts_called = True
    sts_code = code

def img_rect_cb(self, top_left_xy, bot_right_xy, pixel_data, data_len):
    pass

def fill_rect_cb(self, top_left_xy, bot_right_xy, pixel_data):
    pass
    
def copy_rect_cb(self, destination, source):
    pass

def dsk_res_cb(self, width_height):
    pass


class CallbacksCallTest(unittest.TestCase):
    '''
    '''
    def setUp(self):  
      
        self.ip = '10.0.0.130'         
        self.callbacks = {
            'ServerInitCallback':       init, 
            'StatusCallback':           status,
            'CredentialsCallback':      credentials,
            'ImageRectangleCallback':   img_rect_cb,
            'CopyRectangleCallback':    copy_rect_cb,
            'FillRectangleCallback':    fill_rect_cb,
            'DesktopResizeCallback':    dsk_res_cb        
        }    
        vnc.create_viewer(self.callbacks, self.ip, self)
        vnc.start_viewer(self.ip)
        time.sleep(3)

    def test_cred(self):
        global cred_called
        self.assertEqual(cred_called, True, 'Credentials callback not called')  
        cred_called = False  
           
    def test_init(self):
        global init_called
        self.assertEqual(init_called, True, 'Init callback not called')    
        init_called = False

    def test_stop_viewer(self):
        vnc.stop_viewer(self.ip)

    def test_sts(self):
        global sts_called
        self.assertEqual(sts_called, True, 'Status callback not called')
        sts_called = False

class WrongCredentialsTest(unittest.TestCase):
    def setUp(self):
        self.ip = '10.0.0.129'         
        self.callbacks = {
            'ServerInitCallback':       init, 
            'StatusCallback':           status,
            'CredentialsCallback':      wrong_credentials,
            'ImageRectangleCallback':   img_rect_cb,
            'CopyRectangleCallback':    copy_rect_cb,
            'FillRectangleCallback':    fill_rect_cb,
            'DesktopResizeCallback':    dsk_res_cb        
        }    
        vnc.create_viewer(self.callbacks, self.ip, self)
        vnc.start_viewer(self.ip)
        time.sleep(3)
    
    def test_cred(self):
        global cred_called
        self.assertEqual(cred_called, True, 'Credentials callback not called')   
    
    def test_init(self):
        global init_called
        self.assertEqual(init_called, False, 'Init callback not called')    
    
    def test_sts(self):
        global sts_called
        self.assertEqual(sts_called, True, 'Status callback not called')

    def test_sts_code(self):
        global sts_code
        self.assertEqual(sts_code, 7, 'Expected different status')

if __name__ == '__main__':
    unittest.main()
    input()