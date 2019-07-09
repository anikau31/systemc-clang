#============================================================================
#
# File Name    : Makefile
# Description  : Makefile for snow 3G
# Release Date : 16/07/2013
# Author       : PolYU
# Author       : PolyU DARC Lab
#                Benjamin Carrion Schafer, Anushree Mahapatra
#
# Note         : Edit SYSTEMC_HOME path
# Revision History
#---------------------------------------------------------------------------
# Date        Version     Author          Description
#---------------------------------------------------------------------------
#16/17/13         1.0     PolyU DARC Lab   Makefile for Snow 3G
#===========================================================================

TARGET	= sort3G.exe

SRCS	=  \
	main.cpp \
	tb_snow_3G.cpp \
	snow_3G.cpp


HDRS	=  \
	tb_snow_3G.h \
	define.h \
	snow_3G.h

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

LDFLAGS	= $(SYSTEMC_LDFLAGS) -m32
LDFLAGS += -L"$(PATH)/lib"  
LIBS	= -lm $(SYSTEMC_LIBS)
LIBS += -lsystemc

OBJS	= $(notdir $(addsuffix .o,$(basename $(SRCS))))


$(TARGET) : $(OBJS)
	$(LINKER) -o "$@" $(LDFLAGS) $(OBJS) $(LIBS)

debug: $(OBJS)
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS) $(DEBUG)

wave:  $(OBJS) 
	$(LINKER) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS) $(WAVEFLAG)

main.o: main.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL)  -c $< -o $@

tb_snow_3G.o: tb_snow_3G.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

snow_3G.o: snow_3G.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@


clean :
	@$(RM) -f $(OBJS) $(TARGET)

depend :
	makedepend -- $(INCL) $(CFLAGS) -- -Y -fMakefile $(SRCS)

