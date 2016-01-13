CC=g++
CFLAGS=-c $(shell mysql_config --cflags) -std=c++11 -pedantic
LDFLAGS=$(shell mysql_config --libs)
SOURCES=test_db_connect.cpp database.cpp db_row_set.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test_db_connect


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

