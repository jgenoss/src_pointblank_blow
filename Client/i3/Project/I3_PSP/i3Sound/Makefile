######################################################################################
#		I3Sound Makefile															 #	
#		2004. 10. 8						Goat										 #
######################################################################################

INCS		=	-I$(STDINC) -I$(GCCLIB_INC) -I$(TC_INC) -I$(I3_PSP_ROOT)/include -I$(I3SOUND_INC)

TARGET		=	$(OUTDIR)/libi3Sound.a

OBJS		=	i3Sound.o \
				i3SoundAt3Plus.o \
				i3SoundVag.o \
				i3SoundWav.o \
				i3SoundI3s.o \
				i3SoundMidi.o \
				i3SoundMp3.o \
				i3SoundStreamThread.o \
				i3SoundVagThread.o \
				i3SoundPlay.o \
				i3SoundPlayAt3Plus.o \
				i3SoundPlayI3s.o \
				i3SoundPlayMidi.o \
				i3SoundPlayMp3.o \
				i3SoundPlayVag.o \
				i3SoundPlayWav.o \
				i3SoundContext.o 

include		../Common/makebase

vpath %.cpp		\
			$(I3SOUND_SRC)

