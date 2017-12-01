# Makefile
include ../../config.mk

OUT=./install/lib/libsqlite.a
OUT_SO=./install/lib/libsqlite.so

OBJ=$(shell find . | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . | grep -E '\.(c|cpp)$$')

CC=${CROSS_COMPILE}gcc
AR=${CROSS_COMPILE}ar

CFLAGS+= -fPIC -Wdeprecated-declarations
LDFLAGS+=  

OUT:$(OBJ)
	$(AR) cr ${OUT} *.o
	$(CC) -shared -fPIC ${OBJ} ${LDFLAGS} -o ${OUT_SO}

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(OUT) ${OUT_SO}
