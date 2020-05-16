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

    def get_features(self):

        return [self.flowid,self.nbpackets,self.totallen,self.maxlen,self.minlen,self.meanlen]


    def update_features(self,packet):
        self.nbpackets += 1
        self.totallen += packet[-1]

        if packet[-1] > self.maxlen :
            self.maxlen = packet[-1]

        if packet[-1] < self.minlen :
            self.minlen = packet[-1]

        self.meanlen = self.totallen / self.nbpackets
             
""" extracting packets from raw data """


def extract_packets(file_path):
    
    with open(file_path,"r") as data :

        raw_packets = data.read().splitlines()
        raw_packets = [item for item in raw_packets if len(item) > 1]
        packets = []

        for pack in raw_packets:
            values = pack.split(",")
            packet = ["-".join(values[0:3]+values[4:6]),int(values[0]),int(values[3])]
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
    pass


if __name__ == "__main__":
    main()