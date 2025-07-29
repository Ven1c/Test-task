# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -std=c++17 -fPIC
LDFLAGS = -ldl -pthread

# Пути
INCLUDE_DIR = include
SRC_DIR = src

# Исходники
LIB_SRC = $(SRC_DIR)/Journal.cpp $(SRC_DIR)/Message.cpp
LIB_HDR = $(INCLUDE_DIR)/JournalLib.h
APP_SRC = main.cpp

# Цели
all: lib.so main

lib.so: $(LIB_HDR) $(LIB_SRC)
	$(CXX) -shared -fPIC -o lib.so $(LIB_HDR) $(LIB_SRC)

main: $(APP_SRC) $(LIB_HDR)
	$(CXX) -o main $(APP_SRC) $(LIB_HDR) $(LDFLAGS)

clean:
	rm -f *.o core

