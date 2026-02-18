
INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3_PSP_ROOT)/include/i3Input

TARGET		=	$(OUTDIR)/libi3Input.a
OBJS		=	i3InputType.o \
				i3InputDevice.o \
				i3InputController.o i3InputControllerPSP.o \
				i3InputKeyboard.o i3InputKeyboardPSP.o \
				i3InputMouse.o i3InputMousePSP.o \
				i3InputDeviceManager.o i3InputDeviceManagerPSP.o \
				i3InputGlobalVariable.o \
				i3InputMeta.o

include		../Common/makebase

vpath %.cpp		\
			$(I3INPUT_SRC)

