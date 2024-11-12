import socket
import threading

# Server configurations
HOST = '127.0.0.1'  # Server's hostname or IP address
PORT = 8080         # Port used by the server

# Function to receive messages from the server
def receive_messages(client_socket):
    while True:
        try:
            message = client_socket.recv(1024).decode()
            print(message)
        except:
            print("Connection to the server lost.")
            client_socket.close()
            break

# Main function for client
def start_client():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((HOST, PORT))

    print("Connected to the server.")

    # Start a thread to handle incoming messages from the server
    threading.Thread(target=receive_messages, args=(client_socket,)).start()

    while True:
        message = input()
        if message.lower() == 'exit':
            break
        client_socket.sendall(message.encode())

    client_socket.close()

if __name__ == "__main__":
    start_client()
