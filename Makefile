CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra 

LIB_NAME := libFilmMaster2000.a
EXE_NAME := runme

SRCS := main.cpp FilmMaster2000.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(LIB_NAME) $(EXE_NAME)

$(LIB_NAME): FilmMaster2000.o
	ar rcs $@ FilmMaster2000.o

$(EXE_NAME): main.o $(LIB_NAME)
	$(CXX) $(CXXFLAGS) main.o -L. -lFilmMaster2000 -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	./runme input.bin output.bin reverse
	./runme input.bin output.bin -S reverse
	./runme input.bin output.bin -M reverse
	./runme input.bin output.bin swap_channel 1,2
	./runme input.bin output.bin -S swap_channel 1,2
	./runme input.bin output.bin -M swap_channel 1,2
	./runme input.bin output.bin clip_channel 1 [10,200]
	./runme input.bin output.bin -S clip_channel 1 [10,200]
	./runme input.bin output.bin -M clip_channel 1 [10,200]
	./runme input.bin output.bin scale_channel 1 1.1
	./runme input.bin output.bin -S scale_channel 1 1.1
	./runme input.bin output.bin -M scale_channel 1 1.1 

clean:
	rm -f *.o $(EXE_NAME) $(LIB_NAME)
	
.PHONY: clean test
