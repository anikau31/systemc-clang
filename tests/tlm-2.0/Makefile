TARGET_ARCH = linux64

CC = g++
OPT    = -O3 #-DPARSYSC
DEBUG  =    
CFLAGS = $(DEBUG) $(OPT) 


MODULE = out
SRCS = main.cpp 
CU_SRCS = 
OBJS = $(SRCS:.cpp=.o) $(CU_SRCS:.cu=.o)

include ./Makefile.defs
