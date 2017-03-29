DEBUG = -D_NO_DEBUG_

###### C
CC = gcc
COPTS = -g -Wall -fPIC -O0

###### libraries
LIBS = -lm 


all:
	$(CC) -c $(DEBUG) $(COPTS) incg_utils.c
	$(CC) -c $(DEBUG) $(COPTS) incg_tet.c
	$(CC)    $(DEBUG) $(COPTS) test.c incg_tet.o incg_utils.o  $(LIBS)

doc:
	doxygen Doxyfile

clean:
	rm -f *.o *.a a.out
	rm -Rf doxygen_doc


