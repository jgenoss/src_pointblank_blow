#if !defined( __I3_STAGE_INFO_H)
#define __I3_STAGE_INFO_H

enum I3FRM_STAGE_JUMP_TYPE
{
	I3FRM_STAGE_JUMP_SET = 0,
	I3FRM_STAGE_JUMP_PUSH,
	I3FRM_STAGE_JUMP_POP,
};

// i3NamedElement 의 Name속성을 ClassMeta를 저장하는데 사용합니다.
class I3_EXPORT_FRAMEWORK  i3StageInstanceInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( i3StageInstanceInfo);

public:
	INT32					m_nCount;
	INT32					m_bAllocCountFixed;
	REAL32					m_rUpdateInterval;

	void					SetClassNameByMeta( i3ClassMeta * pMeta);
	i3ClassMeta *			GetClassMetaByName();

	void					setInstanceCount( INT32 nCount)	{ m_nCount = nCount;}
	INT32					getInstanceCount()				{ return m_nCount;}

	void					setUpdateInterval( REAL32 rUpdateInterval)	{ m_rUpdateInterval = rUpdateInterval;}
	REAL32					getUpdateInterval()	{	return m_rUpdateInterval;}

	void					setAllocCountFixed( BOOL bTrue = FALSE)
	{
		m_bAllocCountFixed = bTrue;
	}

	BOOL					isAllocCountFixed()	{	return m_bAllocCountFixed;}

public:
	i3StageInstanceInfo();
	virtual ~i3StageInstanceInfo();

	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};

class I3_EXPORT_FRAMEWORK i3StageInfo : public i3NamedElement
{
	I3_CLASS_DEFINE( i3StageInfo);
protected:
	char					m_szClassName[64];
	COLOR					m_colorFI;
	REAL32					m_timeFI;
	COLOR					m_colorFO;
	REAL32					m_timeFO;

	i3StageInfo *			m_pNextStage;
	I3FRM_STAGE_JUMP_TYPE	m_JumpType;

	i3StringList			m_RSCPath;

	i3List				*	m_pInstanceManagerInfoList;
public:
	i3StageInfo(void);
	virtual ~i3StageInfo(void);

	char *					getStageClassName(void)			{ return m_szClassName; }
	void					setStageClassName( char * pszName)
	{
		i3String::NCopy( m_szClassName, pszName, sizeof(m_szClassName) - 1);
	}

	COLOR *					getFadeInColor(void)			{ return &m_colorFI; }
	void					setFadeInColor( COLOR * pColor)
	{
		i3Color::Set( &m_colorFI, pColor);
	}

	COLOR *					getFadeOutColor(void)			{ return &m_colorFO; }
	void					setFadeOutColor( COLOR * pColor)
	{
		i3Color::Set( &m_colorFO, pColor);
	}

	REAL32					getFadeInTime(void)				{ return m_timeFI; }
	void					setFadeInTime( REAL32 tm)		{ m_timeFI = tm; }

	REAL32					getFadeOutTime(void)			{ return m_timeFO; }
	void					setFadeOutTime( REAL32 tm)		{ m_timeFO = tm; }

	i3StageInfo *			getNextStage(void)				{ return m_pNextStage; }
	void					setNextStage( i3StageInfo * pInfo){ I3_REF_CHANGE( m_pNextStage, pInfo);}

	I3FRM_STAGE_JUMP_TYPE	getJumpType(void)				{ return m_JumpType; }
	void					setJumpType( I3FRM_STAGE_JUMP_TYPE type)
	{
		m_JumpType = type;
	}

	INT32					getInstanceManagerInfoCount();
	i3List			*		getInstanceManagerInfoList()	{	return m_pInstanceManagerInfoList;}
	void					setInstanceManagerInfoList(i3List * pList);

	i3StringList *			getRSCPath( void)				{ return &m_RSCPath;	}

	virtual void			OnBuildObjectList( i3List * pList);
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};

#endif
