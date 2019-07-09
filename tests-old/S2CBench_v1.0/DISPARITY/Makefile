#==========================================================================
#
# File Name    : Makefile
# Description  : Makefile for disparity estimation
# Release Date : 16/06/13
# Author       : PolyU DARC Lab
#
# Note         : Edit SYSTEMC_HOME path
# Revision History
#---------------------------------------------------------------------------
# Date      Version  Author    Description
#---------------------------------------------------------------------------
# 16/17/13     1.0   PolyU     Makefile for dispariy estimator
#==========================================================================

TARGET	= disparity.exe

SRCS	=  \
	main.cpp \
	tb_disparity.cpp \
	disparity.cpp \
	pre_process.cpp 

HDRS	=  \
	disparity_top.h \
	disparity.h \
	define.h \
	pre_process.h \
	tb_disparity.h


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
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

tb_disparity.o: tb_disparity.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

disparity.o: disparity.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

pre_process.o: pre_process.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@

clean :
	@$(RM) -f $(OBJS) $(TARGET)

depend :
	makedepend -- $(INCL) $(CFLAGS) -- -Y -fMakefile $(SRCS)

