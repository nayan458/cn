#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PACKET_SIZE 64

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <mode (client/server)>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    const char *mode = argv[2];

    if (strcmp(mode, "client") == 0) {
        // Client mode
        int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sock < 0) {
            perror("Socket error");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_addr.s_addr = inet_addr(server_ip);

        char packet[PACKET_SIZE];

        for (int i = 0; i < 5; i++) { // Send 5 packets
            create_icmp_packet(packet, i);

            if (sendto(sock, packet, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
                perror("Send failed");
            } else {
                printf("Sent ICMP Echo Request to %s\n", server_ip);
            }

            sleep(1);
        }

        close(sock);

    } else if (strcmp(mode, "server") == 0) {
        // Server mode
        int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sock < 0) {
            perror("Socket error");
            exit(EXIT_FAILURE);
        }

        char buffer[PACKET_SIZE];
        struct sockaddr_in src_addr;
        socklen_t addr_len = sizeof(src_addr);

        while (1) {
            int bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &addr_len);
            if (bytes_received > 0) {
                struct icmphdr *icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));
                if (icmp->type == ICMP_ECHO) {
                    printf("Received ICMP Echo Request from %s\n", inet_ntoa(src_addr.sin_addr));

                    // Change type to ICMP_ECHOREPLY
                    icmp->type = ICMP_ECHOREPLY;
                    icmp->checksum = 0;
                    icmp->checksum = checksum(buffer + sizeof(struct iphdr), PACKET_SIZE - sizeof(struct iphdr));

                    if (sendto(sock, buffer, bytes_received, 0, (struct sockaddr *)&src_addr, addr_len) <= 0) {
                        perror("Send reply failed");
                    } else {
                        printf("Sent ICMP Echo Reply to %s\n", inet_ntoa(src_addr.sin_addr));
                    }
                }
            }
        }

        close(sock);
    } else {
        fprintf(stderr, "Invalid mode. Use 'client' or 'server'.\n");
        return 1;
    }

    return 0;
}
