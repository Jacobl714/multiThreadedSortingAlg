CC=cc
CFLAGS=-g
LFLAGS=-pthread

all: selectionSort.o selectionSortTest

selectionSort.o : selectionSort.c
	$(CC) $(CFLAGS) $^ -c -o $@

selectionSortTest: selectionSort.o
	$(CC) $(CFLAGS) selectionSortTest.c -o $@ $^ $(LFLAGS)

clean:
	rm -rf *.o selectionSort.o selectionSortTest
