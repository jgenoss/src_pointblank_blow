#ifndef __I3_GAME_NODE_H
#define __I3_GAME_NODE_H

#include "i3FrameworkPCH.h"

// ============================================================================
//
// i3GameNode
//
// ============================================================================

#define	I3_GAMENODE_STATE_USED				0x00000001
#define	I3_GAMENODE_STATE_DISABLE			0x00000002
#define	I3_GAMENODE_STATE_ATTACHED			0x00000004
#define I3_GAMENODE_STATE_PAUSED			0x00000008
#define I3_GAMENODE_STATE_INVISIBLE			0x00000010
#define I3_GAMENODE_STATE_INPROGRESS		0x00000020
#define I3_GAMENODE_STATE_COMPLETED			0x00000040
#define	I3_GAMENODE_STATE_LOCKED			0x00000080
#define	I3_GAMENODE_STATE_DYNAMIC_DISABLE	0x00000100
#define I3_GAMENODE_STATE_DISABLING			0x00000200
#define I3_GAMENODE_STATE_ENABLING			0x00000400
#define I3_GAMENODE_STATE_INVISIBLING		0x00000800
#define I3_GAMENODE_STATE_VISIBLING			0x00001000

#define	I3_GAMENODE_STYLE_VOLATILE		0x00000001
#define	I3_GAMENODE_STYLE_NODETACH		0x00000002
#define I3_GAMENODE_STYLE_FIXEDCOUNT	0x00000004	//생성시 정해진 숫자만큼만 쓰고 모자란경우 더생성하지 않는는다.

class I3_EXPORT_FRAMEWORK i3GameNode : public i3EventReceiver
{
	I3_CLASS_DEFINE( i3GameNode );
protected:
	// Persist Members
	UINT32				m_Style;
	UINT32				m_State;
	i3GameNode *		m_pParent;

	RT_REAL32			m_UpdateInterval;
	RT_REAL32			m_LifeTime;

	// Volatile Members
//	UINT32				m_InnerState;
	i3GameNode *		m_pNext;
	RT_REAL32			m_timeUpdate;
	RT_REAL32			m_tmLife;
	i3GameNode *		m_pChild;
	RT_REAL32			m_rElapsedTime;

	REAL32				m_timeTargetFade;
	REAL32				m_timeFade;

protected:

	void				_DriveFade( REAL32 tm);

	virtual	void		_ForceEnable( BOOL bFlag);
	virtual	void		_ForceVisible( BOOL bFlag);

public:
	i3GameNode(void);
	virtual ~i3GameNode(void);

	virtual void	Create( i3GameNode * pParentNode);
	
	void		setStyle( UINT32 style)					{ m_Style = style; }
	UINT32		getStyle( void)							{ return m_Style; }
	void		addStyle( UINT32 style)					{ m_Style |= style; }
	void		removeStyle( UINT32 style)				{ m_Style &= ~style; }

	void		setState( UINT32 State)					{ m_State = State;	}
	UINT32		getState(void)							{ return m_State;	}
	void		addState( UINT32 state)					{ m_State |= state; }
	void		removeState( UINT32 state)				{ m_State &= ~state; }

	BOOL		isUsed(void)							{ return (m_State & I3_GAMENODE_STATE_USED) != 0; }
	BOOL		isEnable(void)							{ return (m_State & I3_GAMENODE_STATE_DISABLE) == 0; }
	bool		isState( UINT32 state)					{ return (m_State & state) != 0; }
	void		SetEnable( BOOL bTrue = TRUE, REAL32 tm = 0.0f);

	/*
	void		Enable(void)
	{ 
		removeState( I3_GAMENODE_STATE_DISABLE);
		SetVisible( TRUE );
	}

	void		Disable(void)
	{ 
		addState( I3_GAMENODE_STATE_DISABLE);
		SetVisible( FALSE );
	}
	*/

	BOOL		isPaused()								{ return (m_State & I3_GAMENODE_STATE_PAUSED) != 0; }
	void		setPaused( BOOL bTrue = TRUE )
	{
		if( bTrue ) addState( I3_GAMENODE_STATE_PAUSED );
		else		removeState( I3_GAMENODE_STATE_PAUSED );
	}

	BOOL			isVisible()							{ return 0 == ( m_State & I3_GAMENODE_STATE_INVISIBLE ); }
	void			SetVisible( BOOL bTrue = TRUE, REAL32 tm = 0.0f);

	
	virtual void	ReturnInstance();

	i3GameNode *	getParent(void)						{ return m_pParent; }
	void			setParent( i3GameNode * pParent)	{ m_pParent = pParent; }

	RT_REAL32		getLifeTime(void)					{ return m_LifeTime; }
	void			setLifeTime( RT_REAL32 tm)			{ m_LifeTime = tm; }

	void			setElapsedTime( RT_REAL32 rSetSeconds ){ m_rElapsedTime = rSetSeconds; }
	RT_REAL32		getElapsedTime(){ return m_rElapsedTime; }

	// Enable/Diabling, Visibling/Invisibling 상태 중일때,
	// 거기에 맞춘 0~1 비율이 반환된다.
	// Alpha 값으로 직접 사용 가능.
	REAL32			getFadeRate(void)
	{
		if( m_timeTargetFade <= 0.0f)
			return 0.0f;

		if( isState( I3_GAMENODE_STATE_DISABLING | I3_GAMENODE_STATE_INVISIBLING))
			return m_timeFade / m_timeTargetFade;

		return 1.0f - (m_timeFade / m_timeTargetFade);
	}

	bool			isFading(void)
	{
		return (m_timeTargetFade > 0.0f);
	}

#ifdef I3_DEBUG
	void			AddChild( i3GameNode *pChild, const char *szFile, INT32 nLine );
#endif
	void			AddChild( i3GameNode *pChild )		{ AddChildToTail( pChild ); }

#ifdef I3_DEBUG
	void			AddChildToHead( i3GameNode *pChild, const char *szFile, INT32 nLine );
#endif
	void			AddChildToHead( i3GameNode * pChild);
	void			AddChildToTail( i3GameNode * pChild);
	void			RemoveChild( i3GameNode * pChild);
	void			RemoveAllChild(void);
	//void			DetachChild( i3GameNode * pChild);
	i3GameNode *	FindChildByName( const char * pszName, BOOL bRecurs = TRUE);
	i3GameNode *	getFirstChild(void)					{ return m_pChild; }
	i3GameNode *	GetChildByIndex( INT32 nIndex );
	INT32			GetChildIndex( i3GameNode * pChild);

	i3GameNode *	getNext(void)						{ return m_pNext; }
	void			SetNext( i3GameNode * pNext);

	BOOL			setGroupEnable( i3GameNode * pNode);

	RT_REAL32		getUpdateInterval(void)				{ return m_UpdateInterval; }
	void			setUpdateInterval( RT_REAL32 tm)	{ m_UpdateInterval = tm; }

	virtual void	OnChangeTime( REAL32 tm);
	virtual void	OnUpdate( RT_REAL32 rDeltaSeconds );
	virtual void	OnAttach(void);
	virtual void	OnDetach(void);
	virtual void	OnFadeStart(void);
	virtual void	OnFading( REAL32 tm, REAL32 rate);
	virtual void	OnFadeEnd(void);

	virtual void	OnReloadProperty(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	OnInstancing( void * pUserDefault = NULL);

	static i3GameNode * CreateClone( i3GameNode * pNode, I3_COPY_METHOD method);
};

#ifdef I3_DEBUG
#define I3_GAMENODE_ADDCHILD( P, C )		P->AddChild( C, __FILE__, __LINE__ );
#define I3_GAMENODE_ADDCHILDTOHEAD( P, C )	P->AddChildToHead( C, __FILE__, __LINE__ );
#else
#define I3_GAMENODE_ADDCHILD( P, C )		P->AddChild( C );
#define I3_GAMENODE_ADDCHILDTOHEAD( P, C )	P->AddChildToHead( C );
#endif


#endif // __I3_GAME_OBJ_H
