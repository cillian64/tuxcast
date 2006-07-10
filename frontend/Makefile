main: backend.o main.o ../libraries/socket.o
	g++ -g -o frontend backend.o main.o ../libraries/socket.o

backend.o: backend.cpp
	g++ -c -g backend.cpp

main.o: main.cpp
	g++ -c -g main.cpp

../libraries/socket.o: ../libraries/socket.cpp
	make -C ../libraries/ socket.o

clean:
	rm -f *.o frontend
