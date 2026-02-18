#include "stdafx.h"
#include "i3LevelCameraFront.h"

I3_CLASS_INSTANCE( i3LevelCameraFront);//, i3LevelCamera);

i3LevelCameraFront::i3LevelCameraFront()
{
	AddCameraFlag( I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE | I3_VIEWER_CAM_DISABLE_ROTATE );

	SetOrthogonal( 100, 100, 1.0f, 5000.0f );
	SetName( "Front");
}
