install: multpair

clean:
	rm -f *.o multpair *~

OBJECTS = interface.o sequence.o showpair.o \
  	util.o trees.o gcgcheck.o pairalign.o \
  	calcgapcoeff.o calctree.o \
        readmat.o alnscore.o random.o 

HEADERS = general.h multpair.h

CC	= gcc
CFLAGS  = -c -O3 -g
LFLAGS	= -O3 -lm 

multpair : $(OBJECTS) multpair.o
	$(CC) -o $@ $(OBJECTS) multpair.o $(LFLAGS)

interface.o : interface.c $(HEADERS) param.h
	$(CC) $(CFLAGS) $*.c

readmat.o : readmat.c $(HEADERS) matrices.h
	$(CC) $(CFLAGS) $*.c

trees.o : trees.c $(HEADERS) dayhoff.h
	$(CC) $(CFLAGS) $*.c

.c.o :
	$(CC) $(CFLAGS) $?

