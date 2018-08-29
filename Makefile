CC=c++
CFLAGS=-I. -g -Wall -std=c++11
OBJ = main.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

indentation-changer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o indentation-changer
