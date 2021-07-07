#import numpy as np

#a = np.matrix('1 2 3 4; 4 6 7 8')
#b = np.matrix('0 0; 0 0')
##print(a + b)

#c = np.append(a, b, axis=1)
#print(c)
#print("")

#tx = 0; ty = 0
#bx = 2; by = 2

#print(c[tx:bx, ty:by])
#print("")

#ty = ty + 1
#by = by + 1

#c[tx:bx, ty:by] = b
#print(c)
#print("")

#c = np.append()





from framebuffer import PixelBuffer

f = PixelBuffer((1,2),'ceva',(1,2,3),(2,3,4))


def shit(txy,bxy):
    print(txy)
    print(bxy)
    f.update_buffer(txy,bxy)

if __name__ == '__main__':
    shit((12,12),(15,15))