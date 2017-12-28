# Makefile
include ../../config.mk
EXE=./test

OBJ=$(shell find . | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . | grep -E '\.(c|cpp)$$')

CFLAGS= -g -I../../include -std=c++0x -Wno-psabi 


LDFLAGS= -Wl,-dn -Wl,-dy -L./ -lbase -lrt
#LDFLAGS= -Wl,-dn -ljsoncpp -Wl,-dy
#LDFLAGS= 
#LDFLAGS= -WI,-Bdynamic -lpthread -Wdeprecated-declarations -WI,-Bstatic -ljsoncpp

#CFLAGS= -g -std=c++0x -I../common
#LDFLAGS= -lpthread

CC=g++
LDFLAGS+= -L../3rd/lib

EXE:$(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $^ -lrt $(LDFLAGS)

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(EXE)
