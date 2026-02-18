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
I3MATH_INC	=	$(I3_ROOT)/include/i3Base
I3GFX_INC	=	$(I3_ROOT)/include/i3Gfx
I3GFX_SRC	=	$(I3_ROOT)/src/i3Gfx

CC			=	$(BINDIR)/psp-gcc
CXX			=	$(BINDIR)/psp-g++
AR			=	$(BINDIR)/psp-ar
RANLIB		=	psp-ranlib

DEFS		=	-DI3_PSP -DI3_COMPILER_GCC -DI3G_PSP -DI3_DEBUG
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_ROOT)/include -I$(I3GFX_INC)

TARGET		=	$(OUTDIR)/libi3Gfx.a

OBJS		=	i3GFXType.o i3GfxUtil.o i3GfxUtilPSP.o i3GfxCaps.o i3GfxGeometryUtil.o i3GfxGlobalVariable.o \
				i3GfxResource.o i3IndexArray.o i3IndexArrayPSP.o  i3Clut.o i3ClutPSP.o \
				i3Texture.o i3TexturePSP.o i3TextureMem.o \
				i3VertexArray.o i3VertexArrayPSP.o \
				i3ImageFile.o i3ImageFileBMP.o i3ImageFileI3I.o i3ImageFileTGA.o i3ImageFileDDS.o \
				i3ImageUtil.o i3ImageUtil4Bits.o i3ImageUtil8Bits.o i3ImageUtil16Bits.o i3ImageUtil24Bits.o i3ImageUtil32Bits.o \
				i3Light.o i3PerformanceReport.o i3RenderContext.o \
				i3RenderContextPSP.o i3VideoInfo.o i3GfxMeta.o i3RenderTarget.o i3RenderTargetPSP.o

CFLAGS		= -O2 -Wall $(DEFS) $(INCS)

vpath %.cpp		\
			$(I3GFX_SRC)

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(AR) crv $@ $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -c $<
	
.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	@-rm -f $(TARGET) *.o *~ *.bak

#
%.o : $(I3GFX_INC)/%.h $(I3BASE_INC)/i3CommonType.h $(I3MATH_INC)/i3MathType.h $(I3GFX_SRC)/%.cpp
