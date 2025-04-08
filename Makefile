# Compiler and flags
CXX = g++
LDFLAGS = `pkg-config --cflags --libs opencv4`     -lpigpio -lrt -lpthread

# Source and output
SRC = stream_test.cpp
EXEC = motion_stream

# Default target: compile and link the program
all:
	$(CXX) $(SRC) -o $(EXEC) $(LDFLAGS)

# Clean up the executable
clean:
	rm -f $(EXEC)

# Run the program
run: $(EXEC)
	./$(EXEC)
