CXX      ?= g++
CXXFLAGS ?= -std=c++20
CPPFLAGS ?= -O3 -Wall -I. -Wno-conversion-null -Wno-deprecated-declarations -I/home/jammy/shared-folder/pacs-Examples/Examples/include -I/home/jammy/shared-folder/pacs-Examples/Examples/src/Utilities

EXEC     = main
LDFLAGS ?= -L/home/jammy/shared-folder/pacs-Examples/Examples/lib -Wl,-rpath=/home/jammy/shared-folder/pacs-examples/Examples/lib
LIBS ?=

all: $(EXEC)

%.o: %.cpp Matrix.hpp MatrixTraits.hpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

$(EXEC): main.o Matrix.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

clean:
	$(RM) *.o

distclean: clean
	$(RM) *~