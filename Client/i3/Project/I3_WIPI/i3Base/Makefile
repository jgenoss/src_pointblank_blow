I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Base.a
OBJS		=	i3CommonType.o i3Memory.o i3Debug.o i3String.o \
				i3MemoryPool.o i3FixedMemoryPool.o \
				i3ElementBase.o i3PersistantElement.o i3NamedElement.o \
				i3List.o i3BinList.o i3Stream.o i3Lexer.o i3Thread.o i3ResourceFile.o \
				i3ClassMeta.o i3MetaTemplate.o i3BaseMeta.o i3Timer.o \
				i3Performance.o i3IniParser.o i3ObjectPool.o \
				i3System.o i3NamedResourceManager.o i3PackNode.o i3PackFile.o \
				i3MemoryBuffer.o i3EventReceiver.o i3PlatformWIPI.o \
				i3StringTable.o i3CRC32.o i3MergedVolumeFile.o i3BaseUtil.o \
				i3ErrorHandler.o
				
INCS		=	-I$(I3BASE_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3BASE_SRC)
				
