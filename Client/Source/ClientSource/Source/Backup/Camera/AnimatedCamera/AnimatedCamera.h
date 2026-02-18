#ifndef _ANIMATED_CAMERA_H_
#define _ANIMATED_CAMERA_H_

#ifdef	DOMI_ANIM_CAM

class CameraAnimation;
class Play;
class Stop;
class Updator;
class Notifier;
class OutsideEventSubject;

class AnimatedCamera
{
public:
	AnimatedCamera();
	virtual ~AnimatedCamera();

	void Update();
	void Start(const std::string& key);
	void End();

	void ChangeAnimation(CameraAnimation* anim);
	BOOL NoAnimPlaying();

	void Insert(std::string key, std::string filePath);
	void Delete(const std::string& file);
	BOOL Find(const std::string& file);
	void DeleteAllAnim();

	i3Node*	GetCurAnim() const;
	const char* GetCurAnimName() const { return m_curAnimName.c_str(); }
	const std::map<std::string, CameraAnimation*>& GetContainer() const { return m_container; }

	void SetStartMode(Play* mode);
	void SetEndMode(Stop* mode);
	void SetUpdateMode(Updator* mode);
	void SetNotifier(Notifier* mode);

	void AttachOutsideEvent(OutsideEventSubject* subject);
	void DetachOutsideEvent();
	void NotifyOutsideEvent();

private:
	i3AttrSet*				m_attrSet;
	i3Node*					m_root;
	i3Camera*				m_camera;

	Play*					m_play;
	Stop*					m_stop;
	Updator*				m_updator;
	Notifier*				m_notifier;

	OutsideEventSubject*	m_outsideEventSubject;

	std::string				m_curAnimName;

	std::map<std::string, CameraAnimation*>	m_container;
};

#endif

#endif