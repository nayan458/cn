Below is a C program demonstrating a raw socket implementation for sending ICMP echo requests and handling ICMP echo replies. You can test this program and use Wireshark to observe the ICMP packets.

### Instructions to Test the Program

1. **Compile the Program**:
   ```bash
   gcc -o icmp_program icmp_program.c
   ```

2. **Run the Server**:
   Start the server on one terminal:
   ```bash
   sudo ./icmp_program 0.0.0.0 server
   ```

3. **Run the Client**:
   Start the client on another terminal:
   ```bash
   sudo ./icmp_program <server_ip> client
   ```

4. **Observe Using Wireshark**:
   - Open Wireshark.
   - Start capturing packets on your network interface.
   - Filter packets using `icmp`.
   - You will see ICMP echo requests and replies between the client and server.

### Notes
- **Root Privileges**: Raw sockets require root privileges.
- **Testing in Virtualized Environments**: Use two virtual machines on the same network if testing between client and server on different systems.
- **Security Concerns**: Use raw sockets responsibly, as they can interfere with network operations.