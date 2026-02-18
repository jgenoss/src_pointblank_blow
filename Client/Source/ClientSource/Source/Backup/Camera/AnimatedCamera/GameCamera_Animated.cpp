#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "GameCamera_Animated.h"
#include "CameraAnimation.h"
#include "AnimatedCamera.h"
#include "Behaviors.h"

I3_CLASS_INSTANCE(CGameCameraAnimated)//, CGameCamera);

CGameCameraAnimated::CGameCameraAnimated() : m_camera(0)
{
	m_camera = new AnimatedCamera;
}

CGameCameraAnimated::~CGameCameraAnimated()
{
	delete m_camera; m_camera = 0;
}

void CGameCameraAnimated::AddScene(std::string key, std::string path)
{
	m_camera->Insert(key, path);
}

void CGameCameraAnimated::RemoveScene(std::string path)
{
	m_camera->Delete(path);
}

void CGameCameraAnimated::Start(const std::string& key)
{
	if (m_camera->Find(key))
	{
		InitCamera(CGameCameraManager::i()->getTargetCharacter());
		CGameCameraManager::i()->SetCurrentCamera(this);

		m_camera->Start(key);

		CGameCameraManager::i()->SetOldCamera(g_pCamera);
		g_pCamera = this;

		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		myPlayer->Cmd_ChangeView(CHARA_VIEW_3PV);
	}
}

void CGameCameraAnimated::End()
{
	m_camera->End();
}

BOOL CGameCameraAnimated::IsPlaying()
{
	return !m_camera->NoAnimPlaying();
}

void CGameCameraAnimated::OnUpdate(RT_REAL32 tm)
{
	m_camera->Update();
}


void CGameCameraAnimated::ChangeMode(func modeStuff)
{
	if (modeStuff)
	{
		if (m_camera)
			modeStuff(*m_camera);
	}
}

BOOL CGameCameraAnimated::Empty()
{
	return m_camera->GetContainer().empty();
}

#endif