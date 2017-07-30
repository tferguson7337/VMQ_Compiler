# Makefile for Compiler Theory Virtual Machine for Quads

CPP = g++ -o $@

CPPFLAGS = 

vmq:	vmq.cpp storage.h quad.h
	$(CPP) $(CPPFLAGS) vmq.cpp

# pseudo-targets

clean:
	rm -f vmq *.o
