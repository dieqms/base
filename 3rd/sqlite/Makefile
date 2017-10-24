# Makefile

EXE=./libsqlite.a

OBJ=$(shell find . | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . | grep -E '\.(c|cpp)$$')

ifeq ($(ARCH),arm)
CC=arm-fsl-linux-gnueabi-gcc
AR=arm-fsl-linux-gnueabi-ar
else
CC= gcc
AR=ar
endif

CFLAGS= -I./include -fPIC -Wdeprecated-declarations
LDFLAGS=  

EXE:$(OBJ)
	$(AR) cr libsqlite.a *.o
	$(CC) -shared -fPIC *.c -I./include -o libsqlite.so

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(EXE)
