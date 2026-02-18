#ifndef _GAMECAMERA_ANIMATED_H_
#define _GAMECAMERA_ANIMATED_H_

#ifdef	DOMI_ANIM_CAM

#include "../GameCamera.h"

class AnimatedCamera;

typedef void (*func)(AnimatedCamera&);

class CGameCameraAnimated : public CGameCamera
{
	I3_CLASS_DEFINE(CGameCameraAnimated, CGameCamera);

public:
	CGameCameraAnimated();
	virtual ~CGameCameraAnimated();

	void OnUpdate(RT_REAL32 tm);
	void AddScene(std::string key, std::string path);
	void RemoveScene(std::string path);
	void Start(const std::string& file);
	void End();
	BOOL IsPlaying();
	void ChangeMode(func modeStuff);
	BOOL Empty();

	AnimatedCamera* GetCamera() const { return m_camera; }

private:
	AnimatedCamera*	m_camera;
};

#endif

#endif