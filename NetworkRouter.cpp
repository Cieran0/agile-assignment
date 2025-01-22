// NetworkRouter.cpp
#include "NetworkRouter.h"
#include <iostream>

NetworkRouter::NetworkRouter() {
    
}

NetworkRouter::~NetworkRouter() {
    
}

std::string NetworkRouter::routeTransaction(const std::string& request) {
    // fake sending the request to the network sim hehe
    std::cout << "Routing transaction to Network Simulator: " << request << std::endl;

    
    // fake response bruh
    return "Approved";
}