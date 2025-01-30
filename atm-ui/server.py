import socket
import ssl
import struct
import logging

# Define constants
TRANSACTION_STRUCT_FORMAT = '20s 6s Q Q 5s d'  # Format string for Transaction struct
RESPONSE_STRUCT_FORMAT = 'i d'  # Format string for Response struct

# Sample balance dictionary (for simplicity)
account_balances = {
    1234567890123456: 1000.0,  # Example card number mapped to balance
    9876543210987654: 1500.0,  # Another card number
    5030153826527268: 100000.53
}

def handle_transaction(transaction_data):
    """
    Handles the incoming transaction, validates it, and returns a response.
    """
    try:
        # Unpack transaction using the struct format
        transaction = struct.unpack(TRANSACTION_STRUCT_FORMAT, transaction_data)
        
        card_number = transaction[0].decode('utf-8').strip('\x00')
        expiry_date = transaction[1].decode('utf-8').strip('\x00')
        atm_id = transaction[2]
        unique_transaction_id = transaction[3]
        pin_no = transaction[4].decode('utf-8').strip('\x00')
        withdrawal_amount = transaction[5]
        
        # Log the transaction details
        logging.info(f"Received transaction: Card {card_number}, ATM ID {atm_id}, "
                     f"Transaction ID {unique_transaction_id}, Amount {withdrawal_amount}")

        # Check if card number exists in the balance dictionary
        card_number_long = int(card_number)  # Convert card number to integer
        if card_number_long not in account_balances:
            logging.warning(f"Card {card_number} not found in records.")
            return struct.pack(RESPONSE_STRUCT_FORMAT, 0, 0.0)  # Failed response

        current_balance = account_balances[card_number_long]

        # Process withdrawal request
        if withdrawal_amount > current_balance:
            logging.warning(f"Insufficient funds for card {card_number}. Current balance: {current_balance}")
            return struct.pack(RESPONSE_STRUCT_FORMAT, 0, current_balance)  # Failed response

        # Update balance
        new_balance = current_balance - withdrawal_amount
        account_balances[card_number_long] = new_balance

        # Return success response with updated balance
        logging.info(f"Transaction successful. New balance for card {card_number}: {new_balance}")
        return struct.pack(RESPONSE_STRUCT_FORMAT, 0, new_balance)

    except Exception as e:
        logging.error(f"Error handling transaction: {e}")
        return struct.pack(RESPONSE_STRUCT_FORMAT, 0, 0.0)  # Failed response

def start_tls_server(host, port, certfile, keyfile):
    """
    Starts a TLS server that listens for transactions and sends responses.
    """
    # Create an SSL context
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile=certfile, keyfile=keyfile)

    # Create a TCP socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0) as server_socket:
        server_socket.bind((host, port))
        server_socket.listen(5)

        # Wrap the server socket with SSL
        with context.wrap_socket(server_socket, server_side=True) as ssl_server_socket:
            logging.info(f"Server started on {host}:{port} (TLS)")

            while True:
                # Wait for a connection
                client_socket, addr = ssl_server_socket.accept()
                logging.info(f"Connection from {addr}")

                try:
                    # Receive the transaction data
                    print(struct.calcsize(TRANSACTION_STRUCT_FORMAT))

                    transaction_data = client_socket.recv(struct.calcsize(TRANSACTION_STRUCT_FORMAT))
                    if len(transaction_data) != struct.calcsize(TRANSACTION_STRUCT_FORMAT):
                        logging.warning(f"Invalid transaction data received from {addr}")
                        client_socket.send(struct.pack(RESPONSE_STRUCT_FORMAT, 0, 0.0))
                        client_socket.close()
                        continue

                    # Handle the transaction and get the response
                    response_data = handle_transaction(transaction_data)

                    # Send the response back to the client
                    client_socket.send(response_data)

                except Exception as e:
                    logging.error(f"Error during communication with {addr}: {e}")
                finally:
                    client_socket.close()

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)

    # TLS certificate and key files
    certfile = 'server.crt'
    keyfile = 'server.key'

    # Start the server on localhost:65432
    start_tls_server('localhost', 6667, certfile, keyfile)
