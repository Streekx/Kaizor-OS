CXX = g++
CXXFLAGS = -Wall -std=c++17

TARGET = kaizor

SRC = main.cpp core/system.cpp window_manager/window_manager.cpp desktop_shell/desktop_shell.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET)
