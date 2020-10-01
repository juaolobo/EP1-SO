from os import listdir
import sys
from statistics import stdev

filenames = listdir('./output')

# le as context changes e as deadlines


dl_dt = {}
context = []
for f + sys.argv[1] in filenames:
    with open(f, "r") as f_ :
        while 1 :
            line = f_.readline()
            if line == '':
                break
            line = line.split()
            obj = {line[-1] : line[-2]}
            dl_dt[line[-1]] = line[-2]

        data = f_.read()
        context += int(data[-1])

        f_.close()

met_deadlines = []

for dt in dl_dt :
    dl_dt[dt] = deadline
    if deadline < dt:
        met_deadlines.append(0)
    else:
        met_deadlines.append(1)

mean_dl = sum(met_dealines)/len(met_deadlines)

stdev_dl = stdev(met_deadlines)

mean_context = sum(context)/len(context)

stdev_del = stdev(context)



