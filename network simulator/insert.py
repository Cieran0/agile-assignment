#!/usr/bin/env python3

import sqlite3
import random
import string
from datetime import datetime, timedelta

# Function to generate random data for the Customer table
def generate_customer_data():
    account_number = ''.join(random.choices(string.digits, k=10))
    sort_code = ''.join(random.choices(string.digits, k=6))
    card_number = ''.join(random.choices(string.digits, k=16))
    cvv = random.randint(100, 999)
    expire_date = (datetime.now() + timedelta(days=random.randint(365, 1825))).strftime('%m/%y')
    pin = random.randint(1000, 9999)
    balance = round(random.uniform(100.0, 10000.0), 2)

    return (account_number, sort_code, card_number, cvv, expire_date, pin, balance)

# Function to populate the database with random customers
def populate_database(db_name, num_customers):
    connection = sqlite3.connect(db_name)
    cursor = connection.cursor()

    # Insert random customers into the database
    for _ in range(num_customers):
        customer_data = generate_customer_data()
        try:
            cursor.execute(
                """
                INSERT INTO Customer (AccountNumber, SortCode, CardNumber, CVV, ExpireDate, PIN, Balance)
                VALUES (?, ?, ?, ?, ?, ?, ?)
                """,
                customer_data
            )
        except sqlite3.IntegrityError as e:
            print(f"Error inserting data: {e}. Skipping this record.")

    # Commit and close the connection
    connection.commit()
    connection.close()

# Main script
if __name__ == "__main__":
    database_name = "database.db"
    number_of_customers = 100  # Number of random customers to generate

    populate_database(database_name, number_of_customers)
    print(f"Successfully populated {database_name} with {number_of_customers} random customers.")