CXX=clang++
CXXFLAGS=-std=c++20 -lglog
TESTFLAGS=-lgtest -lgtest_main -pthread

ODIR=obj
SRCDIR=src
DEPS=$(SRCDIR)/*.hpp

_OBJ = TcpConnection.o TcpServer.o Main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TOBJ = TestCache.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

test: $(TOBJ)
	$(CXX) -o test $^ $(CXXFLAGS) $(TESTFLAGS)

cache: $(OBJ)
	$(CXX) -o cache $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -rf test cache $(ODIR)/*.o
