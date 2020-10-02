from os import listdir, getcwd
import sys
from statistics import stdev
scheds = ['fcfs/', 'strn/', 'rr/'] 
output = "./output/"
input_ = "./tests/"

# le as context changes e as deadlines

tests = []
stdevs = []
means = []
data = ''
deadlines = []
for scheduler in range(0, 3):
    filenames = listdir(input_)
    for f in filenames:
        with open(input_ + f, "r") as f_ :
            while 1:
                line = f_.readline()
                if line == '':
                    break
                line = line.split()
                try :
                    dl = (int(line[-1]))
                    deadlines.append(dl)
                except:
                    pass


for scheduler in range(0, 3):
    filenames = listdir(output + scheds[scheduler])
    dl_dt = []
    context = []
    tests.append(dl_dt) 
    for f in filenames:
        print(f)
        with open(output + scheds[scheduler] + f, "r") as f_ :
            line = ''
            while 1 :
                data = line
                line = f_.readline()
                if line == '':
                    break
                line = line.split()
                try:
                    obj = (int(line[-2]))
                    dl_dt.append(obj)
                except:
                    pass
            context.append(int(data[0]))

            f_.close()

    met_deadlines = []


    for obj in range(len(dl_dt)) :
        deadline = deadlines[obj] 
        dt = dl_dt[obj]
        if deadline < dt:
            met_deadlines.append(0)
        else:
            met_deadlines.append(1)

    print(met_deadlines, context)

    if (len(met_deadlines) == 0):
        mean_dl = 0
        stdev_dl = 0
    else:
        mean_dl = sum(met_deadlines)/len(met_deadlines)
        stdev_dl = stdev(met_deadlines)

    if (len(context) == 1) :
        mean_context = 9
        stdev_context = 9

    else :
        mean_context = sum(context)/len(context)
        stdev_context = stdev(context)

    means.append((mean_dl, mean_context))
    stdevs.append((mean_dl, mean_context))

    for i, j in zip(means, stdevs):
        print(i, j)




