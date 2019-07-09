#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#
# File Name    : Makefile
# Description  : Makefile for ADPCM encoder
# Release Date : 30/07/13
# Author       : PolYU
#
# Note         : Edit SYSTEMC_HOME path
# Revision History
#---------------------------------------------------------------------------
# Date     Author      Version     Description
#---------------------------------------------------------------------------
# 30/07/13   PolyU       1.0        Makefile for ADPCM encoder
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TARGET	= adpcm_encoder.exe

SRCS	=  \
	main.cpp \
	tb_adpcm_encoder.cpp \
	adpcm_encoder.cpp


HDRS	=  \
	tb_adpcm_encoder.h \
	define.h \
	adpcm_encoder.h

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
tb_adpcm_encoder.o: tb_adpcm_encoder.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@
adpcm_encoder.o: adpcm_encoder.cpp
	$(CC) $(CFLAGS) $(SYSTEMC_INCL) $(INCL) -c $< -o $@


clean :
	@$(RM) -f $(OBJS) $(TARGET)

depend :
	makedepend -- $(INCL) $(CFLAGS) -- -Y -fMakefile $(SRCS)

