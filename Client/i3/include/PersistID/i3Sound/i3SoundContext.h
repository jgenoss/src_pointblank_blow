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

typedef struct tag_SoundContextCreateParam
{
	UINT64	m_nPlatformData;
	BOOL	m_bHasListener;
	INT32	m_nFlags;
	INT32	m_nVoiceCount;
	BOOL	m_bInitialState;
}I3SND_CONTEXTCREATE_PARAM;

class I3_EXPORT_SOUND i3SoundContext : public i3ElementBase  
{
	I3_ABSTRACT_CLASS_DEFINE(i3SoundContext);

public:
	i3SoundContext(void);
	virtual ~i3SoundContext(void);

protected:
	I3SND_CONTEXTCREATE_PARAM	m_InitParam;
	
public:
	i3List					m_SoundList;
	i3List					m_SoundPlayStateList;
	i3List					m_FreePlayStateList;
	i3List					m_UsedPlayStateList;

	MATRIX					m_MtxListener;					// Camera World Matrix¿Í µ¿ÀÏ..
	VEC3D					m_vListenerVelocity;
	bool					m_bChangedListenerMatrix;
	bool					m_bNeedUpdateListener; 
	
	//
	UINT32					m_nState;

	//
	UINT8					m_nTypeVolume[I3SND_PROP_TYPE_COUNT];
	BOOL					m_bChangedTypeVolume[I3SND_PROP_TYPE_COUNT];

	UINT8					m_nMainVolume;
	BOOL					m_bChangedMainVolume;

	bool					m_bPendingUpdate;

protected:
	virtual void				_UpdateListenerParam(void);

public:
//-----------------------------------------------------------------------------
// Common Section
//-----------------------------------------------------------------------------
	//Context
	virtual BOOL			Create( I3SND_CONTEXTCREATE_PARAM * pInitParam);
	virtual BOOL			Destroy();

	BOOL					PrepareContext();
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
	virtual i3SoundPlayState *	Play( i3SoundPlayInfo * pInfo, VEC3D * pPos = NULL, INT32 nGrpIdx = 0);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, VEC3D * pPos = NULL, INT32 nGrpIdx = 0);

	virtual i3SoundPlayState *	Play( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 nGrpIdx = 0);
	virtual i3SoundPlayState *	RePlay( i3SoundPlayState * pState, i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 nGrpIdx = 0);

	virtual BOOL				Stop( i3SoundPlayState * pState, bool bForce );

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

	virtual char *				ReportPerformance(void);

	bool						isAudioable( MATRIX * pMat, REAL32 rMaxDistance);

	//Channel Group Function
	virtual	INT32				ChannelGroup_Add( const char * pszChannelName){ return 0;}
	virtual bool				ChannelGroup_SetVolume( INT32 idx, INT32 volume){ return true;}
	virtual bool				ChannelGroup_Stop( INT32 idx){ return true;}
	virtual bool				ChannelGroup_Pause( INT32 idx, bool bPause){ return true;}
	virtual void	*			GetChannelGroup( INT32 idx)	{	return NULL;}
	virtual bool				ChannelGroup_SetPitch( INT32 idx, REAL32 rPitch){ return false;}
	virtual INT32				ChannelGroup_GetNumChannels( INT32 idx)	{	return 0;}

public:
	/*
	BOOL					m_bEnableFX;
	UINT32					m_nEffectType;
	UINT32					m_nReverbPresetIndex;

	BOOL					GetFXEnable()	{	return m_bEnableFX;}
	void					SetFXEnable( BOOL bEnable)	{ m_bEnableFX = bEnable;}

	void					SetFXType( INT32 nFXType);
	void					SetReverbParam( UINT32 nPresetIndex);//{ m_nReverbPresetIndex = nPresetIndex;}
	UINT32					GetReverbParam(){	return m_nReverbPresetIndex; }
	*/

//-----------------------------------------------------------------------------
// Platform PSP Section
//-----------------------------------------------------------------------------

#if defined ( I3G_PSP )
	i3SoundSasThread	*	m_pSasThread;

	BOOL					StartSasThread();
	BOOL					StopSasThread();

	BOOL					InitSasModule();
	BOOL					InitWaveModule();
#endif

};

#endif // I3_SOUND_CONTEXT_H__

