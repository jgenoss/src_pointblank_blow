#if !defined( I3SND_PLAY_ARGUMENT_H__)
#define I3SND_PLAY_ARGUMENT_H__

#include "i3SoundProperty.h"

// i3SoundPlayArgument
// 
// 1. Base
// i3SoundProperty			: 재생시킬 소리정보( NULL)

// 2. Positional Sound
// 3D Mode					: 3d positional sound mode( DISABLE)
// position					: if( 3d) world position( 0.0, 0.0, 0.0)

// 3. direction				: 방향성있는 소리의 경우 방향( 0.0, 0.0, 0.0)
// Inside Cone Angle		: 방향성 있는 소리인경우의 안쪽각도(360)
// OutSide cone angle		: 방향성 있는 소리인경우의 바깥쪽 각도.(360)
// OutSide Volume			: OutSide Cone Angle바깥의 볼륨( %)( 100)

// 4.Update
// Update Interval

// 5. state
// state
class i3SoundPlayState;

class I3_EXPORT_SOUND i3SoundPlayArgument : public i3NamedElement
{
	I3_CLASS_DEFINE( i3SoundPlayArgument);

protected:
	//Volatile members
	i3SoundPlayState	*	m_pPlayState;
	UINT32					m_nState;
	

	//Persistant members
	i3SoundProperty		*	m_pProperty;

	UINT32					m_n3DMode;

	bool					m_bLoopPlay;
	VEC3D					m_vPosition;
	VEC3D					m_vDirection;
	VEC3D					m_vVelocity;

	INT32					m_nInsideConeAngle;
	INT32					m_nOutsideConeAngle;
	UINT8					m_nOutsideVolume;

	REAL32					m_rUpdateInterval;

	UINT32					m_nFrequency;

public:
	i3SoundPlayArgument();
	virtual ~i3SoundPlayArgument();

public:
	void					setState( UINT32 nState)	{	m_nState = nState;}
	void					addState( UINT32 nState)	{	m_nState |= nState;}
	void					removeState( UINT32 nState)	{	m_nState &= ~nState;}
	UINT32					getState( void)				{	return m_nState;}
	bool					isState( UINT32 nState)		{	return ((m_nState & nState) != 0);}

	bool					isLoopPlay()				{	return m_bLoopPlay;}
	void					setLoopPlay( bool bLoop = true)	{	m_bLoopPlay = bLoop;}
	
	i3SoundProperty *		getSoundProperty( void)		{	return m_pProperty;}
	void					setSoundProperty( i3SoundProperty * pProperty)	{	I3_REF_CHANGE( m_pProperty, pProperty);}

	void					setPlayState( i3SoundPlayState * pState);
	i3SoundPlayState *		getPlayState( void)							{	return m_pPlayState;}

	void					set3DMode( UINT32 n3DMode)	{	m_n3DMode = n3DMode;}
	UINT32					get3DMode( void)			{	return m_n3DMode;}

	void					setPosition( VEC3D * pPos)	{	i3Vector::Copy( &m_vPosition, pPos);}
	VEC3D			*		getPosition( void)			{	return &m_vPosition;}

	void					setDirection( VEC3D * pDir)	{	i3Vector::Copy( &m_vDirection, pDir);}
	VEC3D			*		getDirection(void)			{	return &m_vDirection;}

	void					setVelocity( VEC3D * pVel)	{	i3Vector::Copy( &m_vVelocity, pVel);}
	VEC3D			*		getVelocity(void)			{	return &m_vVelocity;}

	void					setInsideConeAngle( INT32 nAngle)	{	m_nInsideConeAngle = nAngle;}
	void					setOutsideConeAngle( INT32 nAngle)	{	m_nOutsideConeAngle = nAngle;}
	void					setOutsideVolume( UINT8 nVolume)	{	m_nOutsideVolume = nVolume;}

	INT32					getInsideConeAngle(void)			{	return m_nInsideConeAngle;}
	INT32					getOutsideConeAngle(void)			{	return m_nOutsideConeAngle;}
	UINT8					getOutsideVolume(void)				{	return m_nOutsideVolume;}

	void					setUpdateInterval( REAL32 updateInterval)	{	m_rUpdateInterval = updateInterval;}
	REAL32					getUpdateInterval( void)					{	return m_rUpdateInterval;}

	void					setFrequency( UINT32 nFreq)					{	m_nFrequency = nFreq;}
	UINT32					getFrequency( void)							{	return m_nFrequency;}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif