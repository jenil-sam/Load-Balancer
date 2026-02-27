CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = loadbalancer
SRCS     = main.cpp LoadBalancer.cpp WebServer.cpp RequestQueue.cpp Switch.cpp
OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) loadbalancer.conf

clean:
	rm -f $(OBJS) $(TARGET)

docs:
	doxygen Doxyfile

.PHONY: all clean run docs
