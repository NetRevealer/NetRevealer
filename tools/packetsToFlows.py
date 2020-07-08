import sys
sys.path.append('./livefeaturecollector')
from LiveFeatureExtractor import *


file_path = ''
labeling = False
csvheader = ['flowID', 'nbpackets', 'totallen', 'maxlen', 'minlen', 'meanlen', 'FistTS'
, 'LastTS', 'maxIAT', 'minIAT', 'totalIAT', 'meanIAT', 'duration', 'urgCount', 'ackCount'
, 'pshCount', 'rstCount', 'synCount', 'finCount', 'totalwin', 'maxwin', 'minwin', 'meanwin'
, 'totalseq', 'maxseq', 'minseq', 'meanseq', 'totalack', 'maxack', 'minack', 'meanack'
, 'F_nbpackets', 'F_totallen', 'F_maxlen', 'F_minlen', 'F_meanlen', 'F_FistTS', 'F_LastTS'
, 'F_maxIAT', 'F_minIAT', 'F_totalIAT', 'F_meanIAT', 'F_duration', 'F_urgCount', 'F_ackCount'
, 'F_pshCount', 'F_rstCount', 'F_synCount', 'F_finCount', 'F_totalwin', 'F_maxwin', 'F_minwin'
, 'F_meanwin', 'F_totalseq', 'F_maxseq', 'F_minseq', 'F_meanseq', 'F_totalack', 'F_maxack'
, 'F_minack', 'F_meanack', 'B_nbpackets', 'B_totallen', 'B_maxlen', 'B_minlen', 'B_meanlen'
, 'B_FistTS', 'B_LastTS', 'B_maxIAT', 'B_minIAT', 'B_totalIAT', 'B_meanIAT', 'B_duration'
, 'B_urgCount', 'B_ackCount', 'B_pshCount', 'B_rstCount', 'B_synCount', 'B_finCount'
, 'B_totalwin', 'B_maxwin', 'B_minwin', 'B_meanwin', 'B_totalseq', 'B_maxseq', 'B_minseq'
, 'B_meanseq', 'B_totalack', 'B_maxack', 'B_minack', 'B_meanack']



file_path = sys.argv[1]
output_file = sys.argv[2]

if len(sys.argv) > 3:
    labeling = True



def main():
    packs = extract_packets(file_path)
    if (labeling):
        flows = flow_labeling(packs)
        csvheader.append('Label')
        label = sys.argv[3]
        write_output(output_file, flows, csvheader, label)

    else:
        flows = extract_flows(packs)
        write_output(output_file, flows, csvheader)

if __name__ == '__main__':
    main()