output: solution.o
	g++ solution.o -o output

solution.o: solution.cpp
	g++ -c solution.cpp

clean:
	rm *.o