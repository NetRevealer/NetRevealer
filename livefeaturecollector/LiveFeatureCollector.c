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

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> 
#include <net/ethernet.h>
#include <netinet/ether.h> 
#include <netinet/ip.h> 

/* tcpdump header (ether.h) defines ETHER_HDRLEN) */
#ifndef ETHER_HDRLEN 
#define ETHER_HDRLEN 14
#endif


u_int16_t handle_ethernet(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet);
u_char* handle_IP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*packet);

/* 
 *
 * declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 *
 */
struct ip_header{
	u_int8_t	ip_vhl;		/* header length, version */
    #define IP_V(ip)	(((ip)->ip_vhl & 0xf0) >> 4)
    #define IP_HL(ip)	((ip)->ip_vhl & 0x0f)
	u_int8_t	ip_tos;		/* type of service */
	u_int16_t	ip_len;		/* total length */
	u_int16_t	ip_id;		/* identification */
	u_int16_t	ip_off;		/* fragment offset field */
    #define	IP_DF 0x4000			/* dont fragment flag */
    #define	IP_MF 0x2000			/* more fragments flag */
    #define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	u_int8_t	ip_ttl;		/* time to live */
	u_int8_t	ip_p;		/* protocol */
	u_int16_t	ip_sum;		/* checksum */
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};

/* looking at ethernet headers */
void Jacket(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    u_int16_t type = handle_ethernet(args,pkthdr,packet);

    if(type == ETHERTYPE_IP){
        /* handle IP packet */
        handle_IP(args,pkthdr,packet);
    }else if(type == ETHERTYPE_ARP){
        /* handle arp packet */
    }else if(type == ETHERTYPE_REVARP){
        /* handle reverse arp packet */
    }
}

/* looking at Internet Protocol headers */
u_char* handle_IP(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    const struct ip_header* ip;
    u_int length = pkthdr->len;
    u_int hlen,off,version;
    int i;
    u_int8_t protocol_id = 0;
    char *protocol = "unknown" ; /* for future use to handle tcp and udp headers ... */

    int len;

    /* jump pass the ethernet header */
    ip = (struct ip_header*)(packet + sizeof(struct ether_header));
    protocol_id = ip->ip_p;
    length -= sizeof(struct ether_header); 

    /* check to see we have a packet of valid length */
    if (length < sizeof(struct ip_header)){
        printf("truncated ip %d",length);
        return NULL;
    }

    len     = ntohs(ip->ip_len);
    hlen    = IP_HL(ip); /* header length */
    version = IP_V(ip);/* ip version */

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
    off = ntohs(ip->ip_off);
    /* aka no 1's in first 13 bits */
    if((off & 0x1fff) == 0 ){
        /* print SOURCE DESTINATION hlen version len offset */
        fprintf(stdout,"IP: ");
        fprintf(stdout,"%s ", inet_ntoa(ip->ip_src));
        fprintf(stdout,"%s %d %d %d %d\n", inet_ntoa(ip->ip_dst), hlen,version,len,off);
        printf("Protocol type : %d\n", protocol_id);
    }

    return NULL;
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
            fprintf(stderr,"Error calling pcap_compile\n");
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
    return 0;
}