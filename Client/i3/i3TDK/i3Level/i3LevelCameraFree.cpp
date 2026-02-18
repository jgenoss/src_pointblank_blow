#include "stdafx.h"
#include "i3LevelCameraFree.h"

I3_CLASS_INSTANCE( i3LevelCameraFree);

i3LevelCameraFree::i3LevelCameraFree(void)
{
	SetName( "Perspective");
	SetPerspective( I3_DEG2RAD( 45.0f), 100, 100, 0.5f, 5000.0f);
}

