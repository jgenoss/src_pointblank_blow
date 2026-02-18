#include "stdafx.h"
#include "i3SoundH.h"
#include "i3SoundResManager.h"

//	by KOMET
static bool s_bRegisterSoundMeta = false;

I3_EXPORT_SOUND
void i3SoundRegisterMetas(void)
{
	if( s_bRegisterSoundMeta == false)
	{
		{
			i3SoundResManager * pManager = i3SoundResManager::new_object_ref();

			i3ResourceFile::AddResourceManager( i3Sound::static_meta(), pManager);
		}
	
		s_bRegisterSoundMeta = true;
	}
}


