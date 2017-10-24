# Makefile

OUT=./lib/libbase.a

OBJ=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$')

ifeq ($(ARCH),arm)
CC=arm-fsl-linux-gnueabi-g++
AR=arm-fsl-linux-gnueabi-ar
LDFLAGS=  -Wl,-dn -lsqlite -L../3rd/lib_arm
else
CC= g++
AR=ar
LDFLAGS=  -Wl,-dn -lsqlite -L../3rd/lib_x64
endif

CFLAGS= -I./include -I./3rd/sqlite -std=c++0x -fPIC -Wdeprecated-declarations

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
