# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# Targets
LIB_NAME := libFilmMaster2000.a
EXE_NAME := runme

# Source Files
SRCS := main.cpp FilmMaster2000.cpp
OBJS := $(SRCS:.cpp=.o)

############################
# Default target: all
############################
all: $(LIB_NAME) $(EXE_NAME)

############################
# Build the library
############################
$(LIB_NAME): FilmMaster2000.o
	@echo "Archiving $@ ..."
	ar rcs $@ FilmMaster2000.o

############################
# Build the executable
############################
$(EXE_NAME): main.o $(LIB_NAME)
	@echo "Linking $@ ..."
	$(CXX) $(CXXFLAGS) main.o -L. -lFilmMaster2000 -o $@

############################
# Pattern rule for .o files
############################
%.o: %.cpp FilmMaster2000.hpp
	@echo "Compiling $< ..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

############################
# Test (placeholder)
############################
test:
	@echo "Test target not yet implemented."

############################
# Clean
############################
clean:
	@echo "Cleaning build artifacts..."
	rm -f *.o $(EXE_NAME) $(LIB_NAME)

