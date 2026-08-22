CXX      ?= g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pthread

.PHONY: all lib app test clean

all: lib app

lib: libjournal.so

app: test-lib-journal

libjournal.so: libjournal.o
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

test-lib-journal: main.o Application.o libjournal.so
	$(CXX) $(CXXFLAGS) -o $@ $^ -Wl,-rpath,'$$ORIGIN'

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -fPIC -Iinclude -c $< -o $@

libjournal.o main.o Application.o: \
	include/libjournal.hpp src/ThreadSafeQueue.hpp src/Application.hpp

clean:
	rm -f *.o *.d libjournal.so test-lib-journal test_journal.log
	rm -f log.txt src/*.gch
