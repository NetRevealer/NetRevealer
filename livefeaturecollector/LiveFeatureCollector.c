/**********************************************************************
*
*
* Live feature extractor for extracting features from network
* packets in online mode then pass these features to the deep
* learning model to make prediction then classify the packet
* into its application class (Video, Chat ...)
*
*
*
**********************************************************************/


// 
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> 
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ether.h> 
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

/* tcpdump header (ether.h) defines ETHER_HDRLEN) */
#ifndef ETHER_HDRLEN 
#define ETHER_HDRLEN 14
#endif


u_int16_t handle_ethernet(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet);
u_char* handle_IP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet);
u_char* handle_TCP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet);
u_char* handle_UDP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet);
FILE *output_file;

// control signals : Ctrl + c
void sigintHandler(int sig_num) 
{ 
    /* Reset handler to catch SIGINT next time. 
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler); 
    printf("[*] Exiting live feature collector using Ctrl+C \n");
    printf("[*] Flushing data to the output file \n");
    fflush(output_file);
    fclose(output_file);
    exit(0);
} 

/* looking at ethernet headers */
void Jacket(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    u_int16_t type = handle_ethernet(args,pkthdr,packet);

    if(type == ETHERTYPE_IP){
        /* handle IP packet */
        handle_IP(args,pkthdr,packet);
        fprintf(output_file,"\n");

    }else if(type == ETHERTYPE_ARP){
        /* handle arp packet */
    }else if(type == ETHERTYPE_REVARP){
        /* handle reverse arp packet */
    }
    
}

/* handle ethernet packets when captured.
 */
u_int16_t handle_ethernet(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    u_int caplen = pkthdr->caplen;
    u_int length = pkthdr->len;
    struct ether_header *eptr;  /* net/ethernet.h */
    u_short ether_type;

    if (caplen < ETHER_HDRLEN){
        fprintf(stdout,"Packet length less than ethernet header length\n");
        return -1;
    }

    /* lets start with the ether header... */
    eptr = (struct ether_header *) packet;
    ether_type = ntohs(eptr->ether_type);

    /* Lets print SOURCE DEST TYPE LENGTH */
    printf("----------------------------------------------------------------------------\n");
    fprintf(stdout,"ETH: ");
    fprintf(stdout,"%s ", ether_ntoa((struct ether_addr*)eptr->ether_shost));
    fprintf(stdout,"%s ", ether_ntoa((struct ether_addr*)eptr->ether_dhost));

    /* check if the packet encaplsulate an ip packet or arp packet or other */
    if (ether_type == ETHERTYPE_IP){
        fprintf(stdout,"(IP)");
    }else  if (ether_type == ETHERTYPE_ARP){
        fprintf(stdout,"(ARP)");
    }else  if (eptr->ether_type == ETHERTYPE_REVARP){
        fprintf(stdout,"(RARP)");
    }else {
        fprintf(stdout,"(?)");
    }
    fprintf(stdout," %d\n",length);
    
    return ether_type;
}

/* looking at Internet Protocol headers */
u_char* handle_IP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    const struct ip* iph;
    u_int length = pkthdr->len;
    u_int hlen,off,version;
    int i;
    u_int8_t protocol_id = 0;
    char *protocol = "unknown" ; /* for future use to handle tcp and udp headers ... */

    int len;

    /* jump pass the ethernet header */
    iph = (struct ip*)(packet + sizeof(struct ether_header));
    protocol_id = iph->ip_p;
    length -= sizeof(struct ether_header); 

    /* check to see we have a packet of valid length */
    if (length < sizeof(struct ip)){
        printf("truncated ip %d",length);
        return NULL;
    }

    len     = ntohs(iph->ip_len);
    hlen    = iph->ip_hl; /* header length */
    version = iph->ip_v;/* ip version */
    
    /* check version */
    if(version != 4){
      fprintf(stdout,"Unknown version %d\n",version);
      return NULL;
    }

    /* check header length */
    if(hlen < 5 ){
        fprintf(stdout,"bad-hlen %d \n",hlen);
    }

    /* see if we have as much packet as we should */
    if(length < len){
        printf("\ntruncated IP - %d bytes missing\n",len - length);
    }

    /* Check to see if we have the first fragment */
    off = ntohs(iph->ip_off);
    /* aka no 1's in first 13 bits */
    if((off & 0x1fff) == 0 ){
        /* print SOURCE DESTINATION hlen version len offset */
        fprintf(stdout,"IP: ");
        fprintf(stdout,"%s ", inet_ntoa(iph->ip_src));
        fprintf(stdout,"%s [hdr len %d] [version %d] [len %d] [off %d]\n", inet_ntoa(iph->ip_dst), hlen,version,len,off);
        // printf("Protocol type : %d\n", protocol_id);
        fprintf(output_file,"%d,%s,",protocol_id,inet_ntoa(iph->ip_src));
        fprintf(output_file,"%s,%d,",inet_ntoa(iph->ip_dst),len);
    }
    if (protocol_id == 6){
        handle_TCP(args,pkthdr,packet);
    } else if (protocol_id == 17){
        handle_UDP(args,pkthdr,packet);
    }
    

    return NULL;
}

/**
 * 
 * handle tcp packets
 * 
*/

u_char* handle_TCP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    const struct tcphdr* tcp;
    
    tcp = (struct tcphdr*)(packet + sizeof(struct ip) + sizeof(struct ether_header));
    fprintf(stdout,"TCP: [seq %u] [ack %u] ", ntohl(tcp->th_seq), ntohl(tcp->th_ack));
    fprintf(stdout,"[src port %u] [dst port %u] ", ntohs(tcp->th_sport), ntohs(tcp->th_dport));
    fprintf(stdout,"[win %d] ", ntohs(tcp->th_win));
    fprintf(stdout,"[flags: %c%c%c%c%c%c]\n",
                (tcp->urg ? 'U' : '*'),
                (tcp->ack ? 'A' : '*'),
                (tcp->psh ? 'P' : '*'),
                (tcp->rst ? 'R' : '*'),
                (tcp->syn ? 'S' : '*'),
                (tcp->fin ? 'F' : '*'));

    fprintf(output_file,"%d,%d", ntohs(tcp->th_sport), ntohs(tcp->th_dport));            
}


/**
 * handle udp packets
*/
 u_char* handle_UDP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    const struct udphdr* udp;
    
    udp = (struct udphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
    fprintf(stdout, "UDP: [src port %d] [dst port %d]", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
    fprintf(stdout, " [datagram len %d] [chksum %d]\n" , ntohs(udp->uh_ulen), ntohs(udp->uh_ulen));

    fprintf(output_file,"%d,%d", ntohs(udp->uh_sport), ntohs(udp->uh_dport));

 }



int main(int argc,char **argv){
    char dev[] = "wlp6s0";		/* Device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* liveCapture;
    struct bpf_program fp;      /* hold compiled program     */
    bpf_u_int32 maskp;          /* subnet mask               */
    bpf_u_int32 netp;           /* ip                        */
    u_char* args = NULL;

    /* Options must be passed in as a string */
    if(argc < 2){ 
        fprintf(stdout,"Usage: %s numpackets \"options\"\n",argv[0]);
        return 0;
    }

    signal(SIGINT, sigintHandler);
    output_file = fopen("out.csv","a");
    // fprintf(output_file,"protocol,ip dst,ip src,len,flags,");

    /* ask pcap for the network address and mask of the device */
    pcap_lookupnet(dev,&netp,&maskp,errbuf);

    /* open device for reading. NOTE: defaulting to
     * promiscuous mode*/
    liveCapture = pcap_open_live(dev,BUFSIZ,1,-1,errbuf);
    if(liveCapture == NULL){
        printf("pcap_open_live(): %s\n",errbuf);
        exit(1);
    }


    if(argc > 2){
        /* Lets try and compile the program.. non-optimized */
        if(pcap_compile(liveCapture,&fp,argv[2],0,netp) == -1){
            fprintf(stderr, "%s\n", pcap_geterr(liveCapture));
            exit(1);
        }

        /* set the compiled program as the filter */
        if(pcap_setfilter(liveCapture,&fp) == -1){
            fprintf(stderr,"Error setting filter\n");
            exit(1);
        }
    }

    /* ... and loop */ 
    pcap_loop(liveCapture,atoi(argv[1]),Jacket,args);

    fprintf(stdout,"\nfinished\n");
    fflush(output_file);
    fclose(output_file);
    return 0;
}