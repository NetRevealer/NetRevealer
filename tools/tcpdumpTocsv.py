'''
tcpdumpTocsv is a converter of tcpdump output
to csv. This converted is not adjusted for all
types of tcpdump commands. It is only for the
command : tcpdump -nvvv
'''


import csv
import sys

csvheader = ['pktWay', 'proto', 'srvIP', 'pktlen', 'srcprt', 'dstprt', 'timeStamp', 'synflag'
, 'ackflag', 'finflag', 'resetflag', 'pushflag', 'urgflag', 'pktwin', 'pktseq', 'pktack']
pktWay = ''
srcprt = ''
dstprt = ''
srvIP = ''
hostIP = ''
timeStamp = ''
pktlen = ''
cksum = ''
proto = 0
pktseq = 0
pktack = 0
pktwin = 0
synflag = 0
ackflag = 0
finflag = 0
resetflag = 0
pushflag = 0
urgflag = 0

input_path = ''
output_file = ''

try:
    input_path = sys.argv[1]
    output_file = input_path[:-3] + 'csv'
except IndexError:
    print('[!] Please enter the path of the txt file you want to convert')
    exit()


with open(input_path, 'r') as file:
    linestmp = list((line.strip() for line in file))
    lines = []
    preprocess = list()
    postprocess = list()
    j = 0
    for l in range(0, len(linestmp), 2):
        lines.append(linestmp[l]+linestmp[l+1])
        preprocess.append(lines[j].split(' '))
        j += 1

for p in preprocess:
    synflag = 0
    ackflag = 0
    finflag = 0
    resetflag = 0
    pushflag = 0
    urgflag = 0
    pktseq = 0
    pktack = 0
    pktwin = 0
    x = list()
    timeStamp = p[0]
    pktlen = p[16].split(')')[0]
    srcprt = p[16].split('.')[-1].split(':')[0]
    dstprt = p[18].split('.')[-1].split(':')[0]

    if (p[16].split(')')[1][0:3] == '192'):
        pktWay = 'F'
        srvIP = '.'.join(p[18].split('.')[:-1])
        
    else:
        pktWay = 'B'
        srvIP = '.'.join(p[16].split(')')[1].split('.')[:-1])

    if (p[13] == 'TCP'):
        proto = 6
        pktseq = p[25][0:-1].split(':')[0]
        pktack = p[27][0:-1]
        pktwin = p[29][0:-1]
        
        if ('S' in p[20]):
            synflag = 1
        if ('.' in p[20]):
            ackflag = 1
        if ('F' in p[20]):
            finflag = 1
        if ('R' in p[20]):
            resetflag = 1
        if ('P' in p[20]):
            pushflag = 1
        if ('U' in p[20]):
            urgflag = 1
        
    elif (p[13] == 'UDP'):
        proto = 17
    else:
        proto = -1

    x.append(pktWay)
    x.append(proto)
    x.append(srvIP)
    x.append(pktlen)
    x.append(srcprt)
    x.append(dstprt)
    x.append(timeStamp)
    x.append(synflag)
    x.append(ackflag)
    x.append(finflag)
    x.append(resetflag)
    x.append(pushflag)
    x.append(urgflag)
    x.append(pktwin)
    x.append(pktseq)
    x.append(pktack)
    
    postprocess.append(x)


with open(output_file, 'w') as output:
        writer = csv.writer(output)
        writer.writerow(csvheader)
        for p in postprocess:
            writer.writerow(p)

print('[*] Done.')
