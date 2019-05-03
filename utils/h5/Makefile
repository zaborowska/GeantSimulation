CC=h5c++
CFLAGS=-c -Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`
SOFLAGS = -shared
SOURCES=h52root.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=h52root
DICTIONARY=libH5Dict.so

all: $(SOURCES) $(EXECUTABLE) $(DICTIONARY)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(DICTIONARY):
	rootcling -f $(DICTIONARY).cxx -c $(SOURCES) LinkDef.h
	$(CC) $(CFLAGS) -fPIC $(LDFLAGS) $(DICTIONARY).cxx -o $@.o
	$(CC) $(SOFLAGS)  -shlib $(LDFLAGS)  -o $@ $(DICTIONARY).o
	rm $(DICTIONARY).o $(DICTIONARY).cxx

clean:
	rm -f ./*.so ./*pcm ./*.so.cxx ./*.o ./main
