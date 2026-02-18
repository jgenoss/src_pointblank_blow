#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "AnimatedCamera.h"
#include "CameraAnimation.h"
#include "Behaviors.h"
#include "OutsideEvents.h"


AnimatedCamera::AnimatedCamera() : m_camera(0),
								   m_play(0),
								   m_stop(0),
								   m_updator(0),
								   m_notifier(0),
								   m_outsideEventSubject(0)
																		
{
	m_root = i3Node::new_object();
	m_attrSet = i3AttrSet::new_object();
	m_attrSet->AddChild(m_root);
	g_pFramework->Get3DRoot()->AddChild(m_attrSet);

	m_play = new PlayIfNoAnimPlaying;
	m_stop = new ReturnTo1PV;
	m_updator = new UpdatorDefault;
	m_notifier = new NotifierAtAnimEnd;
}

AnimatedCamera::~AnimatedCamera()
{
	if (m_root)
	{
		m_attrSet->RemoveChild(m_root);
		I3_SAFE_RELEASE(m_root);
	}

	if (m_attrSet)
	{
		m_attrSet->RemoveFromParent();
		m_attrSet->RemoveAllAttrs();
		I3_SAFE_RELEASE(m_attrSet);
	}

	delete m_play;		m_play = 0;
	delete m_stop;		m_stop = 0;
	delete m_updator;	m_updator = 0;
	delete m_notifier;	m_notifier = 0;

	DeleteAllAnim();

	DetachOutsideEvent();
}

void AnimatedCamera::Start(const std::string& key)
{
	m_play->Update(key, *this);
}

void AnimatedCamera::End()
{
	m_stop->Update(*this);
	m_notifier->Reset();
}

void AnimatedCamera::Update()
{
	m_updator->Update(*this);
	m_notifier->Update(*this);

	NotifyOutsideEvent();
}

void AnimatedCamera::ChangeAnimation(CameraAnimation* anim)
{
	if (!anim) return;

	m_root->RemoveAllChild();
	m_root->AddChild(anim->GetNode());
	
	i3Node* transform = m_root->GetChild(0)->GetChild(0)->GetChild(0);
	static_cast<i3Transform*>(transform)->SetPlayMode(I3_ANIM_PLAY_MODE_CLAMP);
	static_cast<i3Transform*>(transform)->SetPlayCount(0);
	static_cast<i3Transform*>(transform)->SetPlayTime(0.0f);

	m_camera = static_cast<i3Camera*>(i3Scene::FindNodeByExactType(m_root->GetChild(0), i3Camera::static_meta()));
	g_pFramework->getSgContext()->setCurrentCamera(m_camera);

	m_curAnimName = anim->GetFile();
}

BOOL AnimatedCamera::NoAnimPlaying()
{
	if (m_root->GetChildCount() == 0) return TRUE;

	i3Node* transform = m_root->GetChild(0)->GetChild(0)->GetChild(0);
	REAL32 playTime = static_cast<i3Transform*>(transform)->getPlayTime();
	REAL32 duration = static_cast<i3Transform*>(transform)->GetDuration();

	return (playTime > duration) ? TRUE : FALSE;
}

void AnimatedCamera::Insert(std::string key, std::string filePath)
{
	m_container.insert(std::make_pair(key, new CameraAnimation(filePath)));
}

void AnimatedCamera::Delete(const std::string& file)
{
	std::map<std::string, CameraAnimation*>::iterator pos;
	pos = m_container.find(file);
	if (pos != m_container.end())
	{
		delete pos->second;
		m_container.erase(pos);
	}
}

BOOL AnimatedCamera::Find(const std::string& file)
{
	std::map<std::string, CameraAnimation*>::iterator pos;
	pos = m_container.find(file);
	if (pos != m_container.end())
		return TRUE;

	return FALSE;
}

void AnimatedCamera::DeleteAllAnim()
{
	std::map<std::string, CameraAnimation*>::iterator pos = m_container.begin();
	while (pos != m_container.end()) 
	{ 
		delete pos->second;
		m_container.erase(pos++);
	}
}

i3Node*	AnimatedCamera::GetCurAnim() const			
{
	if (m_root->GetChildCount() != 0)
		return m_root->GetChild(0);

	return NULL;
}

void AnimatedCamera::SetStartMode(Play* mode)
{
	delete m_play; m_play = 0;
	m_play = mode;
}

void AnimatedCamera::SetEndMode(Stop* mode)
{
	delete m_stop; m_stop = 0;
	m_stop = mode;
}

void AnimatedCamera::SetUpdateMode(Updator* mode)
{
	delete m_updator; m_updator = 0;
	m_updator = mode;
}

void AnimatedCamera::SetNotifier(Notifier* mode)
{
	delete m_notifier; m_notifier = 0;
	m_notifier = mode;
}

void AnimatedCamera::AttachOutsideEvent(OutsideEventSubject* subject)
{
	m_outsideEventSubject = subject;
}

void AnimatedCamera::DetachOutsideEvent()
{
	m_outsideEventSubject = 0;
}

void AnimatedCamera::NotifyOutsideEvent()
{
	if (m_outsideEventSubject)
		m_outsideEventSubject->Notify();
}

#endif