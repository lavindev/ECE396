# compiles main.cpp png.cpp rgbapixel.cpp cut.cpp

COMPILER = g++
COMPILER_FLAGS = -c -g -O0 -Wall -Werror -std=c++11
LINKER = g++

dsa : main.o png.o rgbapixel.o cut.o
	$(LINKER) main.o png.o rgbapixel.o cut.o -lpng -o dsa

main.o : main.cpp png.h rgbapixel.h cut.h
	$(COMPILER) $(COMPILER_FLAGS) main.cpp

png.o : png.cpp png.h rgbapixel.h
	$(COMPILER) $(COMPILER_FLAGS) png.cpp

rgbapixel.o : rgbapixel.cpp rgbapixel.h
	$(COMPILER) $(COMPILER_FLAGS) rgbapixel.cpp

cut.o : cut.cpp cut.h
	$(COMPILER) $(COMPILER_FLAGS) cut.cpp

clean :
	-rm -f *.o mp1
