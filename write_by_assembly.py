import sys
import random  
import re
def int2bin(n, count): 
    n = str(n)
    if not n.isdigit():
        n = ord(n)
        if 65 <= n <= 90:
            n -= 65
        elif 97 <= n <= 122:
            n = n - 97 + 26
    return "".join([str((int(n) >> y) & 1) for y in range(count-1, -1, -1)])


def judgetype(string):
    if string == 'VAL':
        return 8
    elif string == 'REG':
        return 3
    elif string == 'STK':
        return 5
    elif string == 'PTR':
        return 5


filename = sys.argv[1]

with open(filename, 'r') as f:
    lines = f.readlines()

binarys = ''
num_of_in = 0
entry = 1
last_num_in = 0
for i in range(len(lines)):
    
    line = lines[i]
    info = line.strip().split()


    if info[0] == 'FUNC':
        if entry == 0:
            binarys += int2bin(num_of_in, 5)
        if entry == 1:
            entry = 0
        f_num = info[2] 
        binarys += int2bin(int(f_num), 3)
        num_of_in = 0
    if info[0] == 'MOV':
        instru = ''
        instru += '000'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
            
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru

        if info[3] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[4], 8) + instru
   
        elif info[3] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[4], 3) + instru
        elif info[3] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[4], 5) + instru
        elif info[3] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[4], 5) + instru

        binarys += instru
        num_of_in += 1

    if info[0] == 'CAL':
        instru = ''
        instru += '001'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru


        binarys += instru
        num_of_in += 1

    if info[0] == 'RET':
        instru = ''
        instru += '010'
        
        binarys += instru
        num_of_in += 1
    
    if info[0] == 'REF':
        instru = ''
        instru += '011'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru

        if info[3] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[4], 8) + instru
        elif info[3] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[4], 3) + instru
        elif info[3] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[4], 5) + instru
        elif info[3] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[4], 5) + instru

        binarys += instru
        num_of_in += 1

    if info[0] == 'ADD':
        instru = ''
        instru += '100'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru

        if info[3] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[4], 8) + instru
        elif info[3] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[4], 3) + instru
        elif info[3] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[4], 5) + instru
        elif info[3] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[4], 5) + instru

        binarys += instru
        num_of_in += 1

    if info[0] == 'PRINT':
        instru = ''
        instru += '101'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru

        

        binarys += instru
        num_of_in += 1
    
    if info[0] == 'NOT':
        instru = ''
        instru += '110'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru


        binarys += instru
        num_of_in += 1
    
    if info[0] == 'EQU':
        instru = ''
        instru += '111'
        if info[1] == 'VAL':
            instru = '00' + instru
            instru = int2bin(info[2], 8) + instru
        elif info[1] == 'REG':
            instru = '01' + instru
            instru = int2bin(info[2], 3) + instru
        elif info[1] == 'STK':
            instru = '10' + instru
            instru = int2bin(info[2], 5) + instru
        elif info[1] == 'PTR':
            instru = '11' + instru
            instru = int2bin(info[2], 5) + instru


        binarys += instru
        num_of_in += 1

    if i == len(lines) - 1:
        last_num_in = num_of_in

binarys += int2bin(last_num_in,5)
if len(binarys) % 8 != 0:
    binarys = '0' * (8 - len(binarys) % 8) + binarys

print(binarys)
print('\n')
hexs = re.findall(r'\w{8}', binarys)

for h in hexs:
    print(int(h, 2), end = ',')
        
    
