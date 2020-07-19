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
Apps = ['Anghami','Youtube','Instagram','Skype', 'Googlemeet', 'Twitch', 'Others']

mean = [11362.211231423087, 398.89441861278215, 22.09539002511236, 284.05528078558115, 12577.307558922925, 3.547282107276937, 34639.16918310919, 888.1838252079312, 34639.16918310919, 17.901480618554604, 3.811080413357769, 0.2165924781471634, 0.1478279624863453, 0.03289834347535715, 82796.22567390995, 6619.079343793164, 519.2686478290298, 2069.905641847666, 44179690067.06172, 1449325395.9034493, 923745892.6211771, 1104492251.6766684, 44596612706.53647, 1449341074.636649, 923117261.2779014, 1114915317.6631668, 4.993227448918329, 4079.817898654657, 412.7634717670069, 65.79716221009642, 166.3885522312351, 12193.434840558006, 153.7620994193944, 34440.83178843396, 1596.186724286402, 34440.83178843396, 7.8105095759616585, 1.5041180896838444, 0.21652531319489912, 0.07268267332305574, 0.017938556218252282, 77789.4765390326, 6582.471600206301, 4973.057144704797, 4973.477083504164, 23091155374.32402, 1341145034.171547, 1341144945.2980285, 1341144995.9778802, 22462640591.24535, 1305880953.7845, 1305811543.2466037, 1305232859.5982168, 4.993227448918329, 13714.112081856778, 492.4986838244566, 634.7127262678391, 541.2517087428811, 12434.997019054546, 827.1173212486211, 33758.77416308759, 2224.0282257207664, 33758.77416308759, 10.764005288507018, 3.101998571142023, 0.005409318597495089, 0.07676203085800912, 0.022301195522353677, 12390.21778190888, 3368.2796071671396, 539.4212786883967, 569.8228734980119, 30922636256.98162, 1305854197.3980095, 1305842032.1744857, 1305848809.1800985, 31886990655.44518, 1341145006.0214403, 1341135015.6846387, 1341139178.7028515]
        
std = [11362.211231423087, 398.89441861278215, 22.09539002511236, 284.05528078558115, 12577.307558922925, 3.547282107276937, 34639.16918310919, 888.1838252079312, 34639.16918310919, 17.901480618554604, 3.811080413357769, 0.2165924781471634, 0.1478279624863453, 0.03289834347535715, 82796.22567390995, 6619.079343793164, 519.2686478290298, 2069.905641847666, 44179690067.06172, 1449325395.9034493, 923745892.6211771, 1104492251.6766684, 44596612706.53647, 1449341074.636649, 923117261.2779014, 1114915317.6631668, 4.993227448918329, 4079.817898654657, 412.7634717670069, 65.79716221009642, 166.3885522312351, 12193.434840558006, 153.7620994193944, 34440.83178843396, 1596.186724286402, 34440.83178843396, 7.8105095759616585, 1.5041180896838444, 0.21652531319489912, 0.07268267332305574, 0.017938556218252282, 77789.4765390326, 6582.471600206301, 4973.057144704797, 4973.477083504164, 23091155374.32402, 1341145034.171547, 1341144945.2980285, 1341144995.9778802, 22462640591.24535, 1305880953.7845, 1305811543.2466037, 1305232859.5982168, 4.993227448918329, 13714.112081856778, 492.4986838244566, 634.7127262678391, 541.2517087428811, 12434.997019054546, 827.1173212486211, 33758.77416308759, 2224.0282257207664, 33758.77416308759, 10.764005288507018, 3.101998571142023, 0.005409318597495089, 0.07676203085800912, 0.022301195522353677, 12390.21778190888, 3368.2796071671396, 539.4212786883967, 569.8228734980119, 30922636256.98162, 1305854197.3980095, 1305842032.1744857, 1305848809.1800985, 31886990655.44518, 1341145006.0214403, 1341135015.6846387, 1341139178.7028515]

ignored_cols = [0, 1, 6, 7, 13, 36, 37, 43, 66, 67, 73]

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

    def get_features_flattened(self):
        flow = np.array([])
        global_features = np.array([self.flowid])
        global_features = np.append(global_features, self.global_features.get_values())
        forward_features = np.array(self.forward_features.get_values())
        backward_features = np.array(self.backward_features.get_values())
        flow = np.append(flow, global_features)
        flow = np.append(flow, forward_features)
        flow = np.append(flow, backward_features)

        return flow
             
""" extracting packets from raw data """


def extract_packets(file_path):
    
    with open(file_path,"r") as data :

        raw_packets = data.read().splitlines()
        raw_packets = [item for item in raw_packets if len(item) > 1]
        packets = []

        for pack in raw_packets:
            values = pack.split(",")
            try:
                packet = ["-".join(values[1:3]+values[4:6]), values[0], int(values[3]), 
                    float(values[6]),values[7],int(values[8]), int(values[9]), 
                    int(values[10])]

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
    raw_packets = flow.splitlines()
    raw_packets = [item for item in raw_packets if len(item) > 1]
    packets = []

    for pack in raw_packets:
        values = pack.split(",")
        try:
            packet = ["-".join(values[1:3]+values[4:6]), values[0], int(values[3]), 
                float(values[6]), values[7], int(values[8]), int(values[9]), 
                int(values[10])]
            
            packets.append(packet)
        except:
            pass
        
    packets.sort(key=lambda packet: packet[0])
    flows = extract_flows(packets)

    flow = flows[0].get_features_flattened()
    
    flow = np.delete(flow, ignored_cols).astype(float)
    flow = transform(flow.reshape(1,1,80))
    
    output = model(flow.float().reshape(1,80))

    _, pred = torch.max(output, 1)
    
    # print(pred)
    print('========|{}|========'.format(Apps[pred]))


class Network(nn.Module):
    def __init__(self):
        super().__init__()
        
        self.fc1 = nn.Linear(80, 64)
        self.fc2 = nn.Linear(64, 48)
        self.fc3 = nn.Linear(48, 32)
        self.fc4 = nn.Linear(32, 16)
        self.fc5 = nn.Linear(16, 7)
        
    def forward(self, x):
        # Pass the input tensor through each of our operations
        x = x.view(x.shape[0], -1)
        
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = F.relu(self.fc4(x))
        x = F.log_softmax(self.fc5(x), dim=1)
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
    print('[*] IN: LiveFeatureExtractor \n\n')
    model = Network()
    model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict8.pt'))
    transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean, std)])
    print('[1] model and transform has been created !\n\n')
    print('[*] OUT: LiveFeatureExtractor')


# ******************************************
# __main__ in this script is used only for # 
# testing!                                 #
# ******************************************

if __name__== '__main__':
    print('[*] IN: __main__\n\n')
    
    model = Network()
    model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict.pt'))
    
    
    flow = np.array(['6-172.217.19.46-39850-443', '40.0', '30273.0', '1470.0', '52.0', '756.825', '15950081497847.87', '15950081534953.33', '31197.720703125', '0.130859375', '37105.4609375', '951.4220753205128', '37105.4609375', '0.0', '0.0', '11.0', '0.0', '0.0', '0.0', '49452.0', '1987.0', '983.0', '1236.3', '146059002688.0', '3839651276.0', '3260679688.0', '3651475067.2', '137953652989.0', '3839649858.0', '3260680331.0', '3448841324.725', '13.0', '2719.0', '1413.0', '52.0', '209.15384615384616', '15950081497847.87', '15950081534902.361', '31651.2109375', '0.9609375', '37054.4921875', '3087.8743489583335', '37054.4921875', '0.0', '0.0', '3.0', '0.0', '0.0', '0.0', '22669.0', '1987.0', '1603.0', '1743.7692307692307', '42388856299.0', '3260681731.0', '3260679688.0', '3260681253.769231', '49915253213.0', '3839649858.0', '3839626544.0', '3839634862.5384617', '27.0', '27554.0', '1470.0', '52.0', '1020.5185185185185', '15950081498919.46', '15950081534953.33', '32158.048828125', '0.130859375', '36033.869140625', '1385.9180438701924', '36033.869140625', '0.0', '0.0', '8.0', '0.0', '0.0', '0.0', '26783.0', '994.0', '983.0', '991.9629629629629', '103670146389.0', '3839651276.0', '3839626544.0', '3839635051.4444447', '88038399776.0', '3260681731.0', '3260680331.0', '3260681473.185185'])
    flow = np.delete(flow, ignored_cols)
    
    transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean, std)])
    
    flow = flow.reshape(1,1, 55).astype(float)
    print('[1] reshape done !')
    
    flow = transform(flow)
    print('[2] transform done !')
    
    
    output = model(flow.float().reshape(1,55))
    print('[3] output done !')

    _, pred = torch.max(output, 1)
    print('[4] prediction done !')

    print('==========================|{}|======================\n\n'.format(Apps[pred]))
    print('[*] OUT: __main__')

    