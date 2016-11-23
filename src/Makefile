CC=g++

# Use these flags to compile everything except database.cpp. That file contains all
# of the code with calls to the MySQL C API, which requires a different set of flags.

CFLAGS=-c $(shell wx-config --cflags) -I/usr/include/mysql -std=c++11 -pedantic
SOURCES=gendat.cpp gdw_TopFrame.cpp gdw_panel.cpp gdw_edit.cpp gdw_dialog.cpp db_row_set.cpp db_row_set_w.cpp

# Linker flags

LDFLAGS=$(shell wx-config --libs) $(shell mysql_config --libs)
OBJECTS=$(SOURCES:.cpp=.o) database.o
EXECUTABLE=gendat


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

database.o : database.cpp
	$(CC) -c $(shell mysql_config --cflags) -std=c++11 -pedantic database.cpp -o database.o

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

