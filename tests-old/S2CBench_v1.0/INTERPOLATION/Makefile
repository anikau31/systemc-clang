#===========================================================================
#
# File Name    : Makefile
# Description  : Makefile for Interpolation filter 
# Release Date : 23/07/13
# Author       : PolYU
#
# Note         : Edit SYSTEMC_HOME path
# Revision History
#---------------------------------------------------------------------------
# Date     Version  Author         Description
#---------------------------------------------------------------------------
# 23/07/13    1.0    PolYU         Makefile
#===========================================================================

TARGET	= interpolation.exe

SRCS	=  \
	main.cpp \
	tb_interp.cpp \
	filter_interp.cpp


HDRS	=  \
	tb_interp.h \
	define.h \
	filter_interp.h

PATH            =/eda/bin/cwb/cyber
SYSTEMC_HOME	= $(PATH)/osci-dbg
ifeq (, $(findstring Windows,$(OS)))
SYSTEMC_ARCH	= linux
else
SYSTEMC_ARCH	= mingw
endif
SYSTEMC_INCL	= -I"$(SYSTEMC_HOME)/include"
SYSTEMC_LDFLAGS	= -L"$(SYSTEMC_HOME)/lib-$(SYSTEMC_ARCH)"
SYSTEMC_LIBS	= -lsystemc

EMPTY	=
SPACE	= $(EMPTY) $(EMPTY)
CC	= /usr/bin/g++
RM	= /bin/rm
LINKER	= $(CC)
INCL	= -I"." -I"$(PATH)/lib"
CFLAGS	= -m32
CFLAGS += -DNO_VPI
CFLAGS += -fpermissive

debug: CFLAGS += -g -DDEBUG
wave: CFLAGS += -DWAVE_DUMP
verbose: CFLAGS += -DPRINTD


LDFLAGS	= $(SYSTEMC_LDFLAGS) -m32
LDFLAGS += -L"$(PATH)/lib"  
LIBS	= -lm $(SYSTEMC_LIBS)
LIBS += -lsystemc

OBJS	= $(notdir $(addsuffix .o,$(basename $(SRCS))))


$(TARGET) : $(OBJS)
	$(LINKER) -o "$@" $(LDFLAGS) $(OBJS) $(LIBS)

debug: $(OBJS)
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)

wave:  $(OBJS) 
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS) 

verbose:  $(OBJS) 
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS) 


main.o: main.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL)  -c $< -o $@

tb_interp.o: tb_interp.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

filter_interp.o: filter_interp.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@


clean :
	@$(RM) -f $(OBJS) $(TARGET)

depend :
	makedepend -- $(INCL) $(CFLAGS) -- -Y -fMakefile $(SRCS)

