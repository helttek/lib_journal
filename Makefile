CC = g++
STD = -std=c++17

test-libjournal: libjournal
	$(CC) -o test-lib-journal -Iinclude -ljournal -L. $(STD) src/main.cpp src/ThreadSafeQueue.hpp -Wl,-rpath,'$$ORIGIN'

libjournal:
	$(CC) -c -fPIC $(STD) src/libjournal.cpp src/ThreadSafeQueue.hpp
	$(CC) -o libjournal.so -shared $(STD) libjournal.o

.PHONY: clean
clean:
	rm -f *.o *.so test-lib-journal