######################################################################################
#		I3Sound Makefile															 #	
#		2004. 10. 8						Goat										 #
######################################################################################

I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Sound.a

OBJS		=	i3Sound.o \
				i3SoundAt3Plus.o \
				i3SoundWav.o \
				i3SoundI3s.o \
				i3SoundMidi.o \
				i3SoundMp3.o \
				i3SoundMmf.o \
				i3SoundSpf.o \
				i3SoundVag.o \
				i3SoundVagThread.o \
				i3SoundStreamThread.o \
				i3SoundPlay.o \
				i3SoundPlayAt3Plus.o \
				i3SoundPlayI3s.o \
				i3SoundPlayMidi.o \
				i3SoundPlayMp3.o \
				i3SoundPlayWav.o \
				i3SoundPlayMmf.o \
				i3SoundPlaySpf.o \
				i3SoundPlayVag.o \
				i3SoundContext.o \
				i3SoundMeta.o

INCS		=	-I$(I3SOUND_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		\
			$(I3SOUND_SRC)

