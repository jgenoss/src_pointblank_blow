#ifndef _BEHAVIORS_H_
#define _BEHAVIORS_H_

#ifdef	DOMI_ANIM_CAM

class AnimatedCamera;


class Play
{
public:
	virtual void Update(const std::string& key, AnimatedCamera& cam) = 0;
};

class PlayDirect : public Play
{
public:
	virtual void Update(const std::string& key, AnimatedCamera& cam);
};

class PlayIfNoAnimPlaying : public Play
{
public:
	virtual void Update(const std::string& key, AnimatedCamera& cam);
};

class PlayAnimSequence : public Play
{
public:
	virtual void Update(const std::string& key, AnimatedCamera& cam);
};



class Updator
{
public:
	virtual void Update(AnimatedCamera& cam) = 0;
};

class UpdatorDefault : public Updator
{
public:
	virtual void Update(AnimatedCamera& cam);
};

class UpdatorAnimSequence : public Updator
{
public:
	virtual void Update(AnimatedCamera& cam);
};


class Stop
{
public:
	virtual void Update(AnimatedCamera& cam) = 0;
};

class ReturnTo1PV : public Stop
{
public:
	virtual void Update(AnimatedCamera& cam);
};

class ReturnTo1PVDirect : public Stop
{
public:
	virtual void Update(AnimatedCamera& cam);
};


class Notifier
{
public:
	Notifier() : m_completed(FALSE) {}
	virtual ~Notifier() {}

	virtual void Update(AnimatedCamera& cam) = 0;

	BOOL IsCompleted() const		{ return m_completed;		}
	void SetComplete(BOOL complete)	{ m_completed = complete;	}
	void Reset()					{ m_completed = FALSE;		}

protected:
	BOOL m_completed;
};

class NotifierAtAnimStart : public Notifier
{
public:
	virtual void Update(AnimatedCamera& cam);
};

class NotifierAtAnimEnd : public Notifier
{
public:
	virtual void Update(AnimatedCamera& cam);
};

class NotifierAtAnimDelayEnd : public Notifier
{
public:
	NotifierAtAnimDelayEnd(REAL32 delayTime);
	virtual void Update(AnimatedCamera& cam);

private:
	REAL32 m_delayTime;
};

class NotifierSequenceMode : public Notifier
{
public:
	virtual void Update(AnimatedCamera& cam);
};

namespace AnimCamModeStuff
{
	void SetDefaultMode(AnimatedCamera& cam);
	void SetSequenceMode(AnimatedCamera& cam);
	void SetDominationMode(AnimatedCamera& cam);
}

#endif

#endif