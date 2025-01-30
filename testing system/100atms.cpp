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

enum ResponseType {
    SUCCESS = 0,
    INSUFFICIENT_FUNDS = 1,
    DATABASE_ERROR = 2,
    INCORRECT_PIN = 3,
    NETWORK_ERROR = 4,
    SYSTEM_MAINTENANCE = 5,
};

enum TranscationType {
    PIN_CHECK,
    BALANCE_CHECK,
    WITHDRAWL,
    DEPOSIT,
    MOBILE_APROVED_DEPOSIT
};

typedef uint64_t DecimalPosition;

enum Currency {
    GBP,
    USD,
    JPY,
    EUR,
    AUD,
    CAD,
    CHF,
    CNH,
    HKD,
    NZD
};

typedef int64_t AtmCurrency;
typedef uint64_t AtmID;
typedef uint64_t UniqueTranscationID;

struct Transaction {
    TranscationType type;
    UniqueTranscationID id;

    AtmID atmID;
    Currency currency;
    AtmCurrency amount;

    char cardNumber[20];
    char expiryDate[6];
    char pinNo[5];
};

struct Response {
    ResponseType succeeded;
    Currency atmCurrency;
    AtmCurrency newBalance;
    DecimalPosition dotPosition;
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
        transaction.type = transaction.type ==  DEPOSIT ? WITHDRAWL : DEPOSIT;
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


        transaction.id++;
        if(i != total_transactions) {
            std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
        }
    }

    std::cout << "ATM " << atm_id << " finished all transactions\n";
}

std::vector<Transaction> getTranscations() {
    std::vector<Transaction> transactions = {
        {WITHDRAWL, 1, 1234567890, GBP, 0, "5030153826527268", "06/28", "5541"},
        {WITHDRAWL, 1501, 1234567890, GBP, 0, "7352831870655014", "03/28", "4532"},
        {WITHDRAWL, 3001, 1234567890, GBP, 0, "3771984170753636", "01/29", "5959"},
        {WITHDRAWL, 4501, 1234567890, GBP, 0, "9307315805858911", "08/26", "3307"},
        {WITHDRAWL, 6001, 1234567890, GBP, 0, "3741328518603254", "02/29", "1126"},
        {WITHDRAWL, 7501, 1234567890, GBP, 0, "9329498632684939", "04/28", "2400"},
        {WITHDRAWL, 9001, 1234567890, GBP, 0, "3720774724533787", "01/27", "2461"},
        {WITHDRAWL, 10501, 1234567890, GBP, 0, "0832820726785104", "05/26", "3263"},
        {WITHDRAWL, 12001, 1234567890, GBP, 0, "3363278382408078", "12/26", "4756"},
        {WITHDRAWL, 13501, 1234567890, GBP, 0, "1592395037906474", "10/27", "6728"},
        {WITHDRAWL, 15001, 1234567890, GBP, 0, "7803952498074317", "04/28", "2333"},
        {WITHDRAWL, 16501, 1234567890, GBP, 0, "8911467266290734", "02/29", "3427"},
        {WITHDRAWL, 18001, 1234567890, GBP, 0, "9199063999365225", "02/29", "7968"},
        {WITHDRAWL, 19501, 1234567890, GBP, 0, "9707548669125876", "07/26", "2123"},
        {WITHDRAWL, 21001, 1234567890, GBP, 0, "6668897566235512", "08/26", "7050"},
        {WITHDRAWL, 22501, 1234567890, GBP, 0, "2729259902130681", "04/28", "6224"},
        {WITHDRAWL, 24001, 1234567890, GBP, 0, "2306795460102400", "12/27", "1374"},
        {WITHDRAWL, 25501, 1234567890, GBP, 0, "4308400389873621", "07/26", "3181"},
        {WITHDRAWL, 27001, 1234567890, GBP, 0, "1234567812345678", "08/29", "1234"},
        {WITHDRAWL, 28501, 1234567890, GBP, 0, "5432880080008720", "01/28", "5311"},
        {WITHDRAWL, 30001, 1234567890, GBP, 0, "8905672507126086", "11/28", "8914"},
        {WITHDRAWL, 31501, 1234567890, GBP, 0, "6917204417648851", "08/29", "7950"},
        {WITHDRAWL, 33001, 1234567890, GBP, 0, "9749302829504097", "07/29", "5301"},
        {WITHDRAWL, 34501, 1234567890, GBP, 0, "3081330299928113", "09/29", "1018"},
        {WITHDRAWL, 36001, 1234567890, GBP, 0, "1756529172126463", "09/28", "5994"},
        {WITHDRAWL, 37501, 1234567890, GBP, 0, "1743757554333363", "01/28", "2835"},
        {WITHDRAWL, 39001, 1234567890, GBP, 0, "5988021664565097", "01/28", "1685"},
        {WITHDRAWL, 40501, 1234567890, GBP, 0, "7964687834497642", "02/28", "3408"},
        {WITHDRAWL, 42001, 1234567890, GBP, 0, "5758804576269602", "01/29", "8344"},
        {WITHDRAWL, 43501, 1234567890, GBP, 0, "9287899103363694", "07/29", "3598"},
        {WITHDRAWL, 45001, 1234567890, GBP, 0, "8300597628405157", "07/26", "8659"},
        {WITHDRAWL, 46501, 1234567890, GBP, 0, "2845792794461369", "09/28", "4403"},
        {WITHDRAWL, 48001, 1234567890, GBP, 0, "5065384808460899", "05/29", "3284"},
        {WITHDRAWL, 49501, 1234567890, GBP, 0, "7228951668653713", "11/29", "2687"},
        {WITHDRAWL, 51001, 1234567890, GBP, 0, "5068606622409740", "07/28", "8208"},
        {WITHDRAWL, 52501, 1234567890, GBP, 0, "5025977165732892", "02/26", "3122"},
        {WITHDRAWL, 54001, 1234567890, GBP, 0, "2023882592841277", "09/29", "5169"},
        {WITHDRAWL, 55501, 1234567890, GBP, 0, "5169459589054470", "12/28", "2852"},
        {WITHDRAWL, 57001, 1234567890, GBP, 0, "6616393014248283", "03/28", "2512"},
        {WITHDRAWL, 58501, 1234567890, GBP, 0, "8450247521574801", "01/27", "2865"},
        {WITHDRAWL, 60001, 1234567890, GBP, 0, "7431685195434381", "01/27", "7007"},
        {WITHDRAWL, 61501, 1234567890, GBP, 0, "1746780484319824", "08/29", "4842"},
        {WITHDRAWL, 63001, 1234567890, GBP, 0, "8051559030155764", "07/28", "2336"},
        {WITHDRAWL, 64501, 1234567890, GBP, 0, "3270149324457881", "04/27", "6600"},
        {WITHDRAWL, 66001, 1234567890, GBP, 0, "8607676000140349", "06/28", "8416"},
        {WITHDRAWL, 67501, 1234567890, GBP, 0, "0516125761294431", "05/27", "4638"},
        {WITHDRAWL, 69001, 1234567890, GBP, 0, "9044308215762683", "11/29", "8089"},
        {WITHDRAWL, 70501, 1234567890, GBP, 0, "1562961179640328", "01/27", "1682"},
        {WITHDRAWL, 72001, 1234567890, GBP, 0, "8744265217405191", "01/27", "4670"},
        {WITHDRAWL, 73501, 1234567890, GBP, 0, "8157521938425420", "12/28", "2539"},
        {WITHDRAWL, 75001, 1234567890, GBP, 0, "8169191578391098", "09/28", "9049"},
        {WITHDRAWL, 76501, 1234567890, GBP, 0, "0952190943835475", "07/29", "2676"},
        {WITHDRAWL, 78001, 1234567890, GBP, 0, "3938764494105209", "12/27", "5866"},
        {WITHDRAWL, 79501, 1234567890, GBP, 0, "8622889825328098", "11/26", "2556"},
        {WITHDRAWL, 81001, 1234567890, GBP, 0, "4133154495106172", "09/28", "5551"},
        {WITHDRAWL, 82501, 1234567890, GBP, 0, "3636671699679725", "07/29", "9234"},
        {WITHDRAWL, 84001, 1234567890, GBP, 0, "0349466845001848", "05/26", "9754"},
        {WITHDRAWL, 85501, 1234567890, GBP, 0, "4851555232787953", "07/28", "9936"},
        {WITHDRAWL, 87001, 1234567890, GBP, 0, "6117527401765465", "09/27", "9229"},
        {WITHDRAWL, 88501, 1234567890, GBP, 0, "3194341531491800", "12/27", "1620"},
        {WITHDRAWL, 90001, 1234567890, GBP, 0, "3759961831086291", "10/28", "4824"},
        {WITHDRAWL, 91501, 1234567890, GBP, 0, "6932435807333124", "02/28", "3384"},
        {WITHDRAWL, 93001, 1234567890, GBP, 0, "2406615520246185", "10/28", "3809"},
        {WITHDRAWL, 94501, 1234567890, GBP, 0, "4129313235102411", "10/26", "3965"},
        {WITHDRAWL, 96001, 1234567890, GBP, 0, "2211966845672046", "02/26", "4052"},
        {WITHDRAWL, 97501, 1234567890, GBP, 0, "3894904221378119", "10/26", "8912"},
        {WITHDRAWL, 99001, 1234567890, GBP, 0, "3752459700007763", "06/27", "5949"},
        {WITHDRAWL, 100501, 1234567890, GBP, 0, "8402850628325446", "11/27", "6943"},
        {WITHDRAWL, 102001, 1234567890, GBP, 0, "6341578353566171", "02/28", "6906"},
        {WITHDRAWL, 103501, 1234567890, GBP, 0, "6246612820003385", "04/27", "2607"},
        {WITHDRAWL, 105001, 1234567890, GBP, 0, "0681619043469302", "06/29", "7195"},
        {WITHDRAWL, 106501, 1234567890, GBP, 0, "9943803033215401", "10/28", "5221"},
        {WITHDRAWL, 108001, 1234567890, GBP, 0, "8830169681473055", "05/27", "9544"},
        {WITHDRAWL, 109501, 1234567890, GBP, 0, "6935916757809474", "05/28", "2261"},
        {WITHDRAWL, 111001, 1234567890, GBP, 0, "4823839047319095", "08/28", "4110"},
        {WITHDRAWL, 112501, 1234567890, GBP, 0, "2679635956929250", "04/28", "7239"},
        {WITHDRAWL, 114001, 1234567890, GBP, 0, "1531314857803159", "02/28", "6128"},
        {WITHDRAWL, 115501, 1234567890, GBP, 0, "5277973449599253", "11/29", "5618"},
        {WITHDRAWL, 117001, 1234567890, GBP, 0, "6442446935824878", "12/28", "5735"},
        {WITHDRAWL, 118501, 1234567890, GBP, 0, "7749587343108309", "09/28", "6990"},
        {WITHDRAWL, 120001, 1234567890, GBP, 0, "9475267041395788", "02/28", "3507"},
        {WITHDRAWL, 121501, 1234567890, GBP, 0, "5072195388802418", "07/28", "3225"},
        {WITHDRAWL, 123001, 1234567890, GBP, 0, "4917625965574396", "02/29", "9364"},
        {WITHDRAWL, 124501, 1234567890, GBP, 0, "6313083570643335", "09/27", "5676"},
        {WITHDRAWL, 126001, 1234567890, GBP, 0, "6477456967953798", "10/28", "3915"},
        {WITHDRAWL, 127501, 1234567890, GBP, 0, "8471891516684105", "08/26", "1755"},
        {WITHDRAWL, 129001, 1234567890, GBP, 0, "4972493549183837", "07/29", "8730"},
        {WITHDRAWL, 130501, 1234567890, GBP, 0, "6984841470415522", "03/28", "3243"},
        {WITHDRAWL, 132001, 1234567890, GBP, 0, "1668508140097268", "11/27", "5509"},
        {WITHDRAWL, 133501, 1234567890, GBP, 0, "9661726100239564", "12/27", "6079"},
        {WITHDRAWL, 135001, 1234567890, GBP, 0, "1993052780927640", "06/26", "5463"},
        {WITHDRAWL, 136501, 1234567890, GBP, 0, "1644638235263313", "05/29", "5294"},
        {WITHDRAWL, 138001, 1234567890, GBP, 0, "0389928221096826", "08/28", "3577"},
        {WITHDRAWL, 139501, 1234567890, GBP, 0, "3072867065507544", "04/29", "1757"},
        {WITHDRAWL, 141001, 1234567890, GBP, 0, "0313914156678072", "01/28", "7271"},
        {WITHDRAWL, 142501, 1234567890, GBP, 0, "5756521776273810", "11/27", "2052"},
        {WITHDRAWL, 144001, 1234567890, GBP, 0, "5154486008959400", "08/26", "5128"},
        {WITHDRAWL, 145501, 1234567890, GBP, 0, "2236910439435185", "02/28", "9138"},
        {WITHDRAWL, 147001, 1234567890, GBP, 0, "7249972438996357", "09/29", "9426"},
        {WITHDRAWL, 148501, 1234567890, GBP, 0, "7115060446602597", "07/27", "5160"}};

    for (size_t i = 0; i < transactions.size(); i++)
    {
        transactions[i].amount = (i+1);
    }
    
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
