#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "Behaviors.h"
#include "CameraAnimation.h"
#include "AnimatedCamera.h"
#include "UtilBattle.h"




void PlayDirect::Update(const std::string& key, AnimatedCamera& cam)	
{
	std::map<std::string, CameraAnimation*>::const_iterator pos;
	pos = cam.GetContainer().find(key);

	if (pos != cam.GetContainer().end())
		cam.ChangeAnimation(pos->second);
}

void PlayIfNoAnimPlaying::Update(const std::string& key, AnimatedCamera& cam)
{
	if (cam.NoAnimPlaying())
	{
		std::map<std::string, CameraAnimation*>::const_iterator pos;
		pos = cam.GetContainer().find(key);

		if (pos != cam.GetContainer().end())
			cam.ChangeAnimation(pos->second);
	}
}


std::deque<std::string> g_queueAnimSequence;

void PlayAnimSequence::Update(const std::string& key, AnimatedCamera& cam)
{
	std::map<std::string, CameraAnimation*>::const_iterator pos;
	pos = cam.GetContainer().find(key);
	if (pos != cam.GetContainer().end())
		g_queueAnimSequence.push_back(key);
}



void UpdatorDefault::Update(AnimatedCamera& cam)
{
}

void UpdatorAnimSequence::Update(AnimatedCamera& cam)
{
	if (!g_queueAnimSequence.empty())
	{
		if (cam.NoAnimPlaying())
		{
			std::string key = g_queueAnimSequence.front();

			std::map<std::string, CameraAnimation*>::const_iterator pos;
			pos = cam.GetContainer().find(key);
			if (pos != cam.GetContainer().end())
				cam.ChangeAnimation(pos->second);

			g_queueAnimSequence.pop_front();
		}
	}
}



void ReturnTo1PV::Update(AnimatedCamera& cam)
{
	if (cam.GetCurAnim() && cam.NoAnimPlaying())
	{
		cam.GetCurAnim()->RemoveFromParent();
		CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		myPlayer->Cmd_ChangeView(CHARA_VIEW_1PV);
	}
}

void ReturnTo1PVDirect::Update(AnimatedCamera& cam)
{
	if (cam.GetCurAnim())
	{
		cam.GetCurAnim()->RemoveFromParent();
		CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);
		CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		myPlayer->Cmd_ChangeView(CHARA_VIEW_1PV);
	}
}



void NotifierAtAnimStart::Update(AnimatedCamera& cam)
{
	i3Node* node = cam.GetCurAnim();
	if (node)
	{
		if (cam.NoAnimPlaying()) return;

		if (!IsCompleted())
		{
			I3TRACE("animation camera playing started! notify server\n");
			SetComplete(TRUE);
		}
	}
}

void NotifierAtAnimEnd::Update(AnimatedCamera& cam)
{
	i3Node* node = cam.GetCurAnim();
	if (node)
	{
		i3Transform* transform = static_cast<i3Transform*>(node->GetChild(0)->GetChild(0));
		REAL32 playTime = transform->getPlayTime();
		REAL32 duration = transform->GetDuration();

		if (!IsCompleted() && playTime >= duration)
		{
			g_pGameContext->PushKeyNetwork(GAME_KEY_MAP_SPECIALACTION);
			g_pGameContext->SetEvent(EVENT_KEY_PROCESS_NET);
			SetComplete(TRUE);
		}
	}
}

NotifierAtAnimDelayEnd::NotifierAtAnimDelayEnd(REAL32 delayTime)
{
	m_delayTime = delayTime;
}

void NotifierAtAnimDelayEnd::Update(AnimatedCamera& cam)
{
	i3Node* node = cam.GetCurAnim();
	if (node)
	{
		i3Transform* transform = static_cast<i3Transform*>(node->GetChild(0)->GetChild(0));
		REAL32 playTime = transform->getPlayTime();
		REAL32 duration = transform->GetDuration()+m_delayTime;

		if (!IsCompleted() && playTime >= duration)
		{
			g_pGameContext->PushKeyNetwork(GAME_KEY_MAP_SPECIALACTION);
			g_pGameContext->SetEvent(EVENT_KEY_PROCESS_NET);
			SetComplete(TRUE);
		}
	}
}

void NotifierSequenceMode::Update(AnimatedCamera& cam)
{
	i3Node* node = cam.GetCurAnim();
	if (node)
	{
		if (g_queueAnimSequence.size() == 1)
		{
			i3Transform* transform = static_cast<i3Transform*>(node->GetChild(0)->GetChild(0));
			REAL32 playTime = transform->getPlayTime();
			REAL32 duration = transform->GetDuration();

			if (!IsCompleted() && playTime >= duration)
			{
				g_pGameContext->PushKeyNetwork(GAME_KEY_MAP_SPECIALACTION);
				g_pGameContext->SetEvent(EVENT_KEY_PROCESS_NET);
				SetComplete(TRUE);
			}
		}
	}
}


namespace AnimCamModeStuff
{
	void SetDefaultMode(AnimatedCamera& cam)
	{
		cam.SetStartMode(new PlayIfNoAnimPlaying);
		cam.SetEndMode(new ReturnTo1PV);
		cam.SetUpdateMode(new UpdatorDefault);
		cam.SetNotifier(new NotifierAtAnimEnd);
	}

	void SetSequenceMode(AnimatedCamera& cam)
	{
		cam.SetStartMode(new PlayAnimSequence);
		cam.SetEndMode(new ReturnTo1PV);
		cam.SetUpdateMode(new UpdatorAnimSequence);
		cam.SetNotifier(new NotifierSequenceMode);
	}

	void SetDominationMode(AnimatedCamera& cam)
	{
		cam.SetStartMode(new PlayIfNoAnimPlaying);
		cam.SetEndMode(new ReturnTo1PVDirect);
		cam.SetUpdateMode(new UpdatorDefault);
		cam.SetNotifier(new NotifierAtAnimDelayEnd(2.0f));
	}
}

#endif