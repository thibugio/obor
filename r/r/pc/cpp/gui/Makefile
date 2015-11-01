CC=g++
CFLAGS=-g -Wall -std=c++11 

#holds *.h files
IDIR=inc/

# holds *.cpp files for *.h files
LIBDIR=lib/
LSRC=common_utils.cpp string_utils.cpp pc_utils.cpp
LIBSRC=$(foreach src,$(LSRC),$(LIBDIR)$(src))

SRCDIR=src/
SRC=main.cpp 

OUTDIR=bin/
TARGET=test

all:
	$(CC) $(CFLAGS) -I$(IDIR) -o $(OUTDIR)$(TARGET) $(SRCDIR)$(SRC) $(LIBSRC)

.PHONY: clean
clean:
	rm -f $(TARGET) *.o *~
