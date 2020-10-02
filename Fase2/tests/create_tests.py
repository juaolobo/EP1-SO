from sys import argv
from random import uniform
for i in range(3):
    with open('./trace' + str(i+1) + '.txt', "w") as f:
        t0 = 1
        dt = 1
        dl = 1
        for j in range(1, (10**(i+1)+1)):
            t0 = int(uniform(t0, j))
            dt = int(uniform(1,5))
            dl = t0 + dt + int(uniform(1,5))*int(uniform(1,3))
            f.write('processo' + str(j) + ' ' + str(t0) + ' ' + str(dt) + ' ' + str(dl))
            if j != (10**(i+1)+1)-1:
            	f.write('\n')
