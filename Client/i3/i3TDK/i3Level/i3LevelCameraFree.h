#if !defined( __I3_Level_CAMERA_FREE_H)
#define __I3_Level_CAMERA_FREE_H

#include "i3LevelCamera.h"

class I3_EXPORT_TDK i3LevelCameraFree : public i3LevelCamera
{
	I3_EXPORT_CLASS_DEFINE( i3LevelCameraFree, i3LevelCamera);
	
public:
	i3LevelCameraFree(void);
};

#endif