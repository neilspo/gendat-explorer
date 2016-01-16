CC=g++

# Use these flags to compile everything except database.cpp. That file contains all
# of the code with calls to the MySQL C API, which requires a different set of flags.

CFLAGS=-c $(shell wx-config --cflags) -I/usr/include/mysql -std=c++11 -pedantic
SOURCES=test_db_with_wxWidgets.cpp db_row_set.cpp db_row_set_w.cpp

# Linker flags

LDFLAGS=$(shell wx-config --libs) $(shell mysql_config --libs)
OBJECTS=$(SOURCES:.cpp=.o) database.o
EXECUTABLE=test_db_with_wxWidgets


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

database.o : database.cpp
	$(CC) -c $(shell mysql_config --cflags) -std=c++11 -pedantic database.cpp -o database.o

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

