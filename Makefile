CC := g++ -Wno-pointer-arith
DIRB := C:\Users\USER\Documents\make
DIRC := C:\Users\USER\Documents\GitHub\Processor
SRC := $(DIRC)\main.cpp $(DIRC)\processor.cpp $(DIRC)\stack.cpp $(DIRC)\text-sort.cpp

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $(DIRB)\$@

run: $(DIR)\main.exe
	$(DIR)\main.exe

