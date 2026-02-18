I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Gfx.a

OBJS		=	i3GfxType.o i3GfxUtil.o i3GfxUtilOGLES.o i3GfxCaps.o i3GfxGeometryUtil.o i3GfxGlobalVariable.o \
				i3GfxResource.o i3IndexArray.o i3IndexArrayOGLES.o  i3Clut.o i3ClutOGLES.o \
				i3Texture.o i3TextureOGLES.o i3TextureMem.o \
				i3VertexArray.o i3VertexArrayOGLES.o i3VertexBlending.o \
				i3ImageFile.o i3ImageFileBMP.o i3ImageFileI3I.o i3ImageFileTGA.o i3ImageFileDDS.o \
				i3ImageUtil.o i3ImageUtil4Bits.o i3ImageUtil8Bits.o i3ImageUtil16Bits.o i3ImageUtil24Bits.o \
				i3ImageUtil32Bits.o i3GPUCommandList.o \
				i3Light.o i3PerformanceReport.o i3RenderContext.o i3ShaderCode.o \
				i3RenderContextOGLES.o i3VideoInfo.o i3GfxMeta.o i3RenderTarget.o i3RenderTargetOGLES.o i3TextureSequence.o

INCS		=	-I$(I3GFX_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3GFX_SRC)
			
