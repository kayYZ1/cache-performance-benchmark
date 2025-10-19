# Compiler to use
CC = clang++

# Compiler flags
# -std=c++11: Use C++11 standard
# -Wall -Wextra: Enable all warnings for robust code
# -O2: Optimize for performance
# -g: Include debugging symbols
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g

# Target executable name
TARGET = main

# Source files
SOURCES = main.cpp

# Object files (derived from sources)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets (not actual files)
.PHONY: all clean
