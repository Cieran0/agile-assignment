#include "net.h"
#include "atmUtil.h"
#include <iostream>
#include <string>

#include <sstream>

#include <vector>


using namespace std;

    string cardNumber;      
    string expiryDate;      
    string atmID;          
    string transactionID;  
    string pin; 
    double withdrawalAmount;     

int screenWidth;
int screenHeight;

int main()
{
    Response r = forwardToSocket("123", "1", "0", "1", "1234", 5);
    std::cout << r.succeeded << std::endl;

    screenInit();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        


        screenManager();
                 
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

