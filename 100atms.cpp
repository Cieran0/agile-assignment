#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <atomic>
#include "raylib.h"

const std::string SERVER_IP = "127.0.0.1";
int NUMBER_OF_ATMS = 100;
int SERVER_PORT = 6668;
int TRANSACTIONS_PER_MINUTE = 2;
int SIMULATION_DURATION_MINUTES = 5;

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3

struct Response {
    int succeeded;
    double new_balance;
};

SSL_CTX* create_ssl_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Error creating SSL context\n";
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

std::atomic<int> passed = 0;
std::atomic<int> failed = 0;


void atm_client(int atm_id, SSL_CTX *ctx, Transaction transaction) {
    int total_transactions = TRANSACTIONS_PER_MINUTE * SIMULATION_DURATION_MINUTES;

    for (int i = 1; i <= total_transactions; ++i) {
        transaction.withdrawalAmount *= -1;
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            std::cerr << "ATM " << atm_id << ": Error creating socket\n";
            failed.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
            continue; 
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP.c_str(), &server_addr.sin_addr);

        if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "ATM " << atm_id << ": Connection failed\n";
            close(client_socket);
            failed.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_socket);

        if (SSL_connect(ssl) <= 0) {
            std::cerr << "ATM " << atm_id << ": SSL handshake failed\n";
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(client_socket);
             failed.fetch_add(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
            continue;
        }

        std::cout << "ATM " << atm_id << " connected to server securely\n";

        SSL_write(ssl, &transaction, sizeof(transaction));

        Response response;
        SSL_read(ssl, &response, sizeof(response));

        if (response.succeeded != 0) {
            failed.fetch_add(1);
        } else {
            passed.fetch_add(1);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_socket);


        transaction.uniqueTransactionID++;
        if(i != total_transactions) {
            std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
        }
    }

    std::cout << "ATM " << atm_id << " finished all transactions\n";
}

std::vector<Transaction> getTranscations() {
    std::vector<Transaction> transactions = {
        {"5030153826527268", "06/28", 1234567890, 1, "5541", 10.29},
        {"7352831870655014", "03/28", 1234567890, 1501, "4532", 469.92},
        {"3771984170753636", "01/29", 1234567890, 3001, "5959", 651.96},
        {"9307315805858911", "08/26", 1234567890, 4501, "3307", 623.99},
        {"3741328518603254", "02/29", 1234567890, 6001, "1126", 293.93},
        {"9329498632684939", "04/28", 1234567890, 7501, "2400", 145.45},
        {"3720774724533787", "01/27", 1234567890, 9001, "2461", 600.7},
        {"0832820726785104", "05/26", 1234567890, 10501, "3263", 432.84},
        {"3363278382408078", "12/26", 1234567890, 12001, "4756", 574.52},
        {"1592395037906474", "10/27", 1234567890, 13501, "6728", 156.93},
        {"7803952498074317", "04/28", 1234567890, 15001, "2333", 37.64},
        {"8911467266290734", "02/29", 1234567890, 16501, "3427", 606.65},
        {"9199063999365225", "02/29", 1234567890, 18001, "7968", 626.77},
        {"9707548669125876", "07/26", 1234567890, 19501, "2123", 247.86},
        {"6668897566235512", "08/26", 1234567890, 21001, "7050", 901.0},
        {"2729259902130681", "04/28", 1234567890, 22501, "6224", 667.96},
        {"2306795460102400", "12/27", 1234567890, 24001, "1374", 559.93},
        {"4308400389873621", "07/26", 1234567890, 25501, "3181", 756.95},
        {"1234567812345678", "08/29", 1234567890, 27001, "1234", 298.82},
        {"5432880080008720", "01/28", 1234567890, 28501, "5311", 478.33},
        {"8905672507126086", "11/28", 1234567890, 30001, "8914", 868.7},
        {"6917204417648851", "08/29", 1234567890, 31501, "7950", 863.75},
        {"9749302829504097", "07/29", 1234567890, 33001, "5301", 143.52},
        {"3081330299928113", "09/29", 1234567890, 34501, "1018", 170.52},
        {"1756529172126463", "09/28", 1234567890, 36001, "5994", 172.04},
        {"1743757554333363", "01/28", 1234567890, 37501, "2835", 463.87},
        {"5988021664565097", "01/28", 1234567890, 39001, "1685", 379.26},
        {"7964687834497642", "02/28", 1234567890, 40501, "3408", 822.04},
        {"5758804576269602", "01/29", 1234567890, 42001, "8344", 796.63},
        {"9287899103363694", "07/29", 1234567890, 43501, "3598", 9.21},
        {"8300597628405157", "07/26", 1234567890, 45001, "8659", 837.75},
        {"2845792794461369", "09/28", 1234567890, 46501, "4403", 21.0},
        {"5065384808460899", "05/29", 1234567890, 48001, "3284", 505.17},
        {"7228951668653713", "11/29", 1234567890, 49501, "2687", 16.16},
        {"5068606622409740", "07/28", 1234567890, 51001, "8208", 548.99},
        {"5025977165732892", "02/26", 1234567890, 52501, "3122", 691.38},
        {"2023882592841277", "09/29", 1234567890, 54001, "5169", 873.0},
        {"5169459589054470", "12/28", 1234567890, 55501, "2852", 271.43},
        {"6616393014248283", "03/28", 1234567890, 57001, "2512", 169.8},
        {"8450247521574801", "01/27", 1234567890, 58501, "2865", 29.94},
        {"7431685195434381", "01/27", 1234567890, 60001, "7007", 905.84},
        {"1746780484319824", "08/29", 1234567890, 61501, "4842", 20.22},
        {"8051559030155764", "07/28", 1234567890, 63001, "2336", 947.21},
        {"3270149324457881", "04/27", 1234567890, 64501, "6600", 960.96},
        {"8607676000140349", "06/28", 1234567890, 66001, "8416", 547.15},
        {"0516125761294431", "05/27", 1234567890, 67501, "4638", 369.6},
        {"9044308215762683", "11/29", 1234567890, 69001, "8089", 643.71},
        {"1562961179640328", "01/27", 1234567890, 70501, "1682", 412.9},
        {"8744265217405191", "01/27", 1234567890, 72001, "4670", 251.3},
        {"8157521938425420", "12/28", 1234567890, 73501, "2539", 390.6},
        {"8169191578391098", "09/28", 1234567890, 75001, "9049", 963.87},
        {"0952190943835475", "07/29", 1234567890, 76501, "2676", 914.0},
        {"3938764494105209", "12/27", 1234567890, 78001, "5866", 674.64},
        {"8622889825328098", "11/26", 1234567890, 79501, "2556", 681.31},
        {"4133154495106172", "09/28", 1234567890, 81001, "5551", 408.13},
        {"3636671699679725", "07/29", 1234567890, 82501, "9234", 697.98},
        {"0349466845001848", "05/26", 1234567890, 84001, "9754", 779.74},
        {"4851555232787953", "07/28", 1234567890, 85501, "9936", 676.16},
        {"6117527401765465", "09/27", 1234567890, 87001, "9229", 404.27},
        {"3194341531491800", "12/27", 1234567890, 88501, "1620", 482.55},
        {"3759961831086291", "10/28", 1234567890, 90001, "4824", 557.32},
        {"6932435807333124", "02/28", 1234567890, 91501, "3384", 527.6},
        {"2406615520246185", "10/28", 1234567890, 93001, "3809", 734.3},
        {"4129313235102411", "10/26", 1234567890, 94501, "3965", 215.65},
        {"2211966845672046", "02/26", 1234567890, 96001, "4052", 973.39},
        {"3894904221378119", "10/26", 1234567890, 97501, "8912", 903.09},
        {"3752459700007763", "06/27", 1234567890, 99001, "5949", 477.6},
        {"8402850628325446", "11/27", 1234567890, 100501, "6943", 601.34},
        {"6341578353566171", "02/28", 1234567890, 102001, "6906", 435.32},
        {"6246612820003385", "04/27", 1234567890, 103501, "2607", 682.04},
        {"0681619043469302", "06/29", 1234567890, 105001, "7195", 243.63},
        {"9943803033215401", "10/28", 1234567890, 106501, "5221", 29.68},
        {"8830169681473055", "05/27", 1234567890, 108001, "9544", 299.75},
        {"6935916757809474", "05/28", 1234567890, 109501, "2261", 121.39},
        {"4823839047319095", "08/28", 1234567890, 111001, "4110", 877.42},
        {"2679635956929250", "04/28", 1234567890, 112501, "7239", 145.88},
        {"1531314857803159", "02/28", 1234567890, 114001, "6128", 391.55},
        {"5277973449599253", "11/29", 1234567890, 115501, "5618", 265.99},
        {"6442446935824878", "12/28", 1234567890, 117001, "5735", 866.25},
        {"7749587343108309", "09/28", 1234567890, 118501, "6990", 481.42},
        {"9475267041395788", "02/28", 1234567890, 120001, "3507", 965.68},
        {"5072195388802418", "07/28", 1234567890, 121501, "3225", 47.86},
        {"4917625965574396", "02/29", 1234567890, 123001, "9364", 923.84},
        {"6313083570643335", "09/27", 1234567890, 124501, "5676", 98.52},
        {"6477456967953798", "10/28", 1234567890, 126001, "3915", 986.83},
        {"8471891516684105", "08/26", 1234567890, 127501, "1755", 687.68},
        {"4972493549183837", "07/29", 1234567890, 129001, "8730", 888.11},
        {"6984841470415522", "03/28", 1234567890, 130501, "3243", 41.62},
        {"1668508140097268", "11/27", 1234567890, 132001, "5509", 78.14},
        {"9661726100239564", "12/27", 1234567890, 133501, "6079", 259.05},
        {"1993052780927640", "06/26", 1234567890, 135001, "5463", 263.09},
        {"1644638235263313", "05/29", 1234567890, 136501, "5294", 677.0},
        {"0389928221096826", "08/28", 1234567890, 138001, "3577", 906.07},
        {"3072867065507544", "04/29", 1234567890, 139501, "1757", 3.51},
        {"0313914156678072", "01/28", 1234567890, 141001, "7271", 361.4},
        {"5756521776273810", "11/27", 1234567890, 142501, "2052", 28.61},
        {"5154486008959400", "08/26", 1234567890, 144001, "5128", 269.42},
        {"2236910439435185", "02/28", 1234567890, 145501, "9138", 500.21},
        {"7249972438996357", "09/29", 1234567890, 147001, "9426", 237.47},
        {"7115060446602597", "07/27", 1234567890, 148501, "5160", 134.79}};
    return transactions;
}

void run_atm_simulation(int num_atms) {
    SSL_CTX *ctx = create_ssl_context();
    std::vector<std::thread> atm_threads;
    std::vector<Transaction> transactions = getTranscations();

    for (int i = 1; i <= num_atms; ++i) {
        atm_threads.emplace_back(atm_client, i, ctx, transactions[i-1]);
    }

    for (auto& t : atm_threads) {
        if (t.joinable()) t.join();
    }

    SSL_CTX_free(ctx);
    std::cout << "ATM simulation completed.\n";
}

void start(std::thread& simulation_thread) {
    simulation_thread = std::thread(run_atm_simulation, NUMBER_OF_ATMS);
}

int main(int argc, char *argv[]) {
    InitWindow(1920, 1210, "100 ATM - Test");
    int start_len = MeasureText("Start", 66);
    int running_len = MeasureText("Running", 66);
    bool running = false;
    int current_timepoint = 1;
    int passed_at_seconds[600] = {0};
    int failed_at_seconds[600] = {0};
    std::chrono::time_point lastUpdateTime = std::chrono::steady_clock::now();

    std::thread simulation_thread;


    while (!WindowShouldClose()) {
        BeginDrawing();
        std::chrono::time_point currentTime = std::chrono::steady_clock::now();
        int64_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count();

        if (running && elapsedTime >= 500 && current_timepoint < 600) { 
            passed_at_seconds[current_timepoint] = passed;
            failed_at_seconds[current_timepoint] = failed;
            current_timepoint++;
            lastUpdateTime = currentTime;
        }

        ClearBackground(LIGHTGRAY);
        DrawRectangle(0,0,60,1150, DARKGRAY);
        DrawRectangle(0,1150,1920,60, DARKGRAY);
        DrawRectangle(1920-60,0,60,1150, DARKGRAY);
        DrawRectangle(0,0, 1920, (1080 * 0.125), DARKGRAY);
        DrawRectangle(1920 - (256 + 64) - 5, (1080 * 0.125) / 4 - 5, 256 + 10, (1080 * 0.125) / 2 + 10, running ? GREEN : DARKGREEN);
        DrawRectangle(1920 - (256 + 64), (1080 * 0.125) / 4, 256, (1080 * 0.125) / 2,  running ? DARKGREEN : GREEN);
        if(running) {
            DrawText("Running", 1920 - (256 + 64) + (256 - running_len)/2, (1080 * 0.125) / 4 , 66, BLACK);
        } else {
            DrawText("Start", 1920 - (256 + 64) + (256 - start_len)/2, (1080 * 0.125) / 4 , 66, BLACK);
        }

        for (int i = 0; i < 5*2; i++)
        {
            DrawLine(60 + i * (1800/10), 135, 60 + i * (1800/10), 1150, BLACK);
        }
        
        if(!running && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int x = GetMouseX();
            int y = GetMouseY();    

            if ((x >= 1920 - (256 + 64) && x <= (1920 - (256 + 64)) + (256)) 
            && y >= (1080 * 0.125) / 4 && y <= ((1080 * 0.125) / 4) + ((1080 * 0.125) / 2)) {
                running = true;
                start(simulation_thread);
            }

        }


        int passed_int = passed;
        int failed_int = failed;
        int total = passed_int+failed_int;

        std::cout << passed_int << " : " << failed << " : " << total << std::endl;

        const char* passed_string = TextFormat("%d/%d Passed", passed_int, total);
        DrawText(passed_string, 192/5 , ((1080 * 0.125) / 4), 66, BLACK);

        for(int i = 0; i < current_timepoint; i++) {
            DrawRectangle((i-1)*3 + 60, 1150 - failed_at_seconds[i], 3, failed_at_seconds[i], RED);
            DrawRectangle((i-1)*3 + 60, 1150 - passed_at_seconds[i], 3, passed_at_seconds[i], LIME);
        }
        EndDrawing();
    }

    CloseWindow();

    simulation_thread.join();

    return 0;
}