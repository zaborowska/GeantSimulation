CC=g++
CFLAGS=-c -Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`
SOFLAGS = -shared
SOURCES=createHistograms.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=createHistograms
DICTIONARY=libCreateHistogramsDict.so

all: $(SOURCES) $(EXECUTABLE) $(DICTIONARY)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

$(DICTIONARY):
	rootcling -f $(DICTIONARY).cxx -c $(SOURCES) LinkDef.h
	$(CC) $(CFLAGS) -fPIC $(LDFLAGS) $(DICTIONARY).cxx -o $@.o
	$(CC) $(SOFLAGS) $(LDFLAGS)  -o $@ $(DICTIONARY).o
	rm $(DICTIONARY).o $(DICTIONARY).cxx

clean:
	rm -f ./*.so ./*pcm ./*.so.cxx ./*.o ./main
