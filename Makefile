main: lib/http-tiny.o eg/main.o
	g++ -o $@ $^ -lcurl

eg/main.o: eg/main.cpp
	g++ -Ilib -c $< -o $@

lib/http-tiny.o: lib/http-tiny.cpp
	g++ -c $< -o $@

clean:
	rm -f lib/*.o main
