
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3ZLIB_INC)

TARGET		=	$(OUTDIR)/libi3ZLib.a

OBJS		=	adler32.o compress.o deflate.o i3ZLib.o infback.o inffast.o inflate.o inftrees.o \
				trees.o uncompr.o zutil.o
#OBJS		=	i3ZLib.o

include		../Common/makebase

vpath %.cpp		\
			$(I3ZLIB_SRC)
			

			

