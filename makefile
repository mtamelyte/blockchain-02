main:
	g++ -c -fopenmp main.cpp function.cpp hash.cpp user/User.cpp transaction/Transaction.cpp block/Block.cpp merkleTree/MerkleTree.cpp mining.cpp
	g++ -O3 -o main main.o function.o hash.o User.o Transaction.o Block.o MerkleTree.o mining.o -fopenmp
	./main.exe

run:
	./main.exe

clean:
	del *.o *.exe

merkle:
	g++ -c main.cpp merkleTree/MerkleTree.cpp
	g++ -o main main.o MerkleTree.o
	./main.exe