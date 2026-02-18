I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Gui.a

OBJS		=	i3GuiWindow.o i3GuiBase.o i3GuiButton.o \
				i3GuiControl.o i3GuiDefines.o i3GuiMeta.o \
				i3GuiPCH.o i3GuiStatic.o 

INCS		=	-I$(I3GUI_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3GUI_SRC)

