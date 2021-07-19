import struct
import sys 

list_dec = [1,64,32,8,51,14,22,17,64,93,70]

with open(sys.argv[1], 'wb')as fp:
    for x in list_dec:
        a = struct.pack('B', x)
        fp.write(a)


print('done')
