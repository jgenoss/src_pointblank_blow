######################################################################################
#		I3NETWORK Makefile															 #	
#		2005. 6. 2.						Goat										 #
######################################################################################

INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3NETWORK_INC)

TARGET		=	$(OUTDIR)/libi3Network.a

OBJS		=	i3NetworkUser.o \
				i3NetworkRoom.o \
				i3NetworkTrans.o \
				i3NetworkContext.o 

include		../Common/makebase

vpath %.cpp		$(I3NETWORK_SRC)
 
