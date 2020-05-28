""" Extract traffic flow features from a CSV file """

import math 

class Flow():

    def __init__(self,flowid):

        self.flowid = flowid
        self.nbpackets = 0
        self.totallen = 0
        self.maxlen = 0
        self.minlen = math.inf
        self.meanlen = 0
        self.FistTS = 0
        self.LastTS = 0
        self.maxIAT = 0
        self.minIAT = math.inf
        self.totalIAT = 0
        self.meanIAT = 0
        self.duration = 0

    def get_features(self):

        return [self.flowid, self.nbpackets, self.totallen, self.maxlen, self.minlen, self.meanlen,
                self.FistTS, self.LastTS, self.maxIAT, self.minIAT, self.totalIAT, self.meanIAT, self.duration]


    def update_features(self,packet):
        self.nbpackets += 1
        self.totallen += packet[-2]

        self.maxlen = max(packet[-2], self.maxlen)

        self.minlen = min(packet[-2], self.minlen)

        self.meanlen = self.totallen / self.nbpackets

        if self.nbpackets == 1 :
            self.FistTS = packet[-1]
            
        else:
            IAT = packet[-1] - self.LastTS

            self.totalIAT += IAT

            self.maxIAT = max(IAT, self.maxIAT)

            self.minIAT = min(IAT, self.minIAT)

            self.meanIAT = self.totalIAT / (self.nbpackets-1)

        self.LastTS = packet[-1]
        self.duration = self.LastTS - self.FistTS    


             
""" extracting packets from raw data """


def extract_packets(file_path):
    
    with open(file_path,"r") as data :

        raw_packets = data.read().splitlines()
        raw_packets = [item for item in raw_packets if len(item) > 1]
        packets = []

        for pack in raw_packets:
            values = pack.split(",")
            packet = ["-".join(values[0:3]+values[4:6]),int(values[0]),int(values[3]),float(values[-1])]
            packets.append(packet)
        packets.sort(key=lambda packet: packet[0])    

    return packets

""" extracting flow features from packets """    

def extract_flows(packets):

    flows = []
    flow_in_process = packets[0][0]

    flow = Flow(flow_in_process)

    for packet in packets:

        if(packet[0] == flow_in_process):
            flow.update_features(packet)

        else:
            flows.append(flow)

            flow_in_process = packet[0]
            flow = Flow(flow_in_process)
            flow.update_features(packet)

    return flows

def main():
    packs = extract_packets("out.csv")
    flows = extract_flows(packs)
    for f in flows:
        x = f.get_features()
        print(*x)


if __name__ == "__main__":
    main()