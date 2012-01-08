# Define the applications properties here:

APP_NAME = ColorLinesSdl


# Define the compiler settings here:

CPP       = mipsel-linux-uclibc-g++.exe
CC        = mipsel-linux-uclibc-gcc.exe

SOURCE    = .

INCLUDE   = -I/dingux/mipsel-linux-uclibc/include -I/dingux/mipsel-linux-uclibc/include/SDL -I$

W_OPTS    = -Wall -Wno-pointer-sign -fPIC -fno-exceptions
CPP_OPTS  = -g -O2 $(INCLUDE) $(W_OPTS) -c
CC_OPTS   = -g -O2 $(INCLUDE) $(W_OPTS) -c

LIB_PATH  = -L/dingux/lib -L/dingux/mipsel-linux-uclibc/lib
LIBS      = -lSDL -lc -lgcc

LD_OPTS   = -nodefaultlibs $(LIB_PATH) $(LIBS) -o $(APP_NAME).dge


# Find all source files

SRC_CPP   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
SRC_C     = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
OBJ_CPP   = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C     = $(patsubst %.c, %.o, $(SRC_C))
OBJ       = $(OBJ_CPP) $(OBJ_C)


# Compile rules.

.PHONY : all

all : $(APP_NAME).dge

$(APP_NAME).dge : $(OBJ)
	$(CC) $(OBJ) $(LD_OPTS)

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_OPTS) -c -o $@ $<

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_OPTS) -c -o $@ $<


# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ) $(APP_NAME).dge
