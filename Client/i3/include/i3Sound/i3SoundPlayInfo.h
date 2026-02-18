#if !defined( __I3SND_PLAY_INFO_H)
#define __I3SND_PLAY_INFO_H

#include "i3Sound.h"

// RSC FILE INFO
// 섹션이름		: 음원 파일의 이름
// Type			: BGM	: 배경음악
//				: VOICE : 목소리
//				: EFFECT: 효과음->아래의 대부분의 값은 효과음을 위한 값들.
//
// DistMin 		: 소리가 작아지기 시작하는 거리
// DistMax 		: 최대 가청 거리
// DistVolType 	: 거리에 따른 소리의 증/감 형태 : Const: 거리가 멀어져도 영향 안받음
//				: Linear, Exp
//				      
// DefVolume	: 소리의 볼륨 >> 같은 사운드 모두에게 영향을 미친다.
// Loop			: 반복 플레이 여부
// MaxInstance 	: 최대 동시 발음수
// PriorityLevel: 모든 소리중에서의 우선순위
// PriorityType	: 같은 소리중에서의 우선순위 종류(각 타입별로 우선순위가 정의)
//				:Distance     - Listener와의 거리에 따라 우선순위 판정
//				:PlayTime     - 재생 경과시간에 따른 우선순위 판정
//				:DefinedLevel - 지정된 우선순위로 판정(같은 사운드 내에서 판정이므로 삭제 대상)
// File			:파일이 존재하는 directory

class I3_EXPORT_SOUND i3SoundPlayInfo : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3SoundPlayInfo, i3PersistantElement);

public:
	virtual ~i3SoundPlayInfo();

protected:
	// persist
	i3Sound *			m_pSound = nullptr;					// 플레이될 사운드 소스

	UINT32				m_nStyle = I3SND_RT_3DMODE_DISABLE | I3SND_PROP_TYPE_EFFECT;					// 플레이될 사운드 기본 스타일

	INT32				m_nDefaultVolume = 100;			// 0 ~ 100 : default 볼륨이면서 3dPositional volume의 볼륨과 같이 작용
	INT32				m_nListenerVolume = 100;			// 0 ~ 100 : positional volume을 사용하지 않는것의 볼륨

	INT32				m_nPriorityLevel = 0;			// 우선순위 0 : Highest, 255 : Lowest : 미적용

	REAL32				m_rMinDistance = 3.0f;				// 사운드의 소리가 작아지기 시작하는거리, 이거리의 1/2n 만큼 소리는 증가한다.
	REAL32				m_rMaxDistance = 50.0f;				// 사운드의 소리가 더이상 작아지지 않는거리, bMuteAtMaxDistance가 켜져잇다면, 이거리 이후에는 소리는 나지않는다.
	INT32				m_nVolType = 0;					// 현재 적용되지 않음.

	VEC3D				m_vPosition;				// 발음 위치( attached 라면 부모의 영향을 받는다.)
	VEC3D				m_vDirection = { 0.0f, 1.0f, 0.0f };				// 발음 방향(				"					 )
	VEC3D				m_vVelocity;				// 이동 속도( for doppler effect)

	INT32				m_nInsideConeAngle = 360;			// 
	INT32				m_nOutsideConeAngle = 360;		// 

	REAL32				m_rUpdateInterval = 0.1f;			// Update 간격
	UINT32				m_nFrequency = 0;				// Frequency( pitch)
	INT32				m_nI3DL2RvbPreset = -1;			// 환경 리버브 프리셋

	//volatile
	OBJREF64			m_SoundRef = 0;	// 사운드를 로드하지 못한경우 담아둔다.

	// persist
	UINT8				m_nOutsideVolume = 100;			//

	bool				m_bForceStopAtStopAllSound = true;	// StopAllSound( false)가 요청되는경우에 멈추어야할사운드인지 여부

	// persist
	bool				m_bMuteAtMaxDistance = true;		// max거리에서 소리가 묵음이 될지 여부
	bool				m_bStopOnReset = false;				// OnChangeTime이나 Stop시에 강제종료여부

public:
	i3Sound *			getSound(void)					{ return m_pSound; }
	void				setSound( i3Sound * pSnd);

	void				AddStyle( UINT32 nStyle)		{ m_nStyle |= nStyle;}
	void				RemoveStyle( UINT32 nStyle)		{ m_nStyle &= ~nStyle;}
	UINT32				GetStyle()						{ return m_nStyle;}
	void				SetStyle(UINT32 nStyle)			{ m_nStyle = nStyle;}

	//
	UINT32				GetSoundType()					{ return m_nStyle & I3SND_PROP_MASK_TYPE;}
	UINT32				GetPriorityType()				{ return m_nStyle & I3SND_PROP_MASK_PRIORITY;}

	UINT32				GetStreamType()					{ return m_nStyle & I3SND_RT_MASK_STREAM;}
	UINT32				GetLoopMode()					{ return m_nStyle & I3SND_RT_MASK_LOOPMODE;}
	UINT32				Get3DMode(void)					{ return m_nStyle & I3SND_RT_MASK_3DMODE; }

	void				set3DMode( UINT32 n3DMode)
	{	
		RemoveStyle( I3SND_RT_MASK_3DMODE);
		AddStyle( n3DMode);
	}

	void				SetDefaultVolume(INT32 nVolume);
	void				SetPriorityLevel(INT32 nPriority)	{ m_nPriorityLevel	= nPriority;}
	void				setListenerVolume( INT32 nVolume)	{ m_nListenerVolume = nVolume;}
	void				setLoopMode( UINT32 nLoopMode)
	{
		RemoveStyle( I3SND_RT_MASK_LOOPMODE);
		AddStyle( nLoopMode);
	}

	INT32				getListenerVolume(void)				{ return m_nListenerVolume;}
	INT32				GetDefaultVolume()					{ return m_nDefaultVolume;}
	INT32				GetPriorityLevel()					{ return m_nPriorityLevel;}

	void				SetDistanceInfo( REAL32 rMax, REAL32 rMin, INT32 nVolType)
	{
		m_rMinDistance = rMin;
		m_rMaxDistance = rMax;
		m_nVolType = nVolType;
	}

	REAL32				getMinDistance(void)				{ return m_rMinDistance;	}
	REAL32				getMaxDistance(void)				{ return m_rMaxDistance;	}
	INT32				getVolumeType(void)					{ return m_nVolType;		}

	void				setMuteAtMaxDistance( bool bMute)	{ m_bMuteAtMaxDistance = bMute;}
	bool				isMuteAtMaxDistance( void)			{ return m_bMuteAtMaxDistance;}

	void				setStopOnReset(bool bStop)			{ m_bStopOnReset = bStop;}
	bool				isStopOnReset( void)				{ return m_bStopOnReset;}

	void				setPosition( VEC3D * pPos)			{ i3Vector::Copy( &m_vPosition, pPos);}
	VEC3D			*	getPosition( void)					{ return &m_vPosition;}

	void				setDirection( VEC3D * pDir)			{ i3Vector::Copy( &m_vDirection, pDir);}
	VEC3D			*	getDirection(void)					{ return &m_vDirection;}

	void				setVelocity( VEC3D * pVel)			{ i3Vector::Copy( &m_vVelocity, pVel);}
	VEC3D			*	getVelocity(void)					{ return &m_vVelocity;}

	void				setInsideConeAngle( INT32 nAngle)	{ m_nInsideConeAngle = nAngle;}
	void				setOutsideConeAngle( INT32 nAngle)	{ m_nOutsideConeAngle = nAngle;}
	void				setOutsideVolume( UINT8 nVolume)	{ m_nOutsideVolume = nVolume;}

	INT32				getInsideConeAngle(void)			{ return m_nInsideConeAngle;}
	INT32				getOutsideConeAngle(void)			{ return m_nOutsideConeAngle;}
	UINT8				getOutsideVolume(void)				{ return m_nOutsideVolume;}

	void				setUpdateInterval( REAL32 rInterval){ m_rUpdateInterval = rInterval;}
	REAL32				getUpdateInterval( void)			{ return m_rUpdateInterval;}

	void				setFrequency( UINT32 nFreq)			{ m_nFrequency = nFreq;}
	UINT32				getFrequency(void)					{ return m_nFrequency;}

	void				ApplyI3DL2ReverbPreset( UINT32 nPreset)	{	m_nI3DL2RvbPreset = nPreset;}
	UINT32				getI3DL2ReverbPreset(void)				{	return m_nI3DL2RvbPreset;}

	void				setForceStopAtStageEnd( bool bEnd = true){ m_bForceStopAtStopAllSound = bEnd;}
	bool				isForceStopAtStageEnd(void)				{ return m_bForceStopAtStopAllSound;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	void			LoadFromBackground( i3ResourceFile * pFile);

};

#endif
