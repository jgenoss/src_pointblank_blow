I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libzlib.a

OBJS		=	zutil.o adler32.o compress.o deflate.o i3ZLib.o \
				infback.o inffast.o inflate.o inftrees.o \
				trees.o uncompr.o

PLATFORM	=	WIPI

INCS		=	-I$(I3ZLIB_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3ZLIB_SRC)			
