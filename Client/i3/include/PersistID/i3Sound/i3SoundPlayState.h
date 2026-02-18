#ifndef I3_SOUND_PLAYSTATE_H__
#define I3_SOUND_PLAYSTATE_H__

#include "i3Sound.h"
#include "i3SoundStream.h"
#include "i3SoundStreamThread.h"
#include "i3SoundFile.h"
#include "i3SoundPlayInfo.h"

typedef struct tag_PlayStateData
{
	INT32	nState;					//Loop
	BOOL	bUseWorldCoordinate;	// 듣는사람의 위치에 상관없이 나는 소리인지 여부

	VEC3D*	vecPosition;			//if(speakertype> mono) position( worldbase? listenerbase?)

	//sysinfo	:Platform,
	//localinfo	:Buffer Mode, LoopPlay,speaker type, (terminate type: time, distance, priority)
	// 추가 데이터

	//environment
	//env = effType
	//normal = delay =0, playposition= 0
	//if(stream)	handle,smallbuffer,readposition,writeposition,format?
}I3SND_PLAYSTATE_DATA;

class i3SoundContext;
class i3SoundPlayThread;
class i3SoundUpdateThread;

class I3_EXPORT_SOUND i3SoundPlayState :	public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3SoundPlayState );

public:
	i3SoundPlayState(void);
	~i3SoundPlayState(void);

protected:
	//Volatile Member
	i3SoundPlayInfo*		m_pInfo;

	UINT32					m_nState;
	UINT32					m_nIndex;

	//Volatile Member
	static	bool			m_bChangedVolume;

	//Priority
	INT32					m_nPriorityLevel;	//다른 소리들과의 Priority상하관계
	INT32					m_nPriorityType;	//같은 소리들과의 Priority상하 형태

	//Positional Sound
	UINT32					m_nPitch;
	INT32					m_nVolume;
	
	VEC3D					m_vecPos;
	VEC3D					m_vDirection;
	VEC3D					m_vVelocity;

	VEC3D					m_vecOldPos;
	VEC3D					m_vOldDirection;
	VEC3D					m_vOldVelocity;
	
	INT32					m_nFinalVolume;

	//Streamming
protected:
	UINT32					m_nPlayFlag;
	WCHAR32					m_dwPriority;
	WCHAR32					m_dwPlayPointer;

protected:
	virtual INT32			_calcCtxVolume(void);			// Type Volume 및 Master Volume 등을 모두 감안한 최종 Volume 계산. (0~100)

public:
	static	INT32			m_nCount;

public:
	void	SetIndex( UINT32 Index )	{ m_nIndex = Index;}
	UINT32	GetIndex()	{	return m_nIndex; }

	INT32	GetState()						{	return m_nState;}
	void	SetState( UINT32	nState )	{	m_nState = nState;}
	void	AddState( UINT32	nState )	{	m_nState |= nState; }
	void	RemoveState( UINT32	nState )	{	m_nState &= ~nState;}

	INT32	GetVolume(void)					{ return m_nVolume; }
	
	void	SetPlayState( UINT32 nState);
	BOOL	SetSoundPlayInfo( i3SoundPlayInfo * pInfo);
	i3SoundPlayInfo * getPlayInfo()			{ return m_pInfo;}

	BOOL	GetVoiceNum()					{ return m_nIndex;}
	virtual void GetPlayPointer(WCHAR32 * pPointer){};

public:
	
	// isState
	bool	isPlaying()						{ return ((m_nState & I3SND_RT_PLAYSTATE_PLAY) != 0);}
	bool	isPaused()						{ return (((m_nState & I3SND_RT_PLAYSTATE_PLAY) != 0) && ((m_nState & I3SND_RT_PLAYSTATE_PAUSE) != 0));}
	bool	isFree()
	{ 
		return (((m_nState & I3SND_RT_MASK_PLAYSTATE) == I3SND_RT_PLAYSTATE_FREE));
	}

	bool	isMuted()						{ return (((m_nState & I3SND_RT_PLAYSTATE_PLAY) != 0) && ((m_nState & I3SND_RT_PLAYSTATE_MUTE) != 0));}

	bool	isSoundOfType( UINT32 type)		{ return (m_nState & I3SND_PROP_MASK_TYPE) == type; }
	bool    isEffectSound()					{ return (m_nState & I3SND_PROP_MASK_TYPE) == I3SND_PROP_TYPE_EFFECT;}
	bool	isStreamingSound()				{ return (m_nState & I3SND_RT_MASK_STREAM) == I3SND_RT_BUFFMODE_STREAM;}
	bool	isLoop(void)					{ return (m_nState & I3SND_RT_MASK_LOOPMODE) == I3SND_RT_LOOPMODE_LOOPING; }
	bool	isListnerSound(void)			{ return (m_nState & I3SND_RT_MASK_3DMODE) == I3SND_RT_3DMODE_DISABLE; }

	virtual void	Reset(void);
	/////////////////////////////////////////////////////////////////////////////////
	//Request
	/////////////////////////////////////////////////////////////////////////////////
public:

	virtual bool Drive(){	return FALSE;}

	virtual bool Play( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat, INT32 GrpIdx = -1)					{ return false;}
	virtual bool Replay( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat, INT32 GrpIdx = -1)					{ return false;}

	/////////////////////////////////////////////////////////////////////////////////////
	//Real Func
	/////////////////////////////////////////////////////////////////////////////////////
	virtual bool Stop( bool bForce = false)	{ return false;}
	virtual bool Pause(void)				{ return false;}
	virtual bool Mute(bool bMute)			{ return false;}
	virtual bool Resume(void)				{ return false;}
	virtual bool SetVolume( INT32 vol)		{ return false;}
	virtual bool UpdatePos( MATRIX * pMtx)	{ return false;}
	virtual bool SetPitch( INT32 nPitch)	{ return false;}
	virtual bool UpdateMuteState(void)		{ return false;}

	virtual void Set3DParams();

	bool		_UpdateLocationParam( i3SoundPlayInfo * pInfo, MATRIX * pMat);
};

#endif

