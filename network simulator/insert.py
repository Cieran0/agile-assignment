

import sqlite3
import shutil

# Database names
db_names = ["db1.db", "db2.db", "db3.db"]

# Create the databases and tables
def create_database(db_name):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()
    
    # Create tables
    cursor.executescript('''
    CREATE TABLE IF NOT EXISTS ConversionRate (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        CurrencyFrom TEXT,
        CurrencyTo TEXT,
        ConversionRate REAL
    );
    
    CREATE TABLE IF NOT EXISTS Currencies (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        CurrencyCode TEXT UNIQUE,
        DotPosition INTEGER
    );
    
    CREATE TABLE IF NOT EXISTS Customer (
        AccountNumber TEXT PRIMARY KEY,
        SortCode TEXT NOT NULL CHECK(LENGTH(SortCode) = 6),
        CardNumber TEXT NOT NULL UNIQUE,
        CVV TEXT NOT NULL CHECK(LENGTH(CAST(CVV AS TEXT)) = 3),
        ExpireDate TEXT NOT NULL CHECK(LENGTH(ExpireDate) = 5 AND SUBSTR(ExpireDate, 3, 1) = '/'),
        PIN TEXT NOT NULL CHECK(LENGTH(CAST(PIN AS TEXT)) = 4),
        Currency TEXT NOT NULL DEFAULT 'GBP',
        Balance BIGINT,
        FOREIGN KEY(Currency) REFERENCES Currencies(CurrencyCode)
    );
    
    CREATE TABLE IF NOT EXISTS Transactions (
        TransactionID INTEGER PRIMARY KEY,
        ATM_ID INTEGER,
        WithdrawlAmount REAL,
        CardNumber TEXT,
        FOREIGN KEY(CardNumber) REFERENCES Customer(CardNumber)
    );
    ''')
    
    conn.commit()
    conn.close()

# Create all databases
for db in db_names:
    create_database(db)

# Connect to a source database and fetch the data
source_db = "database.db"
conn = sqlite3.connect(source_db)
cursor = conn.cursor()

# Copy ConversionRate and Currencies to all databases
conversion_rate_data = cursor.execute("SELECT * FROM ConversionRate").fetchall()
currency_data = cursor.execute("SELECT * FROM Currencies").fetchall()
customer_data = cursor.execute("SELECT * FROM Customer").fetchall()

conn.close()

# Insert data into the three new databases
def insert_data(db_name, conversion_rate_data, currency_data):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()
    
    cursor.executemany("INSERT INTO ConversionRate VALUES (?, ?, ?, ?)", conversion_rate_data)
    cursor.executemany("INSERT INTO Currencies VALUES (?, ?, ?)", currency_data)
    
    conn.commit()
    conn.close()

for db in db_names:
    insert_data(db, conversion_rate_data, currency_data)

# Distribute customers based on the first digit of their CardNumber
def distribute_customers(customer_data):
    db_connections = [sqlite3.connect(db) for db in db_names]
    db_cursors = [conn.cursor() for conn in db_connections]
    
    for customer in customer_data:
        card_number = customer[2]
        first_digit = int(card_number[0])
        
        if 0 <= first_digit <= 3:
            db_index = 0
        elif 4 <= first_digit <= 6:
            db_index = 1
        else:
            db_index = 2
        
        db_cursors[db_index].execute("INSERT INTO Customer (AccountNumber, SortCode, CardNumber, CVV, ExpireDate, PIN, Currency, Balance) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", customer[:8])
    
    for conn in db_connections:
        conn.commit()
        conn.close()

# Insert customers into the respective databases
distribute_customers(customer_data)

