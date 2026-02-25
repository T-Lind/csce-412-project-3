# Bizaco Load Balancer - Makefile
# Use: make [all] | clean
# Builds loadbalancer executable from src/*.cpp and include/*.h
# On Windows (MinGW): use "make" or "mingw32-make". On Linux/Mac: use "make".

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDE = -Iinclude
SRCDIR = src

SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/Config.cpp $(SRCDIR)/Request.cpp $(SRCDIR)/RequestQueue.cpp $(SRCDIR)/WebServer.cpp $(SRCDIR)/IPBlocker.cpp $(SRCDIR)/LoadBalancer.cpp $(SRCDIR)/Switch.cpp
OBJS = $(SRCDIR)/main.o $(SRCDIR)/Config.o $(SRCDIR)/Request.o $(SRCDIR)/RequestQueue.o $(SRCDIR)/WebServer.o $(SRCDIR)/IPBlocker.o $(SRCDIR)/LoadBalancer.o $(SRCDIR)/Switch.o

# use .exe suffix on Windows
ifeq ($(OS),Windows_NT)
  TARGET = loadbalancer.exe
else
  TARGET = loadbalancer
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(INCLUDE)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	-del /Q $(OBJS) $(TARGET) loadbalancer.exe 2>nul
	@echo Clean done.

.PHONY: all clean
