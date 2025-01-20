#!/usr/bin/env python3
import ssl
import socket

PORT = 8000
SERVER = '127.0.0.1'  # Change to the server's IP address if needed

def main():
    # Create a socket and wrap it with SSL
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)
    context.load_verify_locations(cafile='server.crt')  # Server certificate to verify server

    # Connect to the server
    conn = context.wrap_socket(socket.socket(socket.AF_INET, socket.SOCK_STREAM), server_hostname=SERVER)
    conn.connect((SERVER, PORT))

    # Send some data
    while True:

        message = "Hello, SSL Server!"
        print(f"Sending: {message}")
        conn.send(message.encode())
    
        # Receive the response from the server
        data = conn.recv(2048)
        print(f"Received: {data.decode()}")

    # Close the connection
    conn.close()

if __name__ == '__main__':
    main()
