GCCROOT		=	$(PSPSDK)/sce/psp/gcc-0.7.3
BINDIR		=	$(GCCROOT)/bin
STDINC		=	$(GCCROOT)/psp/include
STDLIB		=	$(GCCROOT)/psp/lib
GCCLIB_ROOT	=	$(GCCROOT)/lib/gcc-lib/psp/3.2.3-psp-0.7.3
GCCLIB_INC	=	$(GCCLIB_ROOT)/include
GCCLIB_LIB	=	$(GCCLIB_ROOT)
TC_ROOT		=	$(PSPSDK)/psp_runtime_libraries-0.1.5
TC_INC		=	$(TC_ROOT)/sce/psp/include
TC_LIB		=	$(TC_ROOT)/sce/psp/lib
OUTDIR		=	$(I3_ROOT)/lib/PSP/Debug
I3BASE_INC	=	$(I3_ROOT)/include/i3Base
I3MATH_INC	=	$(I3_ROOT)/include/i3Math
I3MATH_SRC	=	$(I3_ROOT)/src/i3Math
I3GFX_INC	=	$(I3_ROOT)/include/i3Gfx
I3GFX_SRC	=	$(I3_ROOT)/src/i3Gfx
I3SCENE_INC	=	$(I3_ROOT)/include/i3Scene
I3SCENE_SRC	=	$(I3_ROOT)/src/i3Scene

CC			=	$(BINDIR)/psp-gcc
CXX			=	$(BINDIR)/psp-g++
AR			=	$(BINDIR)/psp-ar
RANLIB		=	psp-ranlib

DEFS		=	-DI3_PSP -DI3_COMPILER_GCC -DI3G_PSP -DI3_DEBUG
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_ROOT)/include -I$(I3SCENE_INC)

TARGET		=	$(OUTDIR)/libi3Scene.a
OBJS		=	i3Node.o i3Transform.o i3Camera.o i3Geometry.o i3LODSelector.o i3NodeSelector.o i3AttrSet.o \
				i3Joint.o i3LightSet.o i3SceneObject.o \
				i3Animation.o i3AnimationState.o i3AttrStack.o i3AttrStackManager.o i3CommonNodeProcs.o \
				i3NodeProcs.o i3NodeStack.o i3SceneDebug.o i3SceneFile.o i3SceneMeta.o i3SceneTracer.o \
				i3Viewer.o \
				i3AbstractBoolAttr.o i3AlphaFuncAttr.o i3AlphaTestEnableAttr.o \
				i3BlendEnableAttr.o i3BlendModeAttr.o \
				i3ClippingEnableAttr.o i3ColorAttr.o \
				i3EdgeAntiAliasEnableAttr.o \
				i3FaceCullModeAttr.o i3FogColorAttr.o i3FogDensityAttr.o i3FogEnableAttr.o i3FogModeAttr.o i3FogNearFarAttr.o \
				i3GeometryAttr.o \
				i3LightAttr.o i3LightingEnableAttr.o \
				i3MaterialAttr.o i3ModelViewMatrixAttr.o \
				i3PointSpriteAttr.o i3ProjectMatrixAttr.o \
				i3RenderAttr.o i3RenderAttrListAttr.o i3RenderModeAttr.o \
				i3ShadeModeAttr.o i3StencilFuncAttr.o i3StencilOperationAttr.o i3StencilTestEnableAttr.o \
				i3TextureBindAttr.o i3TextureEnableAttr.o i3TextureFilterAttr.o i3TextureWrapAttr.o \
				i3VertexBlendEnableAttr.o i3VertexBlendMatrixListAttr.o i3VertexColorEnableAttr.o i3ViewMatrixAttr.o \
				i3ZFuncAttr.o i3ZTestEnableAttr.o i3ZWriteEnableAttr.o
				
				

CFLAGS		= -O0 -g -Wall $(DEFS) $(INCS)

vpath %.cpp		\
			$(I3SCENE_SRC)

.PHONY: all clean

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(AR) crv $@ $(OBJS)
	$(RANLIB) $@

.c.o:
	$(CC) $(CFLAGS) -c $<
	
.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	@-rm -f $(TARGET) *.o *~ *.bak

#
%.o : $(I3SCENE_INC)/%.h $(I3BASE_INC)/i3CommonType.h $(I3MATH_INC)/i3MathType.h $(I3SCENE_SRC)/%.cpp