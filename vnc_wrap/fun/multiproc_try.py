#from multiprocessing import Pool
from multiprocessing import Lock, Process
import multiprocessing as mp
import time
import os

def f(x):
    time.sleep(2)
    print(" this is my ard: %d "%x)
    time.sleep(2)

def info(title):
    print(title)
    print('module name:', __name__)
    print('parent process:', os.getppid())
    print('process id:', os.getpid())

def f2(name, lastname):
    info('func f2')
    print("hi", name, lastname)

def foo(q):
    q.put('hello')


def f(l, i):
    l.acquire()
    try:
        print('hello world', i)
    finally:
        l.release()

if __name__ == '__main__':
    #with Pool(5) as p:
    #    print(p.map(f,[1, 2, 3]))
    
    #info('main')
    #p = Process(target=f2, args=('razvan', 'teslaru',))
    #p.start()
    #p.join()

    #mp.set_start_method('spawn')
    #q = mp.Queue()
    #p = mp.Process(target=foo, args=(q,))
    #p.start()
    #print(q.get())
    #p.join()

    #lock = Lock()
    #for num in range(10):
    #    Process(target=f, args=(lock, num)).start()
    pass

