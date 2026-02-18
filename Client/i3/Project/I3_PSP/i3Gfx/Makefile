
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3GFX_INC)

TARGET		=	$(OUTDIR)/libi3Gfx.a

OBJS		=	i3GfxType.o i3GfxUtil.o i3GfxUtilPSP.o i3GfxCaps.o i3GfxGeometryUtil.o i3GfxGlobalVariable.o \
				i3GfxResource.o i3IndexArray.o i3IndexArrayPSP.o  i3Clut.o i3ClutPSP.o \
				i3Texture.o i3TexturePSP.o i3TextureMem.o \
				i3VertexArray.o i3VertexArrayPSP.o i3VertexBlending.o \
				i3ImageFile.o i3ImageFileBMP.o i3ImageFileI3I.o i3ImageFileTGA.o i3ImageFileDDS.o \
				i3ImageUtil.o i3ImageUtil4Bits.o i3ImageUtil8Bits.o i3ImageUtil16Bits.o i3ImageUtil24Bits.o \
				i3ImageUtil32Bits.o i3GPUCommandList.o i3GPUCommandParserPSP.o \
				i3Light.o i3PerformanceReport.o i3RenderContext.o i3VRAMManager.o\
				i3RenderContextPSP.o i3VideoInfo.o i3GfxMeta.o i3RenderTarget.o i3RenderTargetPSP.o i3TextureSequence.o

include		../Common/makebase

vpath %.cpp		\
			$(I3GFX_SRC)
			

