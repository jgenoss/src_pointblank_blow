#ifndef _OUTSIDE_EVENT_H_
#define _OUTSIDE_EVENT_H_

#ifdef	DOMI_ANIM_CAM

class CGameCameraAnimated;
class OutsideEventObserver;
class AnimatedCamera;

class OutsideEventSubject
{
public:
	OutsideEventSubject(CGameCameraAnimated* cam);
	virtual ~OutsideEventSubject();

	virtual void Attach(OutsideEventObserver* obj);
	virtual void Detach(OutsideEventObserver* obj);
	virtual void Notify();

	AnimatedCamera* GetCamera() const { return m_camera; }

private:
	std::list<OutsideEventObserver*> m_observers;
	AnimatedCamera*	m_camera;
};



class OutsideEventObserver
{
public:
	OutsideEventObserver(OutsideEventSubject* subject, OutsideEventObserver* evt, const char* animName, REAL32 evtTime = 0.0f);
	virtual ~OutsideEventObserver();

	virtual void Update() = 0;

	void		ResetEvent()			{ m_eventDone = FALSE;}
	void		SetEventDone(BOOL done)	{ m_eventDone = done; }
	BOOL		IsEventDone() const		{ return m_eventDone; }
	REAL32		GetEventTime() const	{ return m_eventTime; }
	const char* GetAnimFileName() const { return m_animFileName; }

protected:
	OutsideEventSubject*	m_subject;
	OutsideEventObserver*	m_event;
	REAL32					m_eventTime;
	const char*				m_animFileName;
	BOOL					m_eventDone;
};


namespace
{
	static const INT32	deadtown6DayObjNetIdx = 1;
	static const REAL32 deadtown6DayObjExplosionTime = 13.0f;
	static const char*	deadtown6DayBossDeadCamAnim = "Object/domination/Camera/SceneB_Mutant_Death_Camera.i3s";
	static const char*  deadtown6DayBossWalkCamAnim = "Object/domination/Camera/SceneA_Mutant_Walk_StageOn_Camera.i3s";
}

class Deadtown6DayObjColumn : public OutsideEventObserver
{
public:
	Deadtown6DayObjColumn(OutsideEventSubject* subject)
		: OutsideEventObserver(subject, this, deadtown6DayBossDeadCamAnim, deadtown6DayObjExplosionTime) {}
	virtual ~Deadtown6DayObjColumn() {}

	void Update();
};

class Deadtown6DayBossDeadStart : public OutsideEventObserver
{
public:
	Deadtown6DayBossDeadStart(OutsideEventSubject* subject)
		: OutsideEventObserver(subject, this, deadtown6DayBossDeadCamAnim, 0.0f) {}
	virtual ~Deadtown6DayBossDeadStart() {}

	void Update();
};

class Deadtown6DayBossWalkStart : public OutsideEventObserver
{
public:
	Deadtown6DayBossWalkStart(OutsideEventSubject* subject)
		: OutsideEventObserver(subject, this, deadtown6DayBossWalkCamAnim, 0.0f) {}
	virtual ~Deadtown6DayBossWalkStart() {}

	void Update();
};

#endif

#endif