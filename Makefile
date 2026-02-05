CXX = g++
CXXFLAGS = -std=c++17 -I./include -O3 -fno-stack-protector -fPIC
LDFLAGS = -shared -lcrypto

# Only compile main.cpp which includes the others
SRCS = src/core/main.cpp

TARGET = src/core/libcore.so

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
