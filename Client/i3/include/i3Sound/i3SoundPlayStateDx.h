#ifndef I3_SOUND_PLAYSTATE_DX_H__
#define I3_SOUND_PLAYSTATE_DX_H__

#if defined( I3SND_DSOUND )
#include "i3Sound.h"
#include "i3SoundPlayState.h"

class i3SoundContextDX;
class i3SoundPlayThread;
class i3SoundUpdateThread;

class I3_EXPORT_SOUND i3SoundPlayStateDx :	public i3SoundPlayState
{
//	I3_CLASS_DEFINE( i3SoundPlayStateDx );
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3SoundPlayStateDx, i3SoundPlayState, i3::cms_concrete_dsound::value);

	//아래 함수는 외부에서는 i3SoundPlayThread에서만 사용가능하게...
	friend class i3SoundPlayThread;
	friend class i3SoundUpdateThread;
	friend class i3SoundContextDX;

protected:
	//Volatile Member
	i3SoundPlayStateDx	*	m_pNext;
	i3SoundPlayStateDx	*	m_pPrev;

	UINT32					m_nRequest;
	static	UINT32			m_nGlobalRequest;

	I3SND_BUFINFO		*	m_pBufferInfo;

	UINT32					m_CreateFlag;
	LPDIRECTSOUNDBUFFER8 	m_lpDSBuffer;

	//3dSound
	LPDIRECTSOUND3DBUFFER8	m_p3DBuffer;
	DS3DBUFFER				m_dsBufferParams;

	//Fx : reverb
	LPDIRECTSOUNDFXI3DL2REVERB8 m_lpReverb;
	DSFXI3DL2Reverb				m_paramsReverb;

	DSEFFECTDESC                m_rgFxDesc;
	const GUID *                m_rgRefGuids;

	i3SoundStreamThread	*	m_pStreamThread;
	i3SoundPlayState		*m_pStreamArg;			//스레드의 UserData로 넘겨줄 값( this)


	static void				_OnRunningStream( void * pUserData);
	bool					_CreateStream();
	bool					_StartStream();
	bool					_StopStream();

	bool					_IsValidPlayReq(void);

	//Request
	bool		CMD_Stop(void);
	bool		CMD_Pause(void);
	bool		CMD_Resume(void);
	bool		CMD_Play(void);
	bool		CMD_Replay(void);
	bool		CMD_SetVolume(void);
	bool		CMD_SetPitch(void);
	bool		CMD_UpdatePos(void);

public:
	i3SoundPlayStateDx(void);
	virtual ~i3SoundPlayStateDx(void);
	// State
	i3SoundPlayStateDx	*	getNext()	{ return m_pNext;}
	i3SoundPlayStateDx	*	getPrev()	{ return m_pPrev;}

	void					SetNext( i3SoundPlayStateDx *pState);
	void					SetPrev( i3SoundPlayStateDx * pState);

	bool					UpdateStream();

	bool	isRequestEnable()				{ return ( m_nRequest != I3SND_RT_REQ_NONE);}

	//local Request
	UINT32		getRequest()					{ return m_nRequest;}
	void		setRequest(UINT32 nRequest)		{ m_nRequest = nRequest;}
	void		addRequest(UINT32 nRequest)		{ m_nRequest |= nRequest;}
	void		removeRequest(UINT32 nRequest)	{ m_nRequest &= ~nRequest;}

	//Global Request
	static	void		setGlobalRequest( UINT32 nRequest)		{ m_nGlobalRequest = nRequest;}
	static	UINT32		getGlobalRequest()						{ return m_nGlobalRequest;}
	static	void		addGlobalRequest(UINT32 nRequest)		{ m_nGlobalRequest |= nRequest;}
	static	void		removeGlobalRequest(UINT32 nRequest)	{ m_nGlobalRequest &= ~nRequest;}

	
	virtual bool Play( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat);
	virtual bool Replay( i3SoundPlayInfo * pSoundInfo, MATRIX * pMat);
	virtual void Set3DParams(void);

	virtual void GetPlayPointer( WCHAR32 * pPointer);

	virtual bool Stop( bool bForce = false);
	virtual bool Pause(void);
	virtual bool Resume(void);
	virtual bool SetVolume( INT32 nVol);
	virtual bool SetPitch( INT32 nPitch);
	virtual bool UpdatePos( MATRIX * pMtx);
	virtual bool Drive();


public:
	/////////////////////////////////////////////////////////////////////////////
	// Sound FX
	void	InitReverb();
	HRESULT	SetReverb();
	HRESULT	DisableAllFX();
	HRESULT	SetReverbPreset( UINT32 nPreset);
	HRESULT	SetReverbParam( DSFXI3DL2Reverb * pparam);



	//Reverb
	/*보기 싫겠지만 일단은 그냥 둡니다.
	//여러 eff들을 같이 적용하도록 만들려했는데..메모리 누수등 몇개의 문제가 voicemanage과정에서
	//생겨버려 static이 아닌상태가 되어야 할것같은데...
	//그러기엔 객체가 너무 많은듯 하여..일단 reverb만 활성화시키고 reverb전용으로 진행합니다.
	//Sound FX
	public:
    static	LPDIRECTSOUNDFXCHORUS8      m_lpChorus;
    static	LPDIRECTSOUNDFXCOMPRESSOR8  m_lpCompressor;
    static	LPDIRECTSOUNDFXDISTORTION8  m_lpDistortion;
    static	LPDIRECTSOUNDFXECHO8        m_lpEcho;
    static	LPDIRECTSOUNDFXFLANGER8     m_lpFlanger;
    static	LPDIRECTSOUNDFXGARGLE8      m_lpGargle;
    static	LPDIRECTSOUNDFXPARAMEQ8     m_lpParamEq;
    static	LPDIRECTSOUNDFXI3DL2REVERB8 m_lpReverb;
	

    static	DSFXChorus                  m_paramsChorus;
    static	DSFXCompressor              m_paramsCompressor;
    static	DSFXDistortion              m_paramsDistortion;
    static	DSFXEcho                    m_paramsEcho;
    static	DSFXFlanger                 m_paramsFlanger;
    static	DSFXGargle                  m_paramsGargle;
    static	DSFXParamEq                 m_paramsParamEq;
    static	DSFXI3DL2Reverb             m_paramsReverb;

protected:
    static	DSEFFECTDESC                m_rgFxDesc[I3SND_CMN_FX_COUNT];
	static	const GUID *                m_rgRefGuids[I3SND_CMN_FX_COUNT];
    static	LPVOID *                    m_rgPtrs[I3SND_CMN_FX_COUNT];
    static	bool                        m_rgLoaded[I3SND_CMN_FX_COUNT];
    static	DWORD                       m_dwNumFX;
	HRESULT DisableAllFX();
	HRESULT	SetFX();

	static	void	InitFX();
	static HRESULT	SetFX( UINT32 nEffectType);
	static HRESULT	EnableFX( GUID sfxClass, REFGUID refGUIDInterface, LPVOID * ppObj);
	static HRESULT	SetFXEnable( UINT32 nEffectType);
	static HRESULT	LoadParamValues();
	*/
};

#endif	// if defined ( I3SND_DSOUND )
#endif	// ifndef I3_SOUND_PLAYSTATE_DX_H__