# Makefile

EXE=./test

OBJ=$(shell find . | grep -E '\.(c|cpp)$$' | sed -r s/[^.]+$$/o/)
SRC=$(shell find . | grep -E '\.(c|cpp)$$')

#CFLAGS= -I../3rd/json/include -I../common -std=c++0x -DUSE_JSON -Wno-psabi 
#CFLAGS= -I../3rd/json/include -I../common -std=c++0x -DUSE_JSON
#CFLAGS=  -I../3rd/json/include -I../common -std=c++0x -Wno-psabi  -DUSE_JSON
CFLAGS= -g -I../../util -std=c++0x -Wno-psabi 


LDFLAGS= -Wl,-dn -Wl,-dy
#LDFLAGS= -Wl,-dn -ljsoncpp -Wl,-dy
#LDFLAGS= 
#LDFLAGS= -WI,-Bdynamic -lpthread -Wdeprecated-declarations -WI,-Bstatic -ljsoncpp

#CFLAGS= -g -std=c++0x -I../common
#LDFLAGS= -lpthread

ifeq ($(ARCH),arm)
CC=arm-none-linux-gnueabi-g++
LDFLAGS+= -L../3rd/lib_arm 
else
CC= g++
LDFLAGS+= -L../3rd/lib_x64
endif

EXE:$(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $^ -lrt $(LDFLAGS)

OBJ:$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $^

%.o::%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	-$(RM) $(OBJ) $(EXE)
