// Client Program (icmp_client.c)
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
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];

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
    return 0;
}

// 1. Compile the Programs
// Use gcc to compile both files. Run the following commands in your terminal:

// bash
// Copy code
// gcc icmp_client.c -o icmp_client
// gcc icmp_server.c -o icmp_server
// This will create two executables: icmp_client and icmp_server.

// 2. Run the Programs
// Both programs require elevated privileges to use raw sockets. Use sudo to run them.

// Start the server:

// bash
// Copy code
// sudo ./icmp_server
// The server listens for ICMP Echo Request packets and responds with Echo Replies.

// Start the client:

// bash
// Copy code
// sudo ./icmp_client <server_ip>
// Replace <server_ip> with the actual IP address of the machine running the server.

// 3. Open Wireshark
// Start Wireshark and select the network interface used for communication between the client and server.
// Apply a display filter to focus on ICMP packets:
// Copy code
// icmp
// 4. Observe the Output
// In the server terminal, you will see:

// "Received ICMP Echo Request from <client_ip>".
// "Sent ICMP Echo Reply to <client_ip>".
// In the client terminal, you will see:

// "Sent ICMP Echo Request to <server_ip>".
// In Wireshark, you will observe:

// ICMP Echo Request packets from the client to the server.
// ICMP Echo Reply packets from the server to the client.
// 5. Troubleshooting
// If packets are not visible in Wireshark:

// Ensure the firewall is not blocking ICMP traffic on both machines.
// Ensure the client and server are running on the correct network interfaces.
// If the programs fail to run:

// Verify you have root permissions (use sudo).
// Check that raw sockets are allowed on your system.