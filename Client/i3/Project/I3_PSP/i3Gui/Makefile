
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3GUI_INC)

TARGET		=	$(OUTDIR)/libi3Gui.a

OBJS		=	i3GuiBase.o i3GuiButton.o i3GuiControl.o i3GuiDefines.o i3GuiMeta.o i3GuiPCH.o i3GuiStatic.o i3GuiWindow.o
				

include		../Common/makebase

vpath %.cpp		\
			$(I3GUI_SRC)
			

