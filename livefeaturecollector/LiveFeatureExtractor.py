""" Extract traffic flow features from a CSV file """

import math
import sys
import os
import csv
import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision import transforms
import numpy as np

file_path = ''
csvheader = ''
Apps = ['Anghami','Youtube','Instagram','Skype', 'Googlemeet', 'Twitch']
mean = np.array([26596.706044857296, 1240.7684617642487, 65.67361914463801, 664.9176511214107, 1466.690906578865, 0.6297571781061692, 4714.8354236315545, 120.89321599055195, 4714.8354236315545, 26.90810988138198, 1.5958008874769065, 0.0016402783293333564, 0.008270455997375555, 0.0005179826303157968, 78578.77169052264, 4579.915776024311, 392.3604468463491, 1964.469292263053, 18.387571870089957, 3126.9285701952795, 319.4241759759656, 71.46632249598564, 149.30606734949066, 1471.3529628404117, 14.586752310957923, 4611.501829699451, 238.88621395640908, 4611.501829699451, 11.151561717630402, 0.2334202393079752, 0.001623012241656163, 0.003936667990400055, 0.00018992696444912547, 69308.10128287031, 4561.934716922493, 4263.098140442357, 4308.355860116002, 21.612428129910043, 23469.777474662016, 1147.7975378558972, 676.0977951206037, 1012.8239877174965, 1525.6006670914844, 35.96112816886665, 4455.18144992241, 281.00884501612376, 4455.18144992241, 15.756548163751576, 1.3623806481689313, 0.004333788006975499, 0.00032805566586667127, 9270.67040765233, 569.7002261857485, 399.34937928414803, 408.130168743969])
std = np.array([11657.100742766126, 419.8897593438337, 22.93187086932821, 291.42751856912236, 12785.860281753074, 1.9523809056657333, 36899.711552383735, 946.1464500611211, 36899.711552383735, 18.763863755115867, 3.6331549969314962, 0.15395989045132355, 0.12760323110284025, 0.031641188019272265, 78369.66000139581, 5974.956154491242, 548.388662287432, 1959.24150003502, 5.243431261173247, 4110.026941052746, 420.53640143504816, 38.66691345329793, 164.2612602484499, 12355.17875536256, 165.79729259886298, 36710.646260875874, 1690.657515723557, 36710.646260875874, 8.129568266202455, 1.2157022955874728, 0.1539039899559851, 0.06261925131724928, 0.014980756574734598, 71868.85041338728, 5936.003147288437, 4587.999085958646, 4594.753955472143, 5.243431261173247, 14038.583017670973, 513.3814198097804, 621.9019708367877, 555.8447583361445, 12667.799617016812, 898.2499906493082, 35964.43633886633, 2385.548487113716, 35964.43633886633, 11.240367551137497, 3.116691984650395, 0.06673181392640497, 0.023133116899211128, 12712.582997060581, 2878.942202808302, 559.1322791253796, 581.4964579112329])
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
    
    # print(f_flattened[0])
    f_flattened = [f_flattened[i] for i in range(91) if i not in ignored_cols]
    # print(f_flattened[0])
    f_flattened = np.array(f_flattened)
    for i in range(55):
        f_flattened[i] = (f_flattened[i] - mean[i])/std[i]
    
    flow = torch.Tensor(f_flattened)
    output = model(flow.float())
    _, pred = torch.max(output, 0)
    # print(pred)
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
    f_flattened = np.array([33676, 1492, 52, 841.9, 878.900390625, 0.318359375, 1455.220703125, 37.31335136217949, 1455.220703125, 0, 1, 0, 0, 0, 32061, 1311, 350, 801.525, 19, 2344, 1408, 52, 123.36842105263158, 879.548828125, 11.23046875, 1405.041015625, 78.05783420138889, 1405.041015625, 0, 1, 0, 0, 0, 24711, 1311, 1300, 1300.578947368421, 21, 31332, 1492, 1492, 1492.0, 52.568359375, 11.740234375, 576.3203125, 28.816015625, 576.3203125, 0, 0, 0, 0, 7350, 350, 350, 350.0])
    
    for i in range(55):
        f_flattened[i] = (f_flattened[i] - mean[i])/std[i]
    
    flow = torch.Tensor(f_flattened)
    
    
    print('to tensor done!!!!')
    
    output = model(flow.float())
    print(output)
    
    print('here done')
    _, pred = torch.max(output, 0)
    print('here done too')
    print('==========================|{}|======================'.format(pred))
    print(Apps[pred])
    