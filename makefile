CXX      := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -Iinclude -Isrc -g -pthread

TARGET   := main
SRC      := $(wildcard src/*.cpp)
OBJDIR   := objs
OBJS      := $(SRC:src/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all clean