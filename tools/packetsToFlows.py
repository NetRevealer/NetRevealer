import sys
sys.path.append('./livefeaturecollector')
from LiveFeatureExtractor import *

file_path = ''
csvheader = ['flowID', 'nbpackets', 'totallen', 'maxlen', 'minlen', 'meanlen', 'FistTS', 'LastTS'
, 'maxIAT', 'minIAT', 'totalIAT', 'meanIAT', 'duration', 'urgCount', 'ackCount', 'pshCount'
, 'rstCount', 'synCount', 'finCount', 'F_nbpackets', 'F_totallen', 'F_maxlen', 'F_minlen'
, 'F_meanlen', 'F_FistTS', 'F_LastTS', 'F_maxIAT', 'F_minIAT', 'F_totalIAT', 'F_meanIAT'
, 'F_duration', 'F_urgCount', 'F_ackCount', 'F_pshCount', 'F_rstCount', 'F_synCount', 'F_finCount'
, 'B_nbpackets', 'B_totallen', 'B_maxlen', 'B_minlen', 'B_meanlen', 'B_FistTS', 'B_LastTS'
, 'B_maxIAT', 'B_minIAT', 'B_totalIAT', 'B_meanIAT', 'B_duration', 'B_urgCount', 'B_ackCount'
, 'B_pshCount', 'B_rstCount', 'B_synCount', 'B_finCount']

try:
    file_path = sys.argv[1]
except IndexError:
    print('[!] Please enter the path of the captured packets (csv) you want extract flows from.')
    exit()


def main():
    packs = extract_packets(file_path)
    flows = extract_flows(packs)
    write_output(file_path, flows, csvheader)

if __name__ == '__main__':
    main()