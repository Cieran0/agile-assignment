all:
	g++ -o network_sim *.cpp -lssl -lcrypto -lsqlite3 -std=c++20
 
network_sim: sim.*
	g++ -o network_sim sim.cpp -lssl -lcrypto
	
db: db.*
	g++ -o db_sim db.cpp -lsqlite3 -std=c++20

log: log.*
	g++ -o log log.cpp -std=c++20 -lsqlite3
