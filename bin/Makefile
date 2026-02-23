LEDALIB = '/usr/local/LEDA'
LEDAINCL = '/usr/local/LEDA/incl'
INCL = '../incl'

shitfuck = 'g++ -O3 -c ../src/shortestAugmentedPath.cpp -I$(../incl) -I$(/usr/local/LEDA/incl)'

compile: clean main.o checkMaxFlow.o shortestAugmentedPath.o 
	g++ -O3 -o main -std=c++11 main.o shortestAugmentedPath.o checkMaxFlow.o -lleda -L$(LEDALIB)

main.o: ../src/main.cpp
	g++ -O3 -c ../src/main.cpp -I$(INCL) -I$(LEDAINCL)

checkMaxFlow.o: ../src/checkMaxFlow.cpp
	g++ -O3 -c ../src/checkMaxFlow.cpp -I$(INCL) -I$(LEDAINCL)

shortestAugmentedPath.o: ../src/shortestAugmentedPath.cpp
	g++ -O3 -c ../src/shortestAugmentedPath.cpp -I$(INCL) -I$(LEDAINCL)

clean:
	rm -f main *.o

run:
	./main

