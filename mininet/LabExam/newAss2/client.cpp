#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define PACKET_SIZE 64

int sock; // Global socket for cleanup

// Compute checksum for ICMP packet
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void create_icmp_packet(char *packet, int sequence) {
    struct icmphdr *icmp = (struct icmphdr *)packet;

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = getpid();
    icmp->un.echo.sequence = sequence;
    icmp->checksum = 0;

    // Fill some data
    memset(packet + sizeof(struct icmphdr), 0, PACKET_SIZE - sizeof(struct icmphdr));

    // Calculate checksum
    icmp->checksum = checksum(packet, PACKET_SIZE);
}

void handle_signal(int sig) {
    printf("\nTerminating client...\n");
    close(sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];

    sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(server_ip);

    char packet[PACKET_SIZE];
    int sequence = 0;

    // Handle SIGINT for graceful termination
    signal(SIGINT, handle_signal);

    while (1) {
        create_icmp_packet(packet, sequence++);

        if (sendto(sock, packet, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
            perror("Send failed");
        } else {
            printf("Sent ICMP Echo Request to %s (sequence %d)\n", server_ip, sequence);
        }

        sleep(1); // Send one packet per second
    }

    return 0;
}
