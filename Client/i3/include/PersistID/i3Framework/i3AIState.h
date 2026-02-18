
// ============================================================================
//
// i3AIState											by KOMET 2006.03.13
//
// ============================================================================

#ifndef __I3_AISTATE_H
#define __I3_AISTATE_H

#define I3_AISTATE_STYLE_CALL_FUNC			0x00010000	//	CallBack 함수 호출 (기본)
#define I3_AISTATE_STYLE_CALL_LUA			0x00020000	//	Lua 등록 함수 호출 (예정)
#define I3_AISTATE_STYLE_FIN_TIME			0x00040000	//	시간이 만료되면 해당 AI State를 종료
#define I3_AISTATE_STYLE_FIN_ANIM			0x00080000	//	지정 Animation의 Play가 완료되면 함께 종료
#define	I3_AISTATE_STYLE_BLEND_WITH_STOP	0x00100000	//	Animation Blending을 할 때, 시작부분으로 정지시킨 상태에서 Blending한다.
#define I3_AISTATE_STYLE_ANIM_LOOP			0x00200000	//	Animation의 재생이 완료되면, Play를 처음부터 다시 재생한다. 이 경우에는
														//	Blending이 적용되지 않는다. 이 Style이 없다면 End-Stop 방식으로 한번 재생된 후
														//	마지막 Frame에서 재생을 종료한다.

class i3AIContext;

typedef void	(* I3_AISTATE_PROC)( i3AIContext * pAIState, RT_REAL32 timeDelta);


class I3_EXPORT_FRAMEWORK i3AIState : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3AIState);
public:

protected:
	UINT32				m_Style;

	INT32				m_ID;		

	i3Animation *		m_pAnim;
	REAL32				m_timeScale;
	REAL32				m_timeBlend;
	char * 				m_pRunLuaFuncName;
	char *				m_pFinishLuaFuncName;

	REAL32				m_timeLength;

	i3TimeSequence *	m_pTimeSeq;

	i3AIState *			m_pNext;

public:
	i3AIState( void);
	virtual ~i3AIState( void);

	UINT32				getStyle(void)				{ return m_Style; }
	void				setStyle( UINT32 style)		{ m_Style = style; }
	void				addStyle( UINT32 style)		{ m_Style |= style; }
	void				removeStyle( UINT32 style)	{ m_Style &= ~style; }

	INT32				getID( void)						{	return m_ID;							}
	void				setID( INT32 id)					{ m_ID = id; }

	i3Animation *		getAnim( void)						{	return m_pAnim;							}	
	void				SetAnim( i3Animation * pAnim);

	REAL32				getBlendTime( void)					{	return m_timeBlend;						}
	void				setBlendTime( REAL32 tm)			{ m_timeBlend = tm; }

	REAL32				getTimeScale(void)					{ return m_timeScale; }
	void				setTimeScale( REAL32 tm)			{ m_timeScale = tm; }

	REAL32				getAnimDuration( void)				{	return ((m_pAnim != NULL) ? m_pAnim->GetDuration() : __RT_0);	}

	const char *		getRunLuaProcName(void)				{ return m_pRunLuaFuncName; }
	void				SetRunLuaProcName( char * pszName);

	const char *		getFinishLuaProcName(void)				{ return m_pFinishLuaFuncName; }
	void				SetFinishLuaProcName( char * pszName);

	REAL32				getDuration(void)					{ return m_timeLength; }
	void				setDuration( REAL32 tm)				{ m_timeLength = tm; }

	REAL32				getActualDuration( void)			{ return m_timeLength * (1.0f / m_timeScale);	}
	
	i3TimeSequence *	getTimeSeq(void)					{ return m_pTimeSeq; }
	void				setTimeSeq( i3TimeSequence * pSeq)	{ I3_REF_CHANGE( m_pTimeSeq, pSeq); }

	i3AIState *			getNext(void)						{ return m_pNext; }
	void				setNext( i3AIState * pNext)
	{
		if( pNext != NULL)
			pNext->AddRef();

		I3_SAFE_RELEASE( m_pNext);
		m_pNext = pNext;
	}

	void				OnChangeTime( i3AIContext * pCtx, REAL32 tm);
	void				OnActiveState( i3AIContext * pCtx, RT_REAL32 timeDelta);
	void				OnFinishState( i3AIContext * pCtx, RT_REAL32 timeDelta);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual void	OnQueryStrings( i3StringList * pStrList);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};


#endif // __I3_AISTATE_H