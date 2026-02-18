
// ============================================================================
//
// i3AIState											by KOMET 2006.03.13
//
// ============================================================================

#ifndef __I3_AISTATE_H
#define __I3_AISTATE_H

typedef UINT32 I3_AISTATE_STYLE;
#define I3_AISTATE_STYLE_CALL_FUNC			0x00010000	//	CallBack 함수 호출 (기본)
#define I3_AISTATE_STYLE_CALL_LUA			0x00020000	//	Lua 등록 함수 호출 (예정)
#define I3_AISTATE_STYLE_FIN_TIME			0x00040000	//	시간이 만료되면 해당 AI State를 종료
#define I3_AISTATE_STYLE_FIN_ANIM			0x00080000	//	지정 Animation의 Play가 완료되면 함께 종료
#define	I3_AISTATE_STYLE_BLEND_WITH_STOP	0x00100000	//	Animation Blending을 할 때, 시작부분으로 정지시킨 상태에서 Blending한다.
#define I3_AISTATE_STYLE_ANIM_LOOP			0x00200000	//	Animation의 재생이 완료되면, Play를 처음부터 다시 재생한다. 이 경우에는
														//	Blending이 적용되지 않는다. 이 Style이 없다면 End-Stop 방식으로 한번 재생된 후
														//	마지막 Frame에서 재생을 종료한다.
#define	I3_AISTATE_STYLE_ANIM_TEMPLATE		0x00400000	// 실제 Animation에 대한 연결을 가지고 있는 것이 아니라, Animation의 이름만 가지고 있다.

class i3AIContext;

typedef void	(* I3_AISTATE_PROC)( i3AIContext * pAIState, REAL32 timeDelta);

#define	ENABLE_LUAFUNC


class I3_EXPORT_FRAMEWORK i3AIState : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE( i3AIState, i3ResourceObject);
public:

protected:
	//	기본으로 콜백 호출
	I3_AISTATE_STYLE	m_Style = I3_AISTATE_STYLE_CALL_FUNC;

	INT32				m_ID = -1;

	i3Animation *		m_pAnim = nullptr;
	i3::rc_string		m_AnimName;
	REAL32				m_timeScale = 1.0f;
	REAL32				m_timeBlend = 0.0f;
	REAL32				m_timeLength = 0.0f;

	i3TimeSequence *	m_pTimeSeq = nullptr;

	i3AIState *			m_pNext = nullptr;

#if defined( ENABLE_LUAFUNC)
protected:
	i3::rc_string		m_strRunLuaFuncName;
	i3::rc_string		m_strFinishLuaFuncName;

public:
	const char *		getRunLuaProcName(void)				{ return m_strRunLuaFuncName.c_str(); }
	void				SetRunLuaProcName( const char * pszName);

	const char *		getFinishLuaProcName(void)				{ return m_strFinishLuaFuncName.c_str(); }
	void				SetFinishLuaProcName( const char * pszName);
#endif

public:
	i3AIState();
	virtual ~i3AIState( void);

	I3_AISTATE_STYLE	getStyle(void)							{ return m_Style; }
	void				setStyle( I3_AISTATE_STYLE style)		{ m_Style = style; }
	void				addStyle( I3_AISTATE_STYLE style)		{ m_Style |= style; }
	void				removeStyle( I3_AISTATE_STYLE style)	{ m_Style &= ~style; }

	INT32				getID( void)						{	return m_ID;							}
	void				setID( INT32 id)					{ m_ID = id; }

	i3Animation *		getAnim( void)						{	return m_pAnim;							}	
	void				SetAnim( i3Animation * pAnim);

	const char *			getAnimName(void) const				{	return m_AnimName.c_str(); }
	const i3::rc_string&	getAnimNameString() const			{ return m_AnimName; }
	void				setAnimName(const i3::rc_string& strName)	{ m_AnimName = strName; }

	REAL32				getBlendTime( void)					{	return m_timeBlend;						}
	void				setBlendTime( REAL32 tm)			{ m_timeBlend = tm; }

	REAL32				getTimeScale(void)					{ return m_timeScale; }
	void				setTimeScale( REAL32 tm)			
	{ 
		m_timeScale = tm;
		I3ASSERT( tm > I3_EPS);

		if( m_pTimeSeq != nullptr)	m_pTimeSeq->setTimeScale( m_timeScale);
	}

	REAL32				getAnimDuration( void)				{	return ((m_pAnim != nullptr) ? m_pAnim->GetDuration() : 0.0f);	}	

	REAL32				getDuration(void)					{ return m_timeLength; }
	void				setDuration( REAL32 tm)				{ m_timeLength = tm; }

	REAL32				getActualDuration( void)			{ return m_timeLength * (1.0f / m_timeScale);	}
	
	i3TimeSequence *	getTimeSeq(void)					{ return m_pTimeSeq; }
	void				setTimeSeq( i3TimeSequence * pSeq)	{ I3_REF_CHANGE( m_pTimeSeq, pSeq); }

	i3AIState *			getNext(void)						{ return m_pNext; }
	void				setNext( i3AIState * pNext)			{ I3_REF_CHANGE( m_pNext, pNext);	}

	void				OnChangeTime( i3AIContext * pCtx, REAL32 tm);
	void				OnActiveState( i3AIContext * pCtx, REAL32 timeDelta);
	void				OnFinishState( i3AIContext * pCtx, REAL32 timeDelta);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void	OnQueryStrings( i3::vector<i3::rc_string>& StrList) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};


#endif // __I3_AISTATE_H