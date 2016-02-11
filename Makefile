all: philosopher
CPP      = clang-omp++
OBJECTS  = main.o
PATH_LIB = -L/usr/local/lib
PATH_INC = -I/usr/local/include -I/usr/local/Cellar/libiomp/20150227/include/libiomp/
FLAGS    = -std=c++11 -O3

philosopher: $(OBJECTS)
	$(CPP) $(PATH_LIB) $^ $(FLAGS) -o $@
main.o: main.cpp
	$(CPP) $(PATH_INC) $(FLAGS) $< -c -o $@

clean:
	rm -rf *.o *.html map
