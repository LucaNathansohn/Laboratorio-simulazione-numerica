CC = g++
CFLAGS = -Wall -O3 --std=c++11

main.exe : main.o random.o Individuo.o Popolazione.o
	$(CC) Individuo.o Popolazione.o random.o main.o -o main.exe $(AFLAGS)
main.o : main.cpp Individuo.h Popolazione.h random.h
	$(CC) -c main.cpp -o main.o $(CFLAGS)
Individuo.o : Individuo.cpp Individuo.h
	$(CC) -c Individuo.cpp -o Individuo.o $(CFLAGS)
Popolazione.o : Popolazione.cpp Popolazione.h Individuo.h
	$(CC) -c Popolazione.cpp -o Popolazione.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
clean :
	rm *.o main.exe seed.out