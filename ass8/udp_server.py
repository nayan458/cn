import socket
import threading
import datetime

# Server configurations
HOST = '127.0.0.1'  # Localhost
PORT = 8080         # Port to listen on

clients = []
lock = threading.Lock()

# Function to broadcast messages to all clients
def broadcast(message, client_socket):
    with lock:
        for client in clients:
            if client != client_socket:
                try:
                    client.sendall(message)
                except:
                    client.close()
                    clients.remove(client)

# Function to handle each client connection
def handle_client(client_socket):
    while True:
        try:
            message = client_socket.recv(1024)
            if message:
                timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                message_to_log = f"[{timestamp}] {message.decode()}"
                print(message_to_log)
                
                # Log the message to log.txt
                with open("log.txt", "a") as f:
                    f.write(message_to_log + "\n")
                
                # Broadcast the message to other clients
                broadcast(message, client_socket)
            else:
                # Remove client if no message is received
                with lock:
                    clients.remove(client_socket)
                client_socket.close()
                break
        except:
            with lock:
                clients.remove(client_socket)
            client_socket.close()
            break

# Main server function to accept incoming connections
def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    print("Server started. Waiting for connections...")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address} has been established.")
        
        with lock:
            clients.append(client_socket)

        # Start a new thread to handle the client
        threading.Thread(target=handle_client, args=(client_socket,)).start()

if __name__ == "__main__":
    start_server()
