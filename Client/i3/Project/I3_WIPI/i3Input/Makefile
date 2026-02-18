I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Input.a
OBJS		=	i3InputType.o \
				i3InputDevice.o \
				i3InputController.o i3InputControllerWIPI.o \
				i3InputKeyboard.o i3InputKeyboardWIPI.o \
				i3InputMouse.o i3InputMouseWIPI.o \
				i3InputDeviceManager.o i3InputDeviceManagerWIPI.o \
				i3InputGlobalVariable.o \
				i3InputMeta.o

INCS		=	-I$(I3INPUT_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3INPUT_SRC)

