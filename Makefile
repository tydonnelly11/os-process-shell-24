###########################################################
#
# Makefile: hfsh24
#
###########################################################
.SUFFIXES: .h .c .cpp .l .o

CXX = g++
CFLAGS = -g 
LEX = flex
LIBS = -lfl -lpthread
RM = /bin/rm
RMFLAGS = -rf

PROGS = hfsh24
OBJS = hfsh24.o csapp.o lex.yy.o


$(PROGS): $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c $<

lex.yy.c: scan.l
	$(LEX) $<

test:
	./test-hfsh24.sh

test-all:
	./test-hfsh24.sh -c

clean:
	$(RM) $(RMFLAGS) *.o *~ hfsh24 lex.yy.c tests-out



