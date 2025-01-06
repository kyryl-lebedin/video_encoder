CXX := g++
CXXFLAGS := -Wall -Wextra 

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
	./runme input.bin output.bin -S reverse

clean:
	rm -f *.o $(EXE_NAME) $(LIB_NAME)

.PHONY: clean test
