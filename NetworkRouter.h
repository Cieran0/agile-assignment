// NetworkRouter.h
#ifndef NETWORKROUTER_H
#define NETWORKROUTER_H

#include <string>

class NetworkRouter {
public:
    // constructor & destructor
    NetworkRouter();
    ~NetworkRouter();

    // send a transaction request to network sim
    std::string routeTransaction(const std::string& request);

private:
    // shrug emoji
};

#endif // NETWORKROUTER_H