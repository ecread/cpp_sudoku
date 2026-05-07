# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -I$(INC_DIR) -std=c++20 -MMD -MP
DEBUG_FLAGS = -g -O0

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# Files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/suku.cpp
# This transforms src/file.cpp into obj/file.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = sudoku

# Default Rule (release)
all: $(OBJ_DIR) $(TARGET)

# Debug Rule
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile .cpp files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Specify header dependencies
$(OBJ_DIR)/main.o: $(INC_DIR)/suku.h
$(OBJ_DIR)/suku.o: $(INC_DIR)/suku.h

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -rf obj $(TARGET)

.PHONY: all debug clean

#-include $(OBJECTS: .o=.d)