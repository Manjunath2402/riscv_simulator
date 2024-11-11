CC = g++
TARGET = riscv_sim

all: $(TARGET)

$(TARGET): main.o sim_operations.o alu_operations.o cache_operations.o
	$(CC) $(CFLAGS) main.o sim_operations.o alu_operations.o cache_operations.o -o $(TARGET)

main.o: ./sourceFiles/main.cpp
	$(CC) $(CFLAGS) -c ./sourceFiles/main.cpp -o main.o

sim_operations.o: ./sourceFiles/sim_operations.cpp 
	$(CC) $(CFLAGS) -c ./sourceFiles/sim_operations.cpp -o sim_operations.o

alu_operations.o: ./sourceFiles/alu_operations.cpp
	$(CC) $(CFLAGS) -c ./sourceFiles/alu_operations.cpp -o alu_operations.o

cache_operations.o: ./sourceFiles/cache_operations.cpp
	$(CC) $(CFLAGS) -c ./sourceFiles/cache_operations.cpp -o cache_operations.o

clean:
	rm *.o 