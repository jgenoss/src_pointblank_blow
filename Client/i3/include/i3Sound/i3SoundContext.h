#ifndef I3_SOUND_CONTEXT_H__
#define I3_SOUND_CONTEXT_H__

// Name: 
// Desc: 
//-----------------------------------------------------------------------------
#include "i3Sound.h"
#include "i3SoundFile.h"
#include "i3SoundPlayInfo.h"
#include "i3SoundPlayState.h"
#include "i3SoundPlayStatePSP.h"
#include "i3SoundPlayThread.h"
#include "i3SoundUpdateThread.h"

struct I3SND_CONTEXTCREATE_PARAM
{
	UINT64	m_nPlatformData = 0;
	bool	m_bHasListener = false;
	INT32	m_nFlags = 0;
	INT32	m_nVoiceCount = 0;
	bool	m_bInitialState = false;
};

class I3_EXPORT_SOUND i3SoundContext : public i3ElementBase  
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3SoundContext, i3ElementBase);

public:
	i3SoundContext(void);
	virtual ~i3SoundContext(void);

protected:
	I3SND_CONTEXTCREATE_PARAM	m_InitParam;
	
public:
//	List					m_SoundList;				// 쓰이지 않는듯 하여 주석.
	i3::vector<i3SoundPlayState*>	m_SoundPlayStateList;
	i3::vector<i3SoundPlayState*>	m_FreePlayStateList;
	i3::vector<i3SoundPlayState*>	m_UsedPlayStateList;

	MATRIX					m_MtxListener;					// Camera World Matrix와 동일..
	VEC3D					m_vListenerVelocity;
	bool					m_bChangedListenerMatrix = false;
	bool					m_bNeedUpdateListener = true;
	
	//
	UINT32					m_nState = I3SND_PROP_TYPE_BGM | I3SND_PROP_TYPE_VOICE | I3SND_PROP_TYPE_EFFECT;

	//
	UINT8					m_nTypeVolume[I3SND_PROP_TYPE_COUNT] = { 100, 100, 100 };
	bool					m_bChangedTypeVolume[I3SND_PROP_TYPE_COUNT] = { false, false, false };

	UINT8					m_nMainVolume = 80;
	bool					m_bChangedMainVolume = false;

	bool					m_bPendingUpdate = false;

protected:
	virtual void				_UpdateListenerParam(void);

public:
//-----------------------------------------------------------------------------
// Common Section
//-----------------------------------------------------------------------------
	//Context
	virtual bool			Create( I3SND_CONTEXTCREATE_PARAM * pInitParam);
	virtual bool			Destroy();

	bool					PrepareContext();
	i3SoundPlayState *		AddNewPlayState();

	void					addState( UINT32 nState)	{	m_nState |= nState;}
	void					removeState( UINT32 nState)	{	m_nState &= ~nState;}
	void					setState( UINT32 nState)	{	m_nState = nState;}
	UINT32					getState()	{	return m_nState;}

	void					RemoveSound( i3Sound * pSound);
	void					ReleaseAllSound();
	void					ReleaseAllPlayState();

	void					FlushSound( bool bForce);

	UINT8					GetTypeVolume(UINT32 nType);
	virtual void			SetTypeVolume( UINT32 nType, UINT8 nVolume);

	UINT8					getMainVolume()	{	return m_nMainVolume;}
	virtual void			setMainVolume( UINT8 nVolume);

	//PlayState
	i3SoundPlayState	*		GetPlayState( const char * szFileName );
	i3SoundPlayState	*		GetPlayState( INT32	nIndex );
	virtual i3SoundPlayState *	GetFreePlayState(void);

	virtual void				StopAllSound(bool bForce = false);
	virtual i3SoundPlayState *	Play( i3SoundPlayInfo * pInfo, VEC3D * pPos = nullptr, INT32 nGrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, VEC3D * pPos = nullptr, INT32 nGrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr);

	virtual i3SoundPlayState *	Play( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 nGrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 nGrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr);

	virtual bool				Stop( i3SoundPlayState * pState, bool bForce );

	virtual void				UpdatePos( i3SoundPlayState * pPlayState, VEC3D * pPos, REAL32 rDeltaSeconds);
	virtual void				UpdatePos( i3SoundPlayState * pPlayState, MATRIX * pMat, REAL32 rDeltaSeconds);
	virtual void				UpdateListener();
	virtual void				SetListnerMatrix(MATRIX* pMat, REAL32 rDeltaSeconds);

	void						setNeedUpdateListener( bool bUpdate = true)	{	m_bNeedUpdateListener = bUpdate;}
	bool						isNeedUpdateListener(void)					{	return m_bNeedUpdateListener;}

	virtual void				OnDrive(REAL32 rDeltaSeconds);

	MATRIX*						GetListnerMatrix()				{	return &m_MtxListener;}

	void						SetPendingEnable( bool bPending);
	bool						GetPendingEnable( void)	{	return m_bPendingUpdate;}

	virtual i3::string			ReportPerformance(void);

	bool						isAudioable( MATRIX * pMat, REAL32 rMaxDistance);

	//Channel Group Function
	virtual	INT32				ChannelGroup_Add( const char * pszChannelName){ return 0;}
	virtual bool				ChannelGroup_SetVolume( INT32 idx, INT32 volume){ return true;}
	virtual bool				ChannelGroup_Stop( INT32 idx){ return true;}
	virtual bool				ChannelGroup_Pause( INT32 idx, bool bPause){ return true;}
	virtual void	*			GetChannelGroup( INT32 idx)	{	return nullptr;}
	virtual bool				ChannelGroup_SetPitch( INT32 idx, REAL32 rPitch){ return false;}
	virtual INT32				ChannelGroup_GetNumChannels( INT32 idx)	{	return 0;}

public:
	/*
	bool					m_bEnableFX;
	UINT32					m_nEffectType;
	UINT32					m_nReverbPresetIndex;

	bool					GetFXEnable()	{	return m_bEnableFX;}
	void					SetFXEnable( bool bEnable)	{ m_bEnableFX = bEnable;}

	void					SetFXType( INT32 nFXType);
	void					SetReverbParam( UINT32 nPresetIndex);//{ m_nReverbPresetIndex = nPresetIndex;}
	UINT32					GetReverbParam(){	return m_nReverbPresetIndex; }
	*/

//-----------------------------------------------------------------------------
// Platform PSP Section
//-----------------------------------------------------------------------------

#if defined ( I3G_PSP )
	i3SoundSasThread	*	m_pSasThread;

	bool					StartSasThread();
	bool					StopSasThread();

	bool					InitSasModule();
	bool					InitWaveModule();
#endif

};

#endif // I3_SOUND_CONTEXT_H__

