from viewer import VNCViewer
from viewer import VNCViewerSDK
import time
import threading

sdk = VNCViewerSDK()

def usage():
    print('*** USAGE ***')
    print(' --help  - display commands')
    print(' --exit  - exit program ')
    print(' --create <amt_ip>   - create viewer')
    print(' --start <nr>   - start viewer')
    print(' --open <nr>   - open viewer')
    print(' --stop <nr>   - stop viewer')
    print(' --list - list viewers')   

    print(' test - move mouse :)')

def handler():
    
    #v1 = sdk.create_viewer()    
    #v1.start_viewer()       
    #v2 = sdk.create_viewer()
    #v2.start_viewer()    
    
    print('*** Welcome! ***')
    run = True
    while(run):
        cmd = input('> ').split()

        if cmd == []:
            pass

        elif cmd[0] == 'test':
            sdk.create_viewer('1.0.0.9')
            sdk.viewer_list[0].start_viewer()
            t = threading.Thread(target=sdk.viewer_list[0].printy, args=())
            t.daemon = True
            t.start()  

        elif cmd[0] == 'test2':
            sdk.create_viewer('1.0.0.10')
            sdk.viewer_list[1].start_viewer()
            t = threading.Thread(target=sdk.viewer_list[1].printy, args=())
            t.daemon = True
            t.start()  

        elif cmd[0] == 'exit':
            run = False

        elif cmd[0] == 'create':
            sdk.create_viewer(cmd[1])

        elif cmd[0] == 'start':
            sdk.viewer_list[int(cmd[1]) - 1].start_viewer()

        elif cmd[0] == 'open':
            t = threading.Thread(target=sdk.viewer_list[int(cmd[1]) - 1].printy, args=())
            t.daemon = True
            t.start()  

        elif cmd[0] == 'stop':
            sdk.viewer_list[int(cmd[1]) - 1].stop_viewer()

        elif cmd[0] == 'list':
            sdk.list_viewers()
            
        elif cmd[0] == 'test':
             sdk.create_viewer(cmd[1]) 
             sdk.viewer_list[int(cmd[1]) - 1].start_viewer()  

        else:
            usage()

        time.sleep(0.1)
    

W
if __name__ == '__main__':
    
    try:
        handler()
    except KeyboardInterrupt:
        pass
    print('fin')
    