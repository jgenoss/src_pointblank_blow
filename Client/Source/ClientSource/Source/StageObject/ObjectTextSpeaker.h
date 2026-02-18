#if !defined( __OBJECT_TEXT_SPEAKER_H__)
#define __OBJECT_TEXT_SPEAKER_H__

#include "GameControledObject.h"

class CGameObjectTextSpeaker : public CGameControledObject
{
	I3_CLASS_DEFINE( CGameObjectTextSpeaker, CGameControledObject);
protected:
	// persist
	UINT32		m_nTextType;
	REAL32		m_rTextShowTime;
	REAL32		m_rViewLength;
	
	INT32		m_nTextViewIndex;
	REAL32		m_rAccumulateTime;
	bool		m_bUpdate;
	bool		m_bFirstUpdate;
	VEC3D		m_vCenterPos;

public:
	CGameObjectTextSpeaker();
	virtual ~CGameObjectTextSpeaker();

	virtual UINT32	OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;

	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override;
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;
	virtual void	SetControledObjectState( INT32 state) override;

	UINT32		getTextType( void)				{ return m_nTextType; }
	void		setTextType( UINT32 nTextType)	{ m_nTextType = nTextType; }

	REAL32		getTextShowTime( void)			{ return m_rTextShowTime; }
	void		setTextShowTime( REAL32 tm)		{ m_rTextShowTime = tm; }

	REAL32		getTextViewLength( void)		{ return m_rViewLength; }
	void		setTextViewLength( REAL32 len)	{ m_rViewLength = len; }
};

#endif
