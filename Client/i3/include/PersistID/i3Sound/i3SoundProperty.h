#if !defined( __I3SND_PROPERTY_H__)
#define __I3SND_PROPERTY_H__

#include "i3Sound.h"

// i3Sound Property
// File Format

// 1. Base
// SourceSoundFile			: 재생시킬 음원파일(NULL)

// 2. Distance Information
// DistMin 					: 소리가 작아지기 시작하는 거리(10)
// DistMax 					: 최대 가청 거리(1000000000)
// bMuteAtMaxDistance		: 최대 가청거리를 벗어난경우 묵음처리할것인지의 여부(true)

// 3. PriorityInfo
// Priority Type			: 우선순위 종류(distance, elapsed playback time)
//	if( PriorityType == distance)
//	Priority Level			: 우선순위 레벨

// 3. Volume Information				      
// DefVolume				: 소리의 기본 볼륨( 100)
// ListenerVolume			: 3D Mode가 Disable상태인경우의 볼륨.(100%)
// PositionalVolume			: 3D Mode가 작동하고있는 소리의경우의 볼륨(100%)

class I3_EXPORT_SOUND i3SoundProperty : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3SoundProperty);

public:
	i3SoundProperty();
	virtual ~i3SoundProperty();

protected:
	i3Sound *			m_pSourceSound;
	bool				m_bStream;

	UINT32				m_nSoundType;

	REAL32				m_rMinDistance;
	REAL32				m_rMaxDistance;
	bool				m_bMuteAtMaxDistance;

	UINT8				m_nSourceVolume;			// 0 ~ 100
	UINT8				m_nListenerVolume;			// 0 ~ 100%
	UINT8				m_nPositionalVolume;		// 0 ~ 100%

	UINT32				m_nPriorityType;
	UINT32				m_nPriorityLevel;

public:
	i3Sound *			getSourceSound(void)				{	return m_pSourceSound; }
	void				setSourceSound( i3Sound * pSnd)		{	I3_REF_CHANGE( m_pSourceSound, pSnd);}

	UINT32				getSoundType( void)					{	return m_nSoundType;}
	void				setSoundType( UINT32 nType)			{	m_nSoundType = nType;}

	bool				isStream( void)						{	return m_bStream;}
	void				setStreamming( bool bStream)		{	m_bStream = bStream;}

	UINT8				getSourceVolume( void)				{	return m_nSourceVolume;}
	void				setSourceVolume( UINT8 nVolume)		{	m_nSourceVolume = nVolume;}

	UINT8				getListenerVolume(void)				{	return m_nListenerVolume;}
	void				setListenerVolume( UINT8 nVolume)	{	m_nListenerVolume = nVolume;}

	UINT8				getPositionalVolume(void)			{	return m_nPositionalVolume;}
	void				setPositionalVolume( UINT8 nVolume)	{	m_nPositionalVolume = nVolume;}

	UINT32				getPriorityType(void)				{	return m_nPriorityType;}
	void				setPriorityType( UINT32 nType)		{	m_nPriorityType = nType;}

	UINT32				getPriorityLevel(void)				{	return m_nPriorityLevel;}
	void				setPriorityLevel( UINT32 nLevel)	{	m_nPriorityLevel = nLevel;}

	void				setDistanceInfo( REAL32 rMax, REAL32 rMin, bool bMuteAtMaxDistance)
	{
		m_rMinDistance			= rMin;
		m_rMaxDistance			= rMax;
		m_bMuteAtMaxDistance	= bMuteAtMaxDistance;
	}

	REAL32				getMinDistance(void)			{ return m_rMinDistance; }
	REAL32				getMaxDistance(void)			{ return m_rMaxDistance; }
	bool				isMuteAtMaxDistance( void)		{ return m_bMuteAtMaxDistance;}

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
