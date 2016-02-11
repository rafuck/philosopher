all: philosopher
CPP      = clang-omp++
OBJECTS  = main.o
FLAGS    = -std=c++11 -O3 -Wall -Wextra

philosopher: $(OBJECTS)
	$(CPP) $^ $(FLAGS) -o $@
main.o: main.cpp
	$(CPP) $(FLAGS) $< -c -o $@

clean:
	rm -rf *.o philosopher
