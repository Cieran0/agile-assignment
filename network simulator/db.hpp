#pragma once
#include <sqlite3.h>
#include <future>

#include "Transaction.hpp"

std::future<Response> enqueueTransaction(Transaction transaction);
void processTransactionQueue();

extern std::condition_variable queueCondition;

using TransactionPromise = std::pair<Transaction, std::promise<Response>>;

void enqueueTransactionLog(std::string log_sql);
