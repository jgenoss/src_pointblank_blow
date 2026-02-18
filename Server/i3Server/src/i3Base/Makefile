GCCROOT		=	$(PSPSDK)/sce/psp/gcc-0.7.3
BINDIR		=	$(GCCROOT)/bin
STDINC		=	$(GCCROOT)/psp/include
STDLIB		=	$(GCCROOT)/psp/lib
GCCLIB_ROOT	=	$(GCCROOT)/lib/gcc-lib/psp/3.2.3-psp-0.7.3
GCCLIB_INC	=	$(GCCLIB_ROOT)/include
GCCLIB_LIB	=	$(GCCLIB_ROOT)
TC_ROOT		=	$(PSPSDK)
TC_INC		=	$(TC_ROOT)/sce/psp/include
TC_LIB		=	$(TC_ROOT)/sce/psp/lib
OUTDIR		=	$(I3_ROOT)/lib/PSP/Debug
I3BASE_INC	=	$(I3_ROOT)/include/i3Base
I3BASE_SRC	=	$(I3_ROOT)/src/i3Base

CC			=	psp-gcc
CXX			=	psp-g++
AR			=	psp-ar
RANLIB		=	psp-ranlib
MAKE		=	make

DEFS		=	-DI3_PSP -DI3_COMPILER_GCC -DI3G_PSP -DI3_DEBUG
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_ROOT)/include -I$(I3_ROOT)/include/i3Base

TARGET		=	$(OUTDIR)/libi3Base.a
OBJS		=	i3CommonType.o i3Memory.o i3Debug.o i3String.o \
				i3ElementBase.o i3PersistantElement.o i3NamedElement.o \
				i3List.o i3BinList.o i3Stream.o i3Lexer.o i3Thread.o i3ResourceFile.o \
				i3ClassMeta.o i3MetaTemplate.o i3BaseMeta.o i3Timer.o

CFLAGS		= -O0 -Wall $(DEFS) $(INCS)

vpath %.cpp		\
			$(I3BASE_SRC)
			
.PHONY: all clean

all:	$(TARGET)

rebuild :
	@-rm -f $(TARGET) *.o *~ *.bak
	$(MAKE) -f i3Base_PSP.mak

$(TARGET): $(OBJS)
	$(AR) crv $@ $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -c $<
	
.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	@-rm -f $(TARGET) *.o *~ *.bak
	
depend:
	makedepend -- $(CFLAGS) -- *.c *.cpp

#
%.o : $(I3BASE_INC)/%.h $(I3BASE_INC)/i3CommonType.h $(I3BASE_SRC)/%.cpp

DEPENDENCY_FILES:= $(wildcard *.d)
ifneq ($(strip $(DEPENDENCY_FILES)),)
include $(DEPENDENCY_FILES)
endif