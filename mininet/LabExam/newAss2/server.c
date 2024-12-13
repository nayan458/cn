// Server Program (icmp_server.c)
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
    unsigned short *buf = (unsigned short *)b; // Explicit cast
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


int main() {
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
    return 0;
}



// Below is a C program demonstrating a raw socket implementation for sending ICMP echo requests and handling ICMP echo replies. You can test this program and use Wireshark to observe the ICMP packets.

// ### Instructions to Test the Program

// 1. **Compile the Program**:
//    ```bash
//    gcc -o icmp_program icmp_program.c
//    ```

// 2. **Run the Server**:
//    Start the server on one terminal:
//    ```bash
//    sudo ./icmp_program 0.0.0.0 server
//    ```

// 3. **Run the Client**:
//    Start the client on another terminal:
//    ```bash
//    sudo ./icmp_program <server_ip> client
//    ```

// 4. **Observe Using Wireshark**:
//    - Open Wireshark.
//    - Start capturing packets on your network interface.
//    - Filter packets using `icmp`.
//    - You will see ICMP echo requests and replies between the client and server.

// ### Notes
// - **Root Privileges**: Raw sockets require root privileges.
// - **Testing in Virtualized Environments**: Use two virtual machines on the same network if testing between client and server on different systems.
// - **Security Concerns**: Use raw sockets responsibly, as they can interfere with network operations.