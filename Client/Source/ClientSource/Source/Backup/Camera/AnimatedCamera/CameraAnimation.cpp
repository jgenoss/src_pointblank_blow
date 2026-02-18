#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "CameraAnimation.h"


CameraAnimation::CameraAnimation(const std::string file)
{
	SetFile(file);
}

CameraAnimation::~CameraAnimation()
{
	m_node->RemoveFromParent();
	I3_SAFE_RELEASE(m_node);
}

void CameraAnimation::SetFile(const std::string file)
{
	m_file = file;

	i3SceneFile sceneFile;
	if (sceneFile.Load(m_file.c_str()) == STREAM_ERR)
	{
		I3WARN("Invalid I3S file (%s)", file);
		return;
	}

	m_node = sceneFile.GetSceneGraph();
	m_node->AddRef();

	static_cast<i3Transform*>(m_node->GetChild(0)->GetChild(0))->SetPlayMode(I3_ANIM_PLAY_MODE_CLAMP);
}

#endif