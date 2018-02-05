# Makefile
include ./config.mk

OUT=./lib/libbase.a
OUT_SO=./lib/libbase.so

OBJ=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . ! -path "./test/*" ! -path "./3rd/*" | grep -E '\.(c|cpp)$$')

CC=${CROSS_COMPILE}g++
AR=${CROSS_COMPILE}ar
CFLAGS+= -I./include
CFLAGS+= -I./3rd/sqlite/install/include
CFLAGS+= -I./3rd/openssl/install/include 
CFLAGS+= -std=c++0x -fPIC -Wdeprecated-declarations
LDFLAGS+=  -Wl,-dn -lsqlite -L./3rd/sqlite/install/lib 
LDFLAGS+=  -lparson -L./3rd/parson/install/lib 
LDFLAGS+=  -Wl,-dy -ldl

OUT:$(OBJ)
	$(AR) cr $(OUT) ./src/*.o
	$(CC) -shared -fPIC ${OBJ} ${LDFLAGS} -o ${OUT_SO}

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(OUT) ${OUT_SO}
