IDIR =.
CC=gcc
CFLAGS=-I$(IDIR) -std=c99 -g -Wall

ODIR=build
LDIR =../lib

LIBS=-lm -ledit

_DEPS = mpc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = mpc.o parsing.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


#Pattern rules don't affect default rule
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

lispy: $(OBJ)
	CC -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f lispy $(ODIR)/*.o *~ core $(INCDIR)/*~ 
