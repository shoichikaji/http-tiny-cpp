main: http-tiny.o main.o
	g++ -o $@ $^ -lcurl

main.o: eg/main.cpp
	g++ -Ilib -c $<

http-tiny.o: lib/http-tiny.cpp
	g++ -c $<

clean:
	rm -f *.o main
