######################################################################################
#		I3NETWORK Makefile															 #	
#		2005. 6. 2.						Goat										 #
#		2005. 11. 8.		remaked by dfly79										 #
######################################################################################
I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI

include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Network.a

OBJS		=	i3NetworkDef.o \
				i3NetworkMeta.o	\
				i3NetworkContextWIPI.o \
				$(GCSNETWORK_LIB)
				## KTF GCSNetwork 라이브러리를 넣어줍니다.

INCS		=	-I$(I3NETWORK_INC) -I$(GCSNETWORK_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3NETWORK_SRC)
 
