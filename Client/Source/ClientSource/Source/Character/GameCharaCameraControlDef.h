#if !defined( __GAME_CHARA_CAMERA_CONTROLDEF_H__)
#define __GAME_CHARA_CAMERA_CONTROLDEF_H__

enum CAMERA_CMD
{
	CAMERA_CMD_IDLE = 0,
	CAMERA_CMD_CHANGEFPS,		// Change FPS camera
	CAMERA_CMD_CHANGEKILL,		// Change kill camera
	CAMERA_CMD_CHANGEOBSERVER,	// Change observer camera
	CAMERA_CMD_SETOBSERVERZOOM,	// Set Observer zoom camera

	CAMERA_CMD_RESPAWN,
};

typedef struct __tagCameraParameter
{
	INT32	_nEnemyIdx;
} CameraParameter;

#endif
