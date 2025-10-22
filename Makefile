DEBUG = -D   _DEBUG_

###### C
 CC = gcc
 COPTS = -Wall -fPIC -O0

###### C++
 CXX = g++
 CXXOPTS = -fPIC -O2 -Wall

 COPTS += -g
 CXXOPTS += -g

###### libraries
 LIBS = -lm -lstdc++


all:
	$(CXX) -c $(DEBUG) $(CXXOPTS) incg_smesh.cpp
	$(CXX) -c $(DEBUG) $(CXXOPTS) incg_smesh_uid_factory.cpp
	$(CC) -c $(DEBUG) $(COPTS) incg_utils.c
	$(CC) -c $(DEBUG) $(COPTS) incg_tet.c
	$(CC) -c $(DEBUG) $(COPTS) incg_tri.c
	$(CC) -c $(DEBUG) $(COPTS) incg_arclength.c
	$(CC) -c $(DEBUG) $(COPTS) incg_mesh.c
	$(CC)    $(DEBUG) $(COPTS) test.c \
            incg_tet.o incg_utils.o incg_tri.o incg_mesh.o \
            incg_smesh.o incg_smesh_uid_factory.o \
            $(LIBS)

doc:
	doxygen Doxyfile

clean:
	rm -f *.o *.a a.out
	rm -Rf doxygen_doc


