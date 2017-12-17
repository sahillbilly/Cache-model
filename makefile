default: output
output: cache.o
		g++ cache.o -o output
cache.o: cache.cpp
		g++ -c cache.cpp -o cache.o
clean:
		-rm -f cache.o output
