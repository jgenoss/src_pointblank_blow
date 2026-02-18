I3LIBDIR	=	$(I3_ROOT)/lib/PSP/Debug
I3LIBS		=	$(I3LIBDIR)/libi3Base.a $(I3LIBDIR)/libi3Math.a $(I3LIBDIR)/libi3Gfx.a $(I3LIBDIR)/libi3Scene.a

CC			=	$(GCCROOT)/bin/psp-gcc
CXX			=	$(GCCROOT)/bin/psp-g++
LD			=	$(GCCROOT)/bin/psp-ld
MAKE		=	make

DEFS		=	-DI3_PSP -DI3_COMPILER_GCC -DI3G_PSP -DI3_DEBUG
INCS		=	-I$(I3_ROOT)/include -I$(I3_ROOT)/include/i3Base
LIBFLAG		=	-L$(I3LIBDIR)

TARGET		= i3Viewer
OBJS		= pspmain.o

LOADLIBES	=  $(PSPSDK)/devkit/lib/ctrl_stub.a
LOADLIBES	+= $(PSPSDK)/devkit/lib/display_stub.a
LOADLIBES	+= $(PSPSDK)/devkit/lib/ge_user_stub.a
LOADLIBES	+= $(PSPSDK)/devkit/lib/rtc_stub.a

CFLAGS		= -O2 -g -MD -Wall $(DEFS) $(INCS)
LFLAGS		= $(LIBFLAG) -li3Scene -li3Gfx -li3Input -li3Math -li3Base -lvfpu -lgu -lm
			
.PHONY: all clean rebuild

all:	$(TARGET).prx

$(TARGET).prx: $(OBJS)
	$(CXX) $^ $(LFLAGS) $(LOADLIBES) -startfiles -o $@ 
	psp-prx-strip $@ -o i3Viewer_strip.prx

$(TARGET).elf : $(OBJS)
	$(CXX) $^ $(LFLAGS) $(LOADLIBES) -zgenelf -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<
	
.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	@rm -f *~ *.o *.d *.lst *.map
	
rebuild:
	$(MAKE) clean
	$(MAKE)

#
%.o : %.cpp %.c

DEPENDENCY_FILES:= $(wildcard *.d)
ifneq ($(strip $(DEPENDENCY_FILES)),)
include $(DEPENDENCY_FILES)
endif