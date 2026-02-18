#if !defined( __I3_SOUND_CONTEXT_DX_H)
#define __I3_SOUND_CONTEXT_DX_H

#include "i3SoundConfig.h"

#if defined( I3SND_DSOUND)
#include "i3SoundContext.h"
#include "i3SoundPlayStateDx.h"

class i3SoundContextDX : public i3SoundContext
{
	I3_CLASS_DEFINE( i3SoundContextDX);
protected:
	REAL32					m_rUpdateTime;

	LPDIRECTSOUND3DLISTENER	m_pDSListener;
	DS3DLISTENER			m_dsListenerParams;
	HMODULE					m_hdsound3d;

	i3SoundPlayThread *		m_pPlayThread;
	i3SoundUpdateThread	*	m_pUpdateThread;
	bool					m_bNeedRaisePlaythread;
	i3Mutex				*	m_pReadyListMutex;
	i3Mutex				*	m_pPlayListMutex;
	i3Mutex				*	m_pUpdateListMutex;

	//Control Play/Update state list
	i3SoundPlayStateDx	*	m_pHead_ReqPlay;	//Play Request가 들어온 State
	i3SoundPlayStateDx	*	m_pHead_Update;		//Update( Drive) 할 State
	i3SoundPlayStateDx	*	m_pHead_Ready;		//놀고있는 스테이트

protected:

	void					InitDX(void);
	void					FinishDX(void);
	BOOL					CreateDX( I3SND_CONTEXTCREATE_PARAM * pInitParam);
	BOOL					CreatePrimaryBuffer(HWND PlatFormData );
	BOOL					GetListenerObject();
	void					_CalculatePosByListener( VEC3D * pPos, VEC3D * pSrcPos);
	void					_LinkPlayStateList(void);

	virtual void			_UpdateListenerParam(void);
	virtual i3SoundPlayState *		GetFreePlayState( void);

public:
	i3SoundContextDX(void);
	virtual ~i3SoundContextDX(void);

	virtual void			SetListnerMatrix(MATRIX* pMat, REAL32 rDeltaSeconds);

	i3SoundPlayStateDx	*		getPlayHead()	{	return m_pHead_ReqPlay;}
	i3SoundPlayStateDx	*		getUpdateHead()	{	return m_pHead_Update;}
	i3SoundPlayStateDx	*		getReadyHead()	{	return m_pHead_Ready;}

	void						setPlayHead( i3SoundPlayStateDx * pState)		{ I3_REF_CHANGE( m_pHead_ReqPlay, pState);}
	void						setUpdateHead( i3SoundPlayStateDx * pState)	{ I3_REF_CHANGE( m_pHead_Update, pState);}
	void						setReadyHead( i3SoundPlayStateDx * pState)	{ I3_REF_CHANGE( m_pHead_Ready, pState);}

	virtual BOOL				Create( I3SND_CONTEXTCREATE_PARAM * pInitParam);

	virtual void				StopAllSound(bool bForce = false);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, VEC3D * pPos = NULL);

	virtual i3SoundPlayState *	Play( i3SoundPlayInfo * pInfo, MATRIX * pMat);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, MATRIX * pMat);

	virtual BOOL				Stop( i3SoundPlayState * pState, bool bForce );

	virtual void				UpdatePos( i3SoundPlayState * pPlayState, VEC3D * pPos, REAL32 rDeltaSeconds);
	virtual void				UpdatePos( i3SoundPlayState * pPlayState, MATRIX * pMat, REAL32 rDeltaSeconds);
	virtual void				UpdateListener();

	virtual void				setMainVolume( UINT8 nVolume);
	virtual void				SetTypeVolume( UINT32 nType, UINT8 nVolume);

	virtual void				OnDrive(REAL32 rDeltaSeconds);
};

#endif
#endif
