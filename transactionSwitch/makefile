all: server client

server:
	g++ -std=c++20 main.cpp TransactionSwitchSecure.cpp TransactionLogr.cpp -o server -lssl -lcrypto -lpthread

client:
	g++ -std=c++20 multiATM.cpp -o client -lssl -lcrypto -lpthread

clean:
	rm -f server client
