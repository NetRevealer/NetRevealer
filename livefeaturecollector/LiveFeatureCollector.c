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
#include <string.h>
#include <unistd.h>
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
#include <sys/time.h>

#include <stdbool.h>


/* tcpdump header (ether.h) defines ETHER_HDRLEN) */
#ifndef ETHER_HDRLEN 
#define ETHER_HDRLEN 14
#endif


u_int16_t handle_ethernet(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet);
u_char* handle_IP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet);
u_char* handle_TCP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet,bool direction);
u_char* handle_UDP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet,bool direction);

FILE *output_file;

char host[256];
struct hostent *host_entry;
char hostIP[18];
int hostname;

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

    struct timeval timestamp = pkthdr->ts;

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
    fprintf(stdout,"%ld.%06ld\n", timestamp.tv_sec, timestamp.tv_usec);
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
    bool is_forward; // true for forward and false for backword packets
    int cmp; 

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

    /*determine the direction of the packet*/

    fprintf(stdout,"%s, ",hostIP);
    cmp = strcmp(hostIP,inet_ntoa(iph->ip_src));
    if(cmp == 0){
        is_forward = true;
    } else{
        is_forward = false;
    }

    /* Check to see if we have the first fragment */
    off = ntohs(iph->ip_off);
    /* aka no 1's in first 13 bits */
    if((off & 0x1fff) == 0 ){
        /* print SOURCE DESTINATION hlen version len offset */
        fprintf(stdout,"IP: ");
        fprintf(stdout,"%s ", inet_ntoa(iph->ip_src));
        fprintf(stdout,"%s [hdr len %d] [version %d] [len %d] [off %d]\n", inet_ntoa(iph->ip_dst), hlen,version,len,off);

        if (is_forward){
            fprintf(output_file,"F,%d,%s,%d,",protocol_id,inet_ntoa(iph->ip_dst),len);
        } else{
            fprintf(output_file,"B,%d,%s,%d,",protocol_id,inet_ntoa(iph->ip_src),len);
        }
    }
    if (protocol_id == 6){
        handle_TCP(args,pkthdr,packet,is_forward);
    } else if (protocol_id == 17){
        handle_UDP(args,pkthdr,packet,is_forward);
    }
    

    return NULL;
}

/**
 * 
 * handle tcp packets
 * 
*/

u_char* handle_TCP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet,bool is_forward){
    const struct tcphdr* tcp;
    
    struct timeval timestamp = pkthdr->ts;

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

    if(is_forward){
        fprintf(output_file,"%d,%d,", ntohs(tcp->th_sport), ntohs(tcp->th_dport));
    } else{
        fprintf(output_file,"%d,%d,", ntohs(tcp->th_dport), ntohs(tcp->th_sport));
    }
    fprintf(output_file,"%ld.%06ld", timestamp.tv_sec, timestamp.tv_usec);            
}


/**
 * handle udp packets
*/
 u_char* handle_UDP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet,bool is_forward){
    const struct udphdr* udp;
    
    struct timeval timestamp = pkthdr->ts;
    
    udp = (struct udphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
    fprintf(stdout, "UDP: [src port %d] [dst port %d]", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
    fprintf(stdout, " [datagram len %d] [chksum %d]\n" , ntohs(udp->uh_ulen), ntohs(udp->uh_ulen));
    if(is_forward){
        fprintf(output_file,"%d,%d,", ntohs(udp->uh_sport), ntohs(udp->uh_dport));
    } else{
        fprintf(output_file,"%d,%d,", ntohs(udp->uh_dport), ntohs(udp->uh_sport));
    }
    fprintf(output_file,"%ld.%06ld", timestamp.tv_sec, timestamp.tv_usec);

 }



int main(int argc,char **argv){
    char dev[] = "wlp6s0";		/* Device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* liveCapture;
    struct bpf_program fp;      /* hold compiled program     */
    bpf_u_int32 maskp;          /* subnet mask               */
    bpf_u_int32 netp;           /* ip                        */
    u_char* args = NULL;
    
    hostname = gethostname(host, sizeof(host)); //find the host name
    host_entry = gethostbyname(host); //find host information
    strcpy(hostIP,inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]))); //Convert into IP string
    
    

    /* Options must be passed in as a string */
    if(argc < 2){ 
        fprintf(stdout,"Usage: %s numpackets \"options\"\n",argv[0]);
        return 0;
    }

    signal(SIGINT, sigintHandler);
    output_file = fopen("out.csv","a");
    

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