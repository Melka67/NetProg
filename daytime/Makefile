CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = daytime_client
SOURCES = daytime_client.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

.PHONY: clean