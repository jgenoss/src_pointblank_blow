#if !defined( __I3_STAGE_INFO_H)
#define __I3_STAGE_INFO_H

#include "i3Base/i3AnyData.h"

enum I3FRM_STAGE_JUMP_TYPE
{
	I3FRM_STAGE_JUMP_SET = 0,
	I3FRM_STAGE_JUMP_PUSH,
	I3FRM_STAGE_JUMP_POP,
};

// i3NamedElement 의 Name속성을 ClassMeta를 저장하는데 사용합니다.
class I3_EXPORT_FRAMEWORK  i3StageInstanceInfo : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3StageInstanceInfo, i3NamedElement);

private:
	INT32					m_nCount = 0;
	INT32					m_bAllocCountFixed = 0;
	REAL32					m_rUpdateInterval = 0;

public:

	void					SetClassNameByMeta( i3ClassMeta * pMeta);
	i3ClassMeta *			GetClassMetaByName();

	void					setInstanceCount( INT32 nCount)	{ m_nCount = nCount;}
	INT32					getInstanceCount()				{ return m_nCount;}

	void					setUpdateInterval( REAL32 rUpdateInterval)	{ m_rUpdateInterval = rUpdateInterval;}
	REAL32					getUpdateInterval()	{	return m_rUpdateInterval;}

	void					setAllocCountFixed( bool bTrue = false) { m_bAllocCountFixed = bTrue ? 1 : 0; }

	bool					isAllocCountFixed()	{	return m_bAllocCountFixed != 0;}

public:

	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
};

typedef i3::vector<i3StageInstanceInfo*>					StageInstanceInfoList;
typedef i3::shared_ptr<i3::vector<i3StageInstanceInfo*> >	StageInstanceInfoListPtr;


class I3_EXPORT_FRAMEWORK i3StageInfo : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3StageInfo, i3NamedElement);
protected:
	i3::rc_string			m_strClassName;
	COLOR					m_colorFI = { 0, 0, 0, 255 };
	REAL32					m_timeFI = 0.4f;
	COLOR					m_colorFO = { 0, 0 , 0, 255 };
	REAL32					m_timeFO = 0.4f;

	i3StageInfo *			m_pNextStage = nullptr;
	I3FRM_STAGE_JUMP_TYPE	m_JumpType = I3FRM_STAGE_JUMP_SET;

	i3::vector<i3::rc_string> m_RSCPath;

	StageInstanceInfoListPtr	m_spInstanceManagerInfoList;
	i3AnyData					m_ElementInstanceManagerInfoList = static_cast<StageInstanceInfoList*>(0);

public:
	virtual ~i3StageInfo(void);

	const char *			getStageClassName(void) const	{ return m_strClassName.c_str(); }
	void					setStageClassName( const char * pszName) { m_strClassName = pszName; }

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
	StageInstanceInfoListPtr&	getInstanceManagerInfoListPtr()	{	return m_spInstanceManagerInfoList;}
	i3AnyData*					getElementInstanceManagerInfoList() { return &m_ElementInstanceManagerInfoList; }

	void					setInstanceManagerInfoList(const StageInstanceInfoListPtr& spList);

	const i3::vector<i3::rc_string>&	getRSCPath( void) const	{ return m_RSCPath;	}
	i3::vector<i3::rc_string>&			getRSCPath( void) { return m_RSCPath; }

	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
};

#endif
