#pragma once

#include <cstdint>
#include <string>
#include "transaction.h"


#define NETWORK (Response){.succeeded = ResponseType::NETWORK_ERROR}

extern const char* host;
extern int port;

Response forwardToSocket(TransactionType type, AtmID atmID, Currency currency, AtmCurrency amount, const char cardNumber[20], const char expiryDate[6], const char pinNo[5]);
uint64_t rand_uint64();
