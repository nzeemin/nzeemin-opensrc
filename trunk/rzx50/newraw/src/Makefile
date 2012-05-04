# Define the applications properties here:

APP_NAME = NEWRAW


# Define the compiler settings here:

CPP       = mipsel-linux-uclibc-g++.exe
CC        = mipsel-linux-uclibc-gcc.exe
AR        = mipsel-linux-uclibc-ar.exe
STRIP     = mipsel-linux-uclibc-strip.exe

SOURCE    = .

INCLUDE   = -I/dingux/mipsel-linux-uclibc/include -I/dingux/mipsel-linux-uclibc/include/SDL -I. -I$

DEFS      = -DSYS_LITTLE_ENDIAN -DUSE_SDL
W_OPTS    = -Wall -fPIC -fno-exceptions
CPP_OPTS  = -g -O3 $(INCLUDE) $(W_OPTS) $(DEFS) -c

LIB_PATH  = -L/dingux/lib -L/dingux/mipsel-linux-uclibc/lib
LIBS      = -lSDL -lc -lgcc -lstdc++

LD_OPTS   = -nodefaultlibs $(LIB_PATH) $(LIBS) $(DEFS) -o $(APP_NAME).dge


# Find all source files

SRC_CPP   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
OBJ_CPP   = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ       = $(OBJ_CPP)


# Compile rules.

.PHONY : all

all : $(APP_NAME).dge

$(APP_NAME).dge : $(OBJ)
	$(CC) $(OBJ) $(LD_OPTS)  &&  $(STRIP) $(APP_NAME).dge

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_OPTS) -c -o $@ $<


# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ) $(APP_NAME).dge
