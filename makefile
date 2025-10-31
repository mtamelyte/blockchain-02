main:
	g++ -c main.cpp function.cpp hash.cpp user/User.cpp transaction/Transaction.cpp block/Block.cpp merkleRootHash.cpp
	g++ -O3 -o main main.o function.o hash.o User.o Transaction.o Block.o merkleRootHash.o
	./main.exe

run:
	./main.exe

clean:
	del *.o *.exe

merkle:
	g++ -c main.cpp merkleRootHash.cpp
	g++ -o main main.o merkleRootHash.o
	./main.exe