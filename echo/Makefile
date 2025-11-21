CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = echo_client
SOURCES = echo_client.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

.PHONY: clean