#
# $Id: Makefile,v 2.0 2018/10/30 09:19:00 scott Stab $
#
# Description: Makefile for test program
#
# $Log: Makefile,v $
#

ifdef D
  SEP := ,
  DEFS += $(patsubst %,-D%,$(subst $(SEP), ,$(D)))
endif

TARGET = test
SRC = .
MODULES = test
#RUN_ARGS = -arg1 -arg2

VERSION = 1.0
LABEL = V$(subst .,_,$(VERSION))
#DEFS += -DVERSION=$(VERSION)

# SystemC specific
DEFS += -DTIMEOFDAY
SCDIR = $(SYSTEMC)
LDFLAGS += -L$(SCDIR)/lib-macosx64
#ifeq ($(findstring c++11,$(CXXFLAGS)),)
  CXXFLAGS += -std=c++17 -g
#endif
SRC += $(SCDIR)/include
# squelch warning from SystemC sc_bit_proxies.h
CFLAGS += -Wno-strict-overflow
# application build -std=option must match SystemC library build
LDLIBS += -lsystemc -lpthread

OBJECTS = $(addsuffix .o,$(MODULES))
VPATH = $(subst ' ',:,$(SRC))

#OPT ?= -O3
#OPT += -ftree-vectorize -ffast-math
#MACH = -march=core2 -mfpmath=sse
CPPFLAGS += -MMD $(DEFS)
CPPFLAGS += $(patsubst %,-I%,$(SRC))
CFLAGS += $(MACH) $(OPT) -Wall
CXXFLAGS += $(CFLAGS)
#LDFLAGS += -static
#LDLIBS += -lrt

# Cancel version control implicit rules
%:: %,v
%:: RCS/%
%:: RCS/%,v
%:: s.%
%:: SCCS/s.%
# Delete default suffixes
.SUFFIXES:
# Define suffixes of interest
.SUFFIXES: .o .c .cc .cpp .h .hpp .d .mak

.PHONY: all
all: $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET) $(RUN_ARGS)

.PHONY: clean
clean:
	$(RM) $(wildcard *.o) $(wildcard *.d) $(TARGET) makeflags

.PHONY: vars
vars:
	@echo TARGET: $(TARGET)
	@echo VERSION: $(VERSION)
	@echo DEFS: $(DEFS)
	@echo SRC: $(SRC)
	@echo OBJECTS: $(OBJECTS)
	@echo MAKEFILE_LIST: $(MAKEFILE_LIST)

$(TARGET): $(OBJECTS)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(OBJECTS): $(MAKEFILE_LIST) # rebuild if MAKEFILEs change

$(OBJECTS): makeflags # rebuild if MAKEFLAGS change
# Select only command line variables
cvars = _$(strip $(foreach flag,$(MAKEFLAGS),$(if $(findstring =,$(flag)),$(flag),)))_
makeflags: FORCE
	@[ "$(if $(wildcard $@),$(shell cat $@),)" = "$(cvars)" ] || echo $(cvars)> $@
FORCE: ;

# Establish module specific dependencies
-include $(OBJECTS:.o=.d)
