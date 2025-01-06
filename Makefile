CXX := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -O2

LIB_NAME := libFilmMaster2000.a
EXE_NAME := runme

SRCS := main.cpp FilmMaster2000.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(LIB_NAME) $(EXE_NAME)

$(LIB_NAME): FilmMaster2000.o
	@echo "Creating static library: $@ ..."
	ar rcs $@ FilmMaster2000.o

$(EXE_NAME): main.o $(LIB_NAME)
	@echo "Linking executable: $@ ..."
	$(CXX) $(CXXFLAGS) main.o -L. -lFilmMaster2000 -o $@

%.o: %.cpp
	@echo "Compiling $< ..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	@echo "Running test ..."
	./runme input.bin output.bin -S reverse

clean:
	@echo "Cleaning up ..."
	rm -f *.o $(EXE_NAME) $(LIB_NAME)

.PHONY: clean test
