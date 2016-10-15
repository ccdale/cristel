# CC = gcc `mysql_config --cflags`
CC = gcc
# CFLAGS = -ggdb -I/usr/local/include -W -Wall -ansi `mysql_config --cflags`
CFLAGS = -ggdb -I/usr/local/include -W -Wall -ansi
# CFLAGS = -ggdb -I/usr/local/include -W -Wall
LD = $(CC)
# LDFLAGS = -L/usr/local/lib `mysql_config --libs`
LDFLAGS = -L/usr/local/lib
# LIBS = -largtable2 `mysql_config --libs`
LIBS = -largtable2 -lsqlite3
LDLIBS = -largtable2

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
DOBJ := $(SRC:.c=.d)

BINNAME = cristel

$(BINNAME): $(OBJ)
	        $(CC) -o $@ $(OBJ) $(LIBS)

include $(OBJ:.o=.d)

%.d: %.c
	        ./depend.sh $(CFLAGS) $*.c > $@

cristel.d: cristel.c
	        ./depend.sh $(CFLAGS) cristel.c >cristel.d

cristel.o: cristel.c defs.h macros.h
	        $(CC) $(CFLAGS) -o cristel.o -c cristel.c

clean:
	        -rm -f $(DOBJ) $(OBJ) $(BINNAME)

