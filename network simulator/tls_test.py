
#!/usr/bin/env python3
import ssl
import socket
import time
import struct
import random
import string

PORT = 8000
SERVER = '127.0.0.1'  # Change to the server's IP address if needed

# Helper functions to generate random data
def random_card_number():
    return ''.join(random.choices(string.digits, k=16))

def random_expiry_date():
    month = random.randint(1, 12)
    year = random.randint(24, 29)  # Assuming expiry years between 2024 and 2029
    return f"{month:02d}{year:02d}"

def random_pin():
    return ''.join(random.choices(string.digits, k=4))

def random_withdrawal_amount():
    return round(random.uniform(10, 1000), 2)  # Withdrawal amount between £10 and £1000

def main():
    # Create a socket and wrap it with SSL
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)
    context.load_verify_locations(cafile='server.crt')  # Server certificate to verify server

    # Connect to the server
    conn = context.wrap_socket(socket.socket(socket.AF_INET, socket.SOCK_STREAM), server_hostname=SERVER)
    conn.connect((SERVER, PORT))

    try:
        while True:
            # Generate random transaction data
            card_number = random_card_number()
            expiry_date = random_expiry_date()
            atm_id = random.randint(1000, 9999)  # Random ATM ID
            unique_transaction_id = random.randint(1, 1_000_000)  # Random transaction ID
            pin_no = random_pin()
            withdrawal_amount = random_withdrawal_amount()

            # Print the generated transaction details
            print(f"Generated Transaction:\n"
                  f"  Card Number: {card_number}\n"
                  f"  Expiry Date: {expiry_date}\n"
                  f"  ATM ID: {atm_id}\n"
                  f"  Transaction ID: {unique_transaction_id}\n"
                  f"  PIN: {pin_no}\n"
                  f"  Withdrawal Amount: {withdrawal_amount}\n")

            # Pack the data into binary format
            message = struct.pack(
                '20s 6s Q Q 5s d',
                card_number.encode('utf-8'),
                expiry_date.encode('utf-8'),
                atm_id,
                unique_transaction_id,
                pin_no.encode('utf-8'),
                withdrawal_amount
            )

            print("Sending transaction data...")
            conn.send(message)

            # Receive the response from the server
            response_data = conn.recv(2048)

            # Unpack the response struct from the server
            succeeded, new_balance = struct.unpack('i d', response_data)
            if succeeded == 0:  # TRANSACTION_SUCCESS
                print(f"Transaction Approved. New Balance: £{new_balance:.2f}")
            elif succeeded == 1:  # INSUFFICIENT_FUNDS
                print("Transaction Declined: Insufficient Funds")
            elif succeeded == 2:  # DATABASE_ERROR
                print("Transaction Declined: Database Error")
            elif succeeded == 3:  # INCORRECT_PIN
                print("Transaction Declined: Incorrect PIN")
            else:
                print(f"Transaction Declined: Unknown error code {succeeded}")

            time.sleep(5)  # Wait before sending the next transaction

    finally:
        # Close the connection
        conn.close()

if __name__ == '__main__':
    main()

