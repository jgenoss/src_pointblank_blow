#ifndef __I3_GAME_NODE_H
#define __I3_GAME_NODE_H

#include "i3FrameworkPCH.h"

// ============================================================================
//
// i3GameNode
//
// ============================================================================

typedef UINT32 I3_GAMENODE_STATE;
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
#define I3_GAMENODE_STATE_PRE_LOADING		0x00010000		// Stage가 시작할 때, Resource가 모두 Loading되어야 함을 의미함.
#define I3_GAMENODE_STATE_LOSTDEVICE		0x00020000		// OnLostDevice가 호출되었다.

#define	I3_GAMENODE_STYLE_VOLATILE		0x00000001
#define	I3_GAMENODE_STYLE_NODETACH		0x00000002
#define I3_GAMENODE_STYLE_FIXEDCOUNT	0x00000004	//생성시 정해진 숫자만큼만 쓰고 모자란경우 더생성하지 않는는다.
#define I3_GAMENODE_STYLE_XRAY			0x00000008	//xray형식

class I3_EXPORT_FRAMEWORK i3GameNode : public i3EventReceiver
{
	I3_EXPORT_CLASS_DEFINE( i3GameNode, i3EventReceiver );

public:
	typedef bool	(* TRAVERSEPROC)( i3GameNode * pNode, void * pUserData);

protected:
	// Persist Members
	UINT32				m_GameNodeStyle = 0;
	UINT32				m_GameNodeState = 0;
	i3GameNode *		m_pParent = nullptr;

	REAL32			m_UpdateInterval = 0.0f;

	// Volatile Members
	i3GameNode *		m_pNext = nullptr;
	REAL32			m_timeUpdate = 0.0f;
	bool				m_bPrevUpdateChild = false;					// 이 전 Update에서 Child에 대해 Update를 수행했었는가를 나타낸다.

	i3GameNode *		m_pChild = nullptr;
	REAL32			m_rElapsedTime = 0.0f;
	UINT32				m_LastUpdateID = 0;

	REAL32				m_FadeInterval = 0.0f;

	REAL32				m_timeTargetFade = 0.0f;
	REAL32				m_timeFade = 0.0f;

	i3GameNode *		m_pGameNodeLinkLeft = nullptr;
	i3GameNode *		m_pGameNodeLinkRight = nullptr;

protected:
	void				_Link(void);
	void				_Unlink(void);

	void				_DriveFade( REAL32 tm);

	virtual	void		_ForceEnable( bool bFlag);
	virtual	void		_ForceVisible( bool bFlag);

public:
	i3GameNode(void);
	virtual ~i3GameNode(void);

	virtual void	Create( i3GameNode * pParentNode);
	
	void		setGNodeStyle( UINT32 style)					{ m_GameNodeStyle = style; }
	UINT32		getGNodeStyle( void)							{ return m_GameNodeStyle; }
	void		addGNodeStyle( UINT32 style)					{ m_GameNodeStyle |= style; }
	void		removeGNodeStyle( UINT32 style)				{ m_GameNodeStyle &= ~style; }

	void		setGNodeState( I3_GAMENODE_STATE State)					{ m_GameNodeState = State;	}
	I3_GAMENODE_STATE		getGNodeState(void)							{ return m_GameNodeState;	}
	void		addGNodeState( I3_GAMENODE_STATE state)					{ m_GameNodeState |= state; }
	void		removeGNodeState( I3_GAMENODE_STATE state)				{ m_GameNodeState &= ~state; }

	bool		isUsed(void)								{ return (m_GameNodeState & I3_GAMENODE_STATE_USED) != 0; }
	bool		isEnable(void)								{ return (m_GameNodeState & I3_GAMENODE_STATE_DISABLE) == 0; }
	bool		isGNodeState( I3_GAMENODE_STATE state)		{ return (m_GameNodeState & state) != 0; }
	virtual void		SetEnable( bool bTrue = true, REAL32 tm = 0.0f);

	bool		isPaused()								{ return (m_GameNodeState & I3_GAMENODE_STATE_PAUSED) != 0; }
	void		setPaused( bool bTrue = true)
	{
		if( bTrue ) addGNodeState( I3_GAMENODE_STATE_PAUSED );
		else		removeGNodeState( I3_GAMENODE_STATE_PAUSED );
	}

	bool			isVisible()							{ return 0 == ( m_GameNodeState & I3_GAMENODE_STATE_INVISIBLE ); }
	void			SetVisible( bool bTrue = true, REAL32 tm = 0.0f);

	
	virtual void	ReturnInstance();

	i3GameNode *	getParent(void)						{ return m_pParent; }
	void			setParent( i3GameNode * pParent)	{ m_pParent = pParent; }

	void			setElapsedTime( REAL32 rSetSeconds ){ m_rElapsedTime = rSetSeconds; }
	REAL32		getElapsedTime(){ return m_rElapsedTime; }

	// Enable/Diabling, Visibling/Invisibling 상태 중일때,
	// 거기에 맞춘 0~1 비율이 반환된다.
	// Alpha 값으로 직접 사용 가능.
	REAL32			getFadeRate(void)
	{
		if( m_timeTargetFade <= 0.0f)
			return 0.0f;

		if( isGNodeState( I3_GAMENODE_STATE_DISABLING | I3_GAMENODE_STATE_INVISIBLING))
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
	i3GameNode *	FindChildByName( const char * pszName, bool bRecurs = true);
	i3GameNode *	getFirstChild(void)					{ return m_pChild; }
	i3GameNode *	GetChildByIndex( INT32 nIndex );
	INT32			GetChildIndex( i3GameNode * pChild);
	INT32			GetChildCount(void);
	i3GameNode *	FindChildByType( const i3::class_meta * pMeta, bool bRecursive = true);

	void			ChangeParent( i3GameNode * pParent);

	void			Dump(void);

	i3GameNode *	getNext(void)						{ return m_pNext; }
	void			SetNext( i3GameNode * pNext);

	bool			setGroupEnable( i3GameNode * pNode);

	REAL32		getUpdateInterval(void)				{ return m_UpdateInterval; }
	void			setUpdateInterval( REAL32 tm)	{ m_UpdateInterval = tm; }

	REAL32			getFadeInterval( void)				{ return m_FadeInterval; }
	void			setFadeInterval( REAL32 tm)			{ m_FadeInterval = tm; }

	virtual bool	Traverse( i3GameNode::TRAVERSEPROC pUserProc, void * pUserData);

	virtual void	OnChangeTime( REAL32 tm);
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	OnAttach(void) {}
	virtual void	OnDetach(void) {}
	virtual void	OnFadeStart(void);
	virtual void	OnFading( REAL32 tm, REAL32 rate);
	virtual void	OnFadeEnd(void);

	virtual void	OnReloadProperty(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	OnInstancing( I3_ONINSTANCING_INFO * pInfo);
	virtual void	Instantiate( void);

	virtual bool	OnLostDevice( bool bLostDevice);
	virtual bool	OnRevive( i3RenderContext * pCtx);
	virtual void	OnRecreate( void) {}

	static i3GameNode * CreateClone( i3GameNode * pNode, I3_COPY_METHOD method);

	static void		CallLostDevice( bool bLostDevice);
	static void		CallRevive( i3RenderContext * pCtx);
	static void		CallRecreate( void);

	static void		DeleteStaticMember();

	virtual void	AfterLoadChara(INT32 idx = -1){}
};

#ifdef I3_DEBUG
#define I3_GAMENODE_ADDCHILD( P, C )		P->AddChild( C, __FILE__, __LINE__ );
#define I3_GAMENODE_ADDCHILDTOHEAD( P, C )	P->AddChildToHead( C, __FILE__, __LINE__ );
#else
#define I3_GAMENODE_ADDCHILD( P, C )		P->AddChild( C );
#define I3_GAMENODE_ADDCHILDTOHEAD( P, C )	P->AddChildToHead( C );
#endif


#endif // __I3_GAME_OBJ_H
