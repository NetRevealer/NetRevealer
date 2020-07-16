""" Extract traffic flow features from a CSV file """

import math
import sys
import os
import csv
import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np

file_path = ''
csvheader = ''
Apps = ['Anghami','Youtube','Instagram','Skype', 'Googlemeet', 'Twitch']

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
            self.FistTS = packet[3] * 10000
            
        else:
            IAT = (packet[3] * 10000) - self.LastTS

            self.totalIAT += IAT

            self.maxIAT = max(IAT, self.maxIAT)

            self.minIAT = min(IAT, self.minIAT)

            self.meanIAT = self.totalIAT / (self.nbpackets-1)

        self.LastTS = packet[3] * 10000
        self.duration = self.LastTS - self.FistTS

        if('U' in packet[4]):
            self.urgCount += 1

        if('.' in packet[4]):
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

""" flow labeling from packets """    

def flow_labeling(packets, limit=39):

    flows = []
    flow_in_process, l = packets[0][0], 0

    flow = Flow(flow_in_process)

    for packet in packets:
        
        if(packet[0] == flow_in_process and l < limit):

            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)

            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)   

            l += 1
            
        else:
            if (flow.get_features()[1][0] > limit):
                if ((flow.get_features()[2][0] > 1) and (flow.get_features()[3][0] > 1)):
                    flows.append(flow)

            flow_in_process, l = packet[0], 0
            flow = Flow(flow_in_process)
            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)
                
            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)
    
    if (flow.get_features()[1][0] > limit):
        if ((flow.get_features()[2][0] > 1) and (flow.get_features()[3][0] > 1)):
            flows.append(flow)

    return flows

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
            if ((flow.get_features()[2][0] > 1) and (flow.get_features()[3][0] > 1)):
                flows.append(flow)

            flow_in_process = packet[0]
            flow = Flow(flow_in_process)
            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)
                
            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)

    if ((flow.get_features()[2][0] > 1) and (flow.get_features()[3][0] > 1)):            
        flows.append(flow)    

    return flows

def write_output(output_file, flows, csvheader, lable=''):
    #output_file = file_path[:-4] + '_flows.csv'
    with open(output_file, 'a') as output:
        is_empty = (os.stat(output_file).st_size == 0)
        writer = csv.writer(output)

        if(is_empty):
            writer.writerow(csvheader)

        for f in flows:
            f_flattened = [f.get_features()[0]]
            concat_lists(f_flattened, f.get_features()[1])
            concat_lists(f_flattened, f.get_features()[2])
            concat_lists(f_flattened, f.get_features()[3])

            if (lable != ''):
                f_flattened.append(lable)

            writer.writerow(f_flattened)

def concat_lists(list1, list2):
    for l2 in list2:
        list1.append(l2)


def extractFeatures_fromFlow(flow):
    ignored_cols = [0, 1, 6, 7, 13, 23, 24, 25, 26, 27, 28, 29, 30, 36, 37, 
        43, 53, 54, 55, 56, 57, 58, 59, 60, 66, 67, 73, 76, 83, 84, 85, 86, 87, 88, 89, 90]
    raw_packets = flow.splitlines()
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
    flows = extract_flows(packets)
    
    f_flattened = [flows[0].get_features()[0]]
    concat_lists(f_flattened, flows[0].get_features()[1])
    concat_lists(f_flattened, flows[0].get_features()[2])
    concat_lists(f_flattened, flows[0].get_features()[3])
    
    
    f_flattened = [f_flattened[i] for i in range(91) if i not in ignored_cols]
    flow = torch.Tensor(f_flattened)
    output = model(flow.float())
    _, pred = torch.max(output, 0)
    print('========|{}|========'.format(Apps[pred]))


class Network(nn.Module):
    def __init__(self):
        super().__init__()
        
        self.fc1 = nn.Linear(55, 32)
        self.fc2 = nn.Linear(32, 16)
        self.fc3 = nn.Linear(16, 6)
        
    def forward(self, x):
        # Pass the input tensor through each of our operations
        
        
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.log_softmax(self.fc3(x), dim=0)
        
        return x



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

if __name__ == 'LiveFeatureExtractor':
    print('[*] IN: LiveFeatureExtractor')
    model = Network()
    model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict.pt'))
    print('[*] OUT: LiveFeatureExtractor')


if __name__== '__main__':
    print('[*] IN: __main__')
    model = Network()
    model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict.pt'))
    f_flattened = [33676, 1492, 52, 841.9, 878.900390625, 0.318359375, 1455.220703125, 37.31335136217949, 1455.220703125, 0, 1, 0, 0, 0, 32061, 1311, 350, 801.525, 19, 2344, 1408, 52, 123.36842105263158, 879.548828125, 11.23046875, 1405.041015625, 78.05783420138889, 1405.041015625, 0, 1, 0, 0, 0, 24711, 1311, 1300, 1300.578947368421, 21, 31332, 1492, 1492, 1492.0, 52.568359375, 11.740234375, 576.3203125, 28.816015625, 576.3203125, 0, 0, 0, 0, 7350, 350, 350, 350.0]
    f2 = [[35199], [1470], [52], [879.975], [441.30078125], [0.359375], [801.640625], [20.55488782051282], [801.640625], [0], [7], [0], [0], [0], [35113], [2163], [265], [877.825], [15], [1174], [206], [52], [78.26666666666667], [481.640625], [4.4296875], [774.580078125], [55.3271484375], [774.580078125], [0], [4], [0], [0], [0], [28488], [2163], [1688], [1899.2], [25], [34025], [1470], [52], [1361.0], [27.34765625], [2.2421875], [316.16015625], [13.17333984375], [316.16015625], [0], [3], [0], [0], [6625], [265], [265], [265.0]]
    print(len(f_flattened))
    flow = torch.Tensor(f_flattened)
    
    output = model(flow.float())
    print(output)
    
    print('here done')
    _, pred = torch.max(output, 0)
    print('here done too')
    print('==========================|{}|======================'.format(pred))
    print(Apps[pred])
    