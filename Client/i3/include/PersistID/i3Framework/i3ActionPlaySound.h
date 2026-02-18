#if !defined( __I3_ACTION_PLAY_SOUND_H)
#define __I3_ACTION_PLAY_SOUND_H

class I3_EXPORT_FRAMEWORK i3ActionPlaySound : public i3Action
{
	I3_CLASS_DEFINE( i3ActionPlaySound);

protected:
	bool			m_bEnableRandom;	//랜덤 설정 여부
	UINT16			m_nRandomFactor;	//랜덤시 사용될 인자

	i3SoundNode	*	m_pSndNode;

	bool			m_bAttachedBone;
	i3MemoryBuffer *m_pAttachBone;

public:
	i3ActionPlaySound(void);
	virtual ~i3ActionPlaySound(void);

	void	setSoundNode( i3SoundNode * pSndObj)	{	I3_REF_CHANGE( m_pSndNode, pSndObj);}
	i3SoundNode * getSoundNode( void)				{	return m_pSndNode;}

	void	setRandomEnable( bool bEnable = true)	{	m_bEnableRandom = bEnable;}
	void	setRandomFactor( UINT16 nFactor)		{ 	m_nRandomFactor	= nFactor;}

	bool	isRandomEnable(void)					{	return m_bEnableRandom;}
	UINT16	getRandomFactor(void)					{	return m_nRandomFactor;}

	//Attach
	bool			getAttachEnable(void)				{ return m_bAttachedBone; }
	void			setAttachEnable( bool bFlag)		{ m_bAttachedBone = bFlag;}
	char *			getAttachBoneName(void)				
	{ 
		if( m_pAttachBone == NULL)
			return NULL;

		return (char *) m_pAttachBone->getBuffer(); 
	}

	void			SetAttachBoneName( const char * pszName);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual void	OnChangeTime( REAL32 tm);
	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	virtual void	OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
