#pragma once

#include "i3LevelCamera.h"

class I3_EXPORT_SCENE i3NavMeshCamera : public i3LevelCamera
{
	I3_CLASS_DEFINE( i3NavMeshCamera );

public:

private:
	VEC3D		m_lookat;
	VEC3D		m_eye;
	VEC3D		m_up;
};