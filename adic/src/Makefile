TESTS=test-vector2d test-line test-mesh
CXXFLAGS+=`dope-config --cflags`
LDFLAGS+=`dope-config --libs`
all: $(TESTS)

test-line: test-line.o line.o
	$(CXX) $(CXXFLAGS) test-line.o line.o -o $@

clean:
	rm *.o $(TESTS)

