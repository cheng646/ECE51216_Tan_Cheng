mySAT: dimacs_parser.o dpll.o main.o
	g++ -o mySAT dimacs_parser.o dpll.o main.o

dimacs_parser.o: dimacs_file_parser.c
	gcc -c dimacs_file_parser.c

dpll.o: dpll.cpp dpll.h
	g++ -c dpll.cpp -o dpll.o

main.o: main.cpp dpll.h
	g++ -c main.cpp -o main.o

clean:
	rm -f *.o mySAT