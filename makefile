main:
	g++ -c -fopenmp main.cpp src/function.cpp src/hash.cpp user/User.cpp transaction/Transaction.cpp block/Block.cpp merkleTree/MerkleTree.cpp src/mining.cpp src/libbitcoinMerkle.cpp
	g++ -O3 -o main main.o function.o hash.o User.o Transaction.o Block.o MerkleTree.o mining.o libbitcoinMerkle.o -fopenmp -lbitcoin -lboost_system -lboost_filesystem -lboost_program_options \
       -lboost_thread -lboost_regex -lboost_log -lboost_log_setup \
       -lboost_chrono -lboost_date_time -lpthread
	   -I/usr/local/include -I/usr/local/boost-1.65/include
	   -L/usr/local/lib -L/usr/local/boost-1.65/lib
	./main

run:
	./main

clean:
	del *.o *.exe