
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3FRAMEWORK_INC)

TARGET		=	$(OUTDIR)/libi3Framework.a

OBJS		=	i3AI.o i3Framework.o i3FrameworkMeta.o i3FrameworkPCH.o i3GameNode.o i3GameObj.o i3GameObjBase.o \
				i3GameRes.o i3GameResAnimation.o i3GameResSceneGraph.o i3GameResTexture.o i3ResourceManager.o \
				i3Stage.o i3Viewer.o i3ViewerMode.o i3ViewerModeDefault.o i3FontManager.o i3StageStack.o 

include		../Common/makebase

vpath %.cpp		\
			$(I3FRAMEWORK_SRC)
			

