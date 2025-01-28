#include "net.h"
#include "atmUtil.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

string atmID;
string transactionID;
string pin;
double withdrawalAmount;

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 6667

int main(int argc, char *argv[]) {
    port = DEFAULT_PORT;
    host = DEFAULT_HOST;

    if (argc != 1 && argc != 3) {
        cout << "Usage: " << argv[0] << " [<host> <port>]\n";
        return 1;
    }

    if (argc == 3) {
        host = argv[1];
        port = atoi(argv[2]);
        if (port <= 0 || port > 65535) {
            cout << "Invalid port.\n";
            return 1;
        }
    }

    setCurrency(USD);
    screenInit();
    while (!WindowShouldClose()) {
        BeginDrawing();
        drawAtmCasing();
        screenManager();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}