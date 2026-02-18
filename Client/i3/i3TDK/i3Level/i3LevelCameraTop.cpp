#include "stdafx.h"
#include "i3LevelCameraTop.h"

//I3_CLASS_INSTANCE( i3LevelCameraTop, i3LevelCamera);
I3_CLASS_INSTANCE( i3LevelCameraTop);

i3LevelCameraTop::i3LevelCameraTop(void)
{
	AddCameraFlag( I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE | I3_VIEWER_CAM_DISABLE_ROTATE);

	SetOrthogonal( 100, 100, 1.0f, 5000.0f);

	SetName( "Top");
}

