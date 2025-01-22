#!/usr/bin/env python3
import ssl
import socket
import struct

PORT = 8000
SERVER = '127.0.0.1'  # Change to the server's IP address if needed

def main():
    # Hardcoded transaction data
    card_number = "5030153826527268"  # 16-digit card number
    expiry_date = "0628"  # MMYY format
    atm_id = 2486815646  # Random ATM ID
    unique_transaction_id = 268229  # Random transaction ID
    pin_no = "5541"  # 4-digit PIN
    withdrawal_amount = 100.0  # Withdrawal amount

    # Create a socket and wrap it with SSL
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)
    context.load_verify_locations(cafile='server.crt')  # Server certificate to verify server

    # Connect to the server
    conn = context.wrap_socket(socket.socket(socket.AF_INET, socket.SOCK_STREAM), server_hostname=SERVER)
    conn.connect((SERVER, PORT))

    try:
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

        print("Sending hardcoded transaction data:")
        print(f"  Card Number: {card_number}")
        print(f"  Expiry Date: {expiry_date}")
        print(f"  ATM ID: {atm_id}")
        print(f"  Transaction ID: {unique_transaction_id}")
        print(f"  PIN: {pin_no}")
        print(f"  Withdrawal Amount: {withdrawal_amount}")

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

    finally:
        # Close the connection
        conn.close()

if __name__ == '__main__':
    main()

