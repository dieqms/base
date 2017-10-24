# Makefile

OUT=./lib/libbase.a

OBJ=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$')

ifeq ($(ARCH),arm)
CC=arm-fsl-linux-gnueabi-g++
AR=arm-fsl-linux-gnueabi-ar
else
CC= g++
AR=ar
endif

CFLAGS= -I./include -I./3rd/sqlite -std=c++0x -fPIC -Wdeprecated-declarations
LDFLAGS=  -Wl,-dn -lsqlite

OUT:$(OBJ)
	$(AR) cr $(OUT) ./src/*.o
#	$(CC) -shared -fPIC *.c -I./include -o libbase.so

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(OUT)
