main:
	g++ -c -fopenmp main.cpp src/function.cpp src/hash.cpp user/User.cpp transaction/Transaction.cpp block/Block.cpp merkleTree/MerkleTree.cpp src/mining.cpp
	g++ -O3 -o main main.o function.o hash.o User.o Transaction.o Block.o MerkleTree.o mining.o -fopenmp
	./main.exe

run:
	./main.exe

clean:
	del *.o *.exe