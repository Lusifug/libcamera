CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

LIBCAMERA_DIR = /usr/include/libcamera

INCLUDES = -I$(LIBCAMERA_DIR)

LIBS = -lcamera -lcamera-base -lm -lpthread

TARGET = capture

SOURCES = main.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -g -o $(TARGET) $(SOURCES) $(LIBS) $(INCLUDES)

clean:
	rm -f $(TARGET)

.PHONY: clean
