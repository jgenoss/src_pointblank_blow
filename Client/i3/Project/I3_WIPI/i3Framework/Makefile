I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Framework.a
OBJS		=	i3Viewer.o i3ViewerMode.o i3ViewerModeDefault.o \
				i3AI.o i3FadeEffect.o \
				i3Framework.o i3ResourceManager.o i3Stage.o i3StageStack.o \
				i3GameRes.o i3GameResSceneGraph.o i3GameResAnimation.o i3GameResTexture.o \
				i3GameNode.o i3GameObj.o i3GameObjBase.o \
				i3FrameworkMeta.o i3FontManager.o

INCS		=	-I$(I3FRAMEWORK_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3FRAMEWORK_SRC)

