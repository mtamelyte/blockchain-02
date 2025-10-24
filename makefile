main:
	g++ -c main.cpp function.cpp hash.cpp user/User.cpp transaction/Transaction.cpp block/Block.cpp
	g++ -O3 -o main main.o function.o hash.o User.o Transaction.o Block.o
	./main.exe

run:
	./main.exe

clean:
	del *.o *.exe