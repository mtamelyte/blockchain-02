CXX = g++
CXXFLAGS = -std=c++11 -Wall -g
INCLUDES = -I/usr/local/boost-1.65/include -I/usr/local/include -Iinclude
LDFLAGS = -L/usr/local/boost-1.65/lib -L/usr/local/lib -Wl,-rpath,/usr/local/boost-1.65/lib
LIBS = -lbitcoin -lboost_system -lboost_filesystem -lboost_program_options \
       -lboost_thread -lboost_regex -lboost_log -lboost_log_setup \
       -lboost_chrono -lboost_date_time -lpthread -fopenmp

TARGET = main

SOURCES = main.cpp block/Block.cpp src/function.cpp src/hash.cpp src/libbitcoinMerkle.cpp src/mining.cpp transaction/Transaction.cpp user/User.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run