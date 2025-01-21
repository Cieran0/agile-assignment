#include "db.hpp"
#include <iostream>
#include <sstream>

static int callback(void* data, int argc, char** argv, char** azColName){
    for(int i = 0; i < argc; i++){
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;

    return 0;

}

int init(sqlite3* db){
    int exitCode = sqlite3_open("database.db", &db);
    if(exitCode){
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }

    return exitCode;
}

int doTransaction(Transaction transaction, double &balance, sqlite3* db){
    std::stringstream query;
    if(transaction.withdrawalAmount != 0.00){
        query << "UPDATE Customer SET Balance = Balance - " << transaction.withdrawalAmount << "WHERE " <<  transaction.cardNumber << " = CardNumber AND " << transaction.pinNo << " = PIN;";

    }
    query.str(std::string());

    query << "SELECT Balance FROM Customer WHERE " <<  transaction.cardNumber << " = CardNumber AND " << transaction.pinNo << " = PIN;";
}

int main(){
    sqlite3* db;
    const char* db_name = "database.db";

    int exitCode = sqlite3_open(db_name, &db);
    if(exitCode){
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }

    const char* query = "SELECT * FROM Customer WHERE Balance = (SELECT MAX(Balance) FROM Customer);";

    char* err_msg = nullptr;
    exitCode = sqlite3_exec(db, query, callback, 0, &err_msg);
    if(exitCode != SQLITE_OK){
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }

    sqlite3_close(db);

    return 0;
}
