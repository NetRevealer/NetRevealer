""" Extract traffic flow features from a CSV file """

import math
import sys
import csv

file_path = ''
csvheader = ''

class Flow_Stat_Features():

    def __init__(self):

        self.nbpackets = 0
        """ len features """
        self.totallen = 0
        self.maxlen = 0
        self.minlen = math.inf
        self.meanlen = 0
        """ time features """ 
        self.FistTS = 0
        self.LastTS = 0
        self.maxIAT = 0
        self.minIAT = math.inf
        self.totalIAT = 0
        self.meanIAT = 0
        self.duration = 0
        """ tcp flags counts """
        self.urgCount = 0
        self.ackCount = 0
        self.pshCount = 0
        self.rstCount = 0
        self.synCount = 0
        self.finCount = 0
        """ window size features """
        self.totalwin = 0
        self.maxwin = 0
        self.minwin = math.inf
        self.meanwin = 0
        """ sequance number features """
        self.totalseq = 0
        self.maxseq = 0
        self.minseq = math.inf
        self.meanseq = 0
        """ ack number features """
        self.totalack = 0
        self.maxack = 0
        self.minack = math.inf
        self.meanack = 0

    def get_values(self):

        return [self.nbpackets, self.totallen, self.maxlen, self.minlen, self.meanlen,
                self.FistTS, self.LastTS, self.maxIAT, self.minIAT, self.totalIAT, self.meanIAT, self.duration,
                self.urgCount, self.ackCount, self.pshCount, self.rstCount, self.synCount, self.finCount,
                self.totalwin, self.maxwin, self.minwin, self.meanwin,self.totalseq, self.maxseq, self.minseq, self.meanseq,
                self.totalack, self.maxack, self.minack, self.meanack]


    def update_values(self,packet):
        self.nbpackets += 1

        self.totallen += packet[2]

        self.maxlen = max(packet[2], self.maxlen)

        self.minlen = min(packet[2], self.minlen)

        self.meanlen = self.totallen / self.nbpackets

        if self.nbpackets == 1 :
            self.FistTS = packet[3]
            
        else:
            IAT = packet[3] - self.LastTS

            self.totalIAT += IAT

            self.maxIAT = max(IAT, self.maxIAT)

            self.minIAT = min(IAT, self.minIAT)

            self.meanIAT = self.totalIAT / (self.nbpackets-1)

        self.LastTS = packet[3]
        self.duration = self.LastTS - self.FistTS

        if('U' in packet[4]):
            self.urgCount += 1

        if('A' in packet[4]):
            self.ackCount += 1

        if('P' in packet[4]):
            self.pshCount += 1

        if('R' in packet[4]):
            self.rstCount += 1

        if('S' in packet[4]):
            self.synCount += 1

        if('F' in packet[4]):
            self.finCount += 1

        self.totalwin += packet[5]

        self.maxwin = max(packet[5], self.maxwin)

        self.minwin = min(packet[5], self.minwin)

        self.meanwin = self.totalwin / self.nbpackets

        self.totalseq += packet[6]

        self.maxseq = max(packet[6], self.maxseq)

        self.minseq = min(packet[6], self.minseq)

        self.meanseq = self.totalseq / self.nbpackets

        self.totalack += packet[7]

        self.maxack = max(packet[7], self.maxack)

        self.minack = min(packet[7], self.minack)

        self.meanack = self.totalack / self.nbpackets                        


class Flow():

    def __init__(self,flowid):

        self.flowid = flowid
        self.global_features = Flow_Stat_Features()
        self.forward_features = Flow_Stat_Features()
        self.backward_features = Flow_Stat_Features()

    def get_features(self):

        return [self.flowid, self.global_features.get_values(), self.forward_features.get_values(), self.backward_features.get_values()]


             
""" extracting packets from raw data """


def extract_packets(file_path):
    
    with open(file_path,"r") as data :

        raw_packets = data.read().splitlines()
        raw_packets = [item for item in raw_packets if len(item) > 1]
        packets = []

        for pack in raw_packets:
            values = pack.split(",")
            try:
                packet = ["-".join(values[1:3]+values[4:6]),values[0],int(values[3]),float(values[6]),values[7],int(values[8]),
                int(values[9]),int(values[10])]
                packets.append(packet)
            except:
                pass
            
        packets.sort(key=lambda packet: packet[0])    

    return packets



""" extracting flow features from packets """    

def extract_flows(packets):

    flows = []
    flow_in_process = packets[0][0]

    flow = Flow(flow_in_process)

    for packet in packets:

        if(packet[0] == flow_in_process):

            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)

            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)   

        else:
            flows.append(flow)

            flow_in_process = packet[0]
            flow = Flow(flow_in_process)
            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)
                
            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)
    flows.append(flow)    

    return flows

def write_output(file_path, flows):
    output_file = file_path[:-3] + '_flows.csv'
    with open(output_file, 'w') as output:
        writer = csv.writer(output)
        writer.writerow(csvheader)
        for f in flows:
            writer.writerow(f)

def main():
    try:
        file_path = sys.argv[1]
    except IndexError:
        print('[!] Please enter the path of the captured packets (csv) you want extract flows from.')
        exit()
    
    packs = extract_packets(file_path)
    flows = extract_flows(packs)
    for f in flows:
        x = f.get_features()
        print(*x)

if __name__== '__main__':
    main()