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
from sklearn import model_selection
from sklearn.linear_model import LogisticRegression
from sklearn import preprocessing
import pickle

file_path = ''
csvheader = ''
Apps = ['Anghami','Youtube','Instagram','Skype', 'Googlemeet', 'Twitch', 'Others']

mean = [11362.211231423087, 398.89441861278215, 22.09539002511236, 284.05528078558115, 12577.307558922925, 3.547282107276937, 34639.16918310919, 888.1838252079312, 34639.16918310919, 17.901480618554604, 3.811080413357769, 0.2165924781471634, 0.1478279624863453, 0.03289834347535715, 82796.22567390995, 6619.079343793164, 519.2686478290298, 2069.905641847666, 44179690067.06172, 1449325395.9034493, 923745892.6211771, 1104492251.6766684, 44596612706.53647, 1449341074.636649, 923117261.2779014, 1114915317.6631668, 4.993227448918329, 4079.817898654657, 412.7634717670069, 65.79716221009642, 166.3885522312351, 12193.434840558006, 153.7620994193944, 34440.83178843396, 1596.186724286402, 34440.83178843396, 7.8105095759616585, 1.5041180896838444, 0.21652531319489912, 0.07268267332305574, 0.017938556218252282, 77789.4765390326, 6582.471600206301, 4973.057144704797, 4973.477083504164, 23091155374.32402, 1341145034.171547, 1341144945.2980285, 1341144995.9778802, 22462640591.24535, 1305880953.7845, 1305811543.2466037, 1305232859.5982168, 4.993227448918329, 13714.112081856778, 492.4986838244566, 634.7127262678391, 541.2517087428811, 12434.997019054546, 827.1173212486211, 33758.77416308759, 2224.0282257207664, 33758.77416308759, 10.764005288507018, 3.101998571142023, 0.005409318597495089, 0.07676203085800912, 0.022301195522353677, 12390.21778190888, 3368.2796071671396, 539.4212786883967, 569.8228734980119, 30922636256.98162, 1305854197.3980095, 1305842032.1744857, 1305848809.1800985, 31886990655.44518, 1341145006.0214403, 1341135015.6846387, 1341139178.7028515]
        
std = [11362.211231423087, 398.89441861278215, 22.09539002511236, 284.05528078558115, 12577.307558922925, 3.547282107276937, 34639.16918310919, 888.1838252079312, 34639.16918310919, 17.901480618554604, 3.811080413357769, 0.2165924781471634, 0.1478279624863453, 0.03289834347535715, 82796.22567390995, 6619.079343793164, 519.2686478290298, 2069.905641847666, 44179690067.06172, 1449325395.9034493, 923745892.6211771, 1104492251.6766684, 44596612706.53647, 1449341074.636649, 923117261.2779014, 1114915317.6631668, 4.993227448918329, 4079.817898654657, 412.7634717670069, 65.79716221009642, 166.3885522312351, 12193.434840558006, 153.7620994193944, 34440.83178843396, 1596.186724286402, 34440.83178843396, 7.8105095759616585, 1.5041180896838444, 0.21652531319489912, 0.07268267332305574, 0.017938556218252282, 77789.4765390326, 6582.471600206301, 4973.057144704797, 4973.477083504164, 23091155374.32402, 1341145034.171547, 1341144945.2980285, 1341144995.9778802, 22462640591.24535, 1305880953.7845, 1305811543.2466037, 1305232859.5982168, 4.993227448918329, 13714.112081856778, 492.4986838244566, 634.7127262678391, 541.2517087428811, 12434.997019054546, 827.1173212486211, 33758.77416308759, 2224.0282257207664, 33758.77416308759, 10.764005288507018, 3.101998571142023, 0.005409318597495089, 0.07676203085800912, 0.022301195522353677, 12390.21778190888, 3368.2796071671396, 539.4212786883967, 569.8228734980119, 30922636256.98162, 1305854197.3980095, 1305842032.1744857, 1305848809.1800985, 31886990655.44518, 1341145006.0214403, 1341135015.6846387, 1341139178.7028515]

scaler_scale = np.array([11362.211231422996, 398.89441861283774, 22.095390025104336, 284.0552807855749, 12577.307558922945, 3.5472821072767426, 34639.16918310936, 888.1838252079325, 34639.16918310936, 17.90148061856373, 3.8110804133574883, 0.2165924781472898, 0.14782796248648586, 0.03289834347536707, 82796.22567388852, 6619.079343792069, 519.2686478290484, 2069.9056418472132, 4.9932274489178425, 4079.8178986551316, 412.7634717669484, 65.79716221011732, 166.38855223128988, 12193.434840557951, 153.76209941939365, 34440.8317884339, 1596.1867242864034, 34440.8317884339, 7.810509575960372, 1.5041180896852326, 0.21652531319481672, 0.07268267332304165, 0.017938556218255484, 77789.47653902286, 6582.471600207786, 4973.057144705109, 4973.477083503627, 4.9932274489178425, 13714.112081856476, 492.4986838246197, 634.7127262679145, 541.2517087428342, 12434.997019054508, 827.1173212486167, 33758.774163087684, 2224.0282257207746, 33758.774163087684, 10.764005288509718, 3.1019985711422766, 0.07676203085795044, 0.022301195522360817, 12390.21778191195, 3368.2796071663765, 539.4212786882188, 569.822873498086])

scaler_mean = np.array([27775.95069423108, 1274.727560022824, 63.53892522202227, 694.3987673557768, 1463.820963741697, 0.5987108386278511, 4499.870594074164, 115.38129728395292, 4499.870594074164, 28.902793018186074, 1.642891629723917, 0.0029407891849185797, 0.011060878725365405, 0.0006144932625203002, 88518.24124712871, 5299.442420518222, 380.05824518281173, 2212.9560311782175, 18.1122913283296, 2893.485449677391, 297.8026159856033, 70.82437197325491, 139.92092318269204, 1471.8547545844672, 15.20822551696989, 4397.81761711949, 231.18313803077834, 4397.81761711949, 11.97720522611889, 0.3038230259403942, 0.0029115276009890416, 0.005310977483211166, 0.00029261583929538105, 79403.155847196, 5278.526532941228, 4887.5912303032965, 4939.997439025108, 21.8877086716704, 24882.46524455369, 1192.3349573512414, 750.1328475910401, 1069.9965671174546, 1505.6343441038823, 32.49852237573886, 4249.007983383078, 261.60412461331754, 4249.007983383078, 16.925587792067184, 1.3390686037835229, 0.005749901242154238, 0.0003218774232249192, 9115.0853999327, 621.2149409647544, 388.86908367349923, 401.0085414894636])

scaler_var = np.array([129099844.06747487, 159116.75720047383, 488.20626036148013, 80687.40254217178, 158188665.43174025, 12.583210348605727, 1199872041.6960735, 788870.5073609952, 1199872041.6960735, 320.4630083368129, 14.524333917077083, 0.04691230158998421, 0.021853106492905873, 0.0010823010034232271, 6855214985.841476, 43812211.359414846, 269639.92861820833, 4284509.366150923, 24.932320356626587, 16644914.086186774, 170373.6836251044, 4329.2665549044905, 27685.150313624676, 148679853.21093252, 23642.783217859494, 1186170894.2791986, 2547812.058788159, 1186170894.2791986, 61.00405983616867, 2.262371227718354, 0.04688321125411347, 0.005282771001383991, 0.00032179179919551254, 6051202660.215188, 43328932.36754205, 24731297.36450253, 24735474.300135743, 24.932320356626587, 188076870.19372174, 242554.95356898272, 402860.2448864485, 292953.4122170379, 154629150.86389452, 684123.0631094873, 1139654832.9943566, 4946301.548802697, 1139654832.9943566, 115.86380985106517, 9.622395135368725, 0.005892409381436936, 0.0004973433217265661, 153517496.68320706, 11345307.512052879, 290975.31590163294, 324698.10716161574])

ignored_cols = [0, 1, 6, 7, 13, 23, 24, 25, 26, 27, 28, 29, 30, 36, 37, 43, 53, 54, 55, 56, 57, 58, 59, 60, 66, 67, 73, 76, 83, 84, 85, 86, 87, 88, 89, 90]

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

""" extracting flow features from packets """    

def extract_flows(packets, limit=0):

    flows = []
    flow_in_process, l = packets[0][0], 0

    flow = Flow(flow_in_process)

    for packet in packets:

        if(packet[0] == flow_in_process):
            if(not limit or l < limit):

                flow.global_features.update_values(packet)

                if packet[1] == 'F':
                    flow.forward_features.update_values(packet)

                elif packet[1] == 'B':
                    flow.backward_features.update_values(packet)   

                l += 1    

        else:
            if((flow.get_features()[1][0] > limit) or not limit):
                if ((flow.get_features()[2][0] > 1) and (flow.get_features()[3][0] > 1)):
                    flows.append(flow)

            flow_in_process,l = packet[0], 0
            flow = Flow(flow_in_process)
            flow.global_features.update_values(packet)

            if packet[1] == 'F':
                flow.forward_features.update_values(packet)
                
            elif packet[1] == 'B':
                flow.backward_features.update_values(packet)

    if ((flow.get_features()[1][0] > limit) or not limit):
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
    try:
        flows = extract_flows(packets)
    except Exception as e:
        print(e)
    flow = flows[0].get_features_flattened()
    flow = np.delete(flow, ignored_cols).astype(float).reshape(1, -1)
    # flow = transform(flow.reshape(1,1,80))
    
    # output = model(flow.float().reshape(1,80))

    # _, pred = torch.max(output, 1)
    
    
    
    
    pred = loaded_model.predict(flow)
    # print(pred)
    print('========|{}|========'.format(Apps[pred[0]]))


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
    if len(sys.argv) == 1 :
    # print('[*] IN: LiveFeatureExtractor \n\n')
    # model = Network()
    # model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict8.pt'))
    # transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean, std)])
    # print('[1] model and transform has been created !\n\n')
    # print('[*] OUT: LiveFeatureExtractor')
        loaded_model = pickle.load(open('models/NetDump_RandomForest.sav', 'rb'))
        print('[*] model has been created')
    # scaler = preprocessing.StandardScaler()
    # scaler.scale_ = scaler_scale
    # scaler.mean_ = scaler_mean
    # scaler.var_ = scaler_var
    # scaler.n_features_in_ = 55
    # print('[*] params has been given to the scaler')


# ******************************************
# __main__ in this script is used only for # 
# testing!                                 #
# ******************************************

if __name__== '__main__':
    # print('[*] IN: __main__\n\n')
    
    # model = Network()
    # model.load_state_dict(torch.load('/home/sysbot/Desktop/NetDump/models/NetDum_MLP_statedict.pt'))
    
    
    # flow = np.array(['6-172.217.19.46-39850-443', '40.0', '30273.0', '1470.0', '52.0', '756.825', '15950081497847.87', '15950081534953.33', '31197.720703125', '0.130859375', '37105.4609375', '951.4220753205128', '37105.4609375', '0.0', '0.0', '11.0', '0.0', '0.0', '0.0', '49452.0', '1987.0', '983.0', '1236.3', '146059002688.0', '3839651276.0', '3260679688.0', '3651475067.2', '137953652989.0', '3839649858.0', '3260680331.0', '3448841324.725', '13.0', '2719.0', '1413.0', '52.0', '209.15384615384616', '15950081497847.87', '15950081534902.361', '31651.2109375', '0.9609375', '37054.4921875', '3087.8743489583335', '37054.4921875', '0.0', '0.0', '3.0', '0.0', '0.0', '0.0', '22669.0', '1987.0', '1603.0', '1743.7692307692307', '42388856299.0', '3260681731.0', '3260679688.0', '3260681253.769231', '49915253213.0', '3839649858.0', '3839626544.0', '3839634862.5384617', '27.0', '27554.0', '1470.0', '52.0', '1020.5185185185185', '15950081498919.46', '15950081534953.33', '32158.048828125', '0.130859375', '36033.869140625', '1385.9180438701924', '36033.869140625', '0.0', '0.0', '8.0', '0.0', '0.0', '0.0', '26783.0', '994.0', '983.0', '991.9629629629629', '103670146389.0', '3839651276.0', '3839626544.0', '3839635051.4444447', '88038399776.0', '3260681731.0', '3260680331.0', '3260681473.185185'])
    # flow = np.delete(flow, ignored_cols)
    
    # transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize(mean, std)])
    
    # flow = flow.reshape(1,1, 55).astype(float)
    # print('[1] reshape done !')
    
    # flow = transform(flow)
    # print('[2] transform done !')
    
    
    # output = model(flow.float().reshape(1,55))
    # print('[3] output done !')

    # _, pred = torch.max(output, 1)
    # print('[4] prediction done !')

    # print('==========================|{}|======================\n\n'.format(Apps[pred]))
    # print('[*] OUT: __main__')
    loaded_model = pickle.load(open('models/NetDump_OneVsOne.sav', 'rb'))
    # scaler = preprocessing.StandardScaler()
    # scaler.scale_ = scaler_scale
    # scaler.mean_ = scaler_mean
    # scaler.var_ = scaler_var
    # scaler.n_features_in_ = 55
    flow = np.array(['6-172.217.21.4-48428-443', '40.0', '5369.0', '598.0', '52.0', '134.225', '15951861470046.488', '15951861477553.092', '4858.08984375', '0.1015625', '7506.603515625', '192.47701322115384', '7506.603515625', '0.0', '0.0', '25.0', '0.0', '0.0', '0.0', '96360.0', '4070.0', '1050.0', '2409.0', '149711829670.0', '4082140504.0', '3465150003.0', '3742795741.75', '152179789573.0', '4082140504.0', '3465150003.0', '3804494739.325', '18.0', '2561.0', '598.0', '52.0', '142.27777777777777', '15951861470046.488', '15951861477553.092', '5212.08984375', '0.41015625', '7506.603515625', '441.56491268382354', '7506.603515625', '0.0', '0.0', '9.0', '0.0', '0.0', '0.0', '73260.0', '4070.0', '4070.0', '4070.0', '73478518291.0', '4082140504.0', '4082138879.0', '4082139905.0555553', '62372707444.0', '3465151667.0', '3465150003.0', '3465150413.5555553', '22.0', '2808.0', '455.0', '52.0', '127.63636363636364', '15951861470398.14', '15951861477551.752', '5368.7890625', '0.25', '7153.611328125', '340.64815848214283', '7153.611328125', '0.0', '0.0', '16.0', '0.0', '0.0', '0.0', '23100.0', '1050.0', '1050.0', '1050.0', '76233311379.0', '3465151636.0', '3465150003.0', '3465150517.2272725', '89807082129.0', '4082140504.0', '4082139156.0', '4082140096.7727275'])
    flow = np.delete(flow, ignored_cols).astype(float).reshape(1, -1)
    # scaler.transform(flow)
    result = loaded_model.predict(flow.reshape(1,-1))
    print(Apps[result[0]])