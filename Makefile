CC = g++
STD = -std=c++17

test-libjournal: libjournal
	$(CC) -o test-lib-journal -Iinclude -ljournal -L. $(STD) src/main.cpp src/SignalAction.cpp src/WorkerThread.cpp -Wl,-rpath,'$$ORIGIN'

libjournal:
	$(CC) -c -fPIC $(STD) src/libjournal.cpp
	$(CC) -o libjournal.so -shared $(STD) libjournal.o

.PHONY: clean
clean:
	rm -f *.o *.so test-lib-journal