#ifndef I3_SOUNDH_H__
#define I3_SOUNDH_H__

#include "i3Sound/i3SoundConfig.h"
#include "i3Sound/i3SoundDef.h"
#include "i3Sound/i3SoundMeta.h"
#include "i3Sound/i3Sound.h"

#if defined( I3SND_DSOUND)
	//#include "i3Sound/ogg/os_types.h"
	#include "i3Sound/ogg/ogg.h"
	#include "i3Sound/ogg/vorbisfile.h"
	#include "i3Sound/ogg/vorbisenc.h"
	#include "i3Sound/ogg/codec.h"
	#include "i3Sound/i3SoundDx.h"
#elif defined( I3SND_FMOD)
	#include "i3Sound/i3SoundFMOD.h"
#endif

#include "i3Sound/i3SoundUtil.h"
#include "i3Sound/i3SoundFile.h"
#include "i3Sound/i3SoundPlayState.h"
#include "i3Sound/i3SoundContext.h"
#include "i3Sound/i3SoundStream.h"

#include "i3Sound/i3SoundStreamThread.h"
#include "i3Sound/i3SoundResManager.h"

#if defined ( I3SND_DSOUND )
	#include "i3Sound/i3SoundStreamDx.h"
	#include "i3Sound/i3SoundContextDX.h"
	#include "i3Sound/i3SoundPlayStateDx.h"
	#include "i3Sound/i3SoundPlayThread.h"
	#include "i3Sound/i3SoundUpdateThread.h"
#elif defined( I3SND_FMOD)
	#include "i3Sound/i3SoundStreamFMOD.h"
	#include "i3Sound/i3SoundPlayStateFMOD.h"
	#include "i3Sound/i3SoundContextFMOD.h"
	#include "i3Sound/i3FMODGeometry.h"
#endif

#endif // I3_SOUNDH_H__

