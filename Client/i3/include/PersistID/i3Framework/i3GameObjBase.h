#if !defined( __I3_GAME_OBJ_BASE_H)
#define __I3_GAME_OBJ_BASE_H

#include "i3GameNode.h"
#include "i3GameResSceneGraph.h"

////////////////////////////////////////////////////////////////////////////////////
//  Network Types
enum I3_NETWORK_GAMEOBJECT_TYPE
{
	I3_NETWORK_GAMEOBJECT_TYPE_NONE = 0,
	I3_NETWORK_GAMEOBJECT_TYPE_STATIC,			// 고정되어 있다
	I3_NETWORK_GAMEOBJECT_TYPE_MOVE,			// 이동값만
	I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC,			// 이동 + 회전
	I3_NETWORK_GAMEOBJECT_TYPE_ANIM,			// 애니메이션 키프레임
};

class i3GameSceneInfo;
class i3GameObjRef;

class I3_EXPORT_FRAMEWORK i3GameObjBase : public i3GameNode
{
	I3_CLASS_DEFINE( i3GameObjBase);
protected:
	UINT32							m_Type;
	i3Node *						m_pSceneNode;
	I3_NETWORK_GAMEOBJECT_TYPE		m_NetworkSyncMode;

	// Persist Members
	i3GameResSceneGraph	 *			m_pGameRes;

	i3List *						m_pRefClassList;
protected:
	virtual void					_ForceEnable( BOOL bFlag);
	virtual void					_ForceVisible( BOOL bFlag);

public:
	i3GameObjBase(void);
	virtual ~i3GameObjBase(void);

	virtual BOOL	Create( i3Node *pParentNode = NULL, BOOL bEnable = TRUE );
	virtual void	SetInstanceSg( i3Node * pNode);			// Internal use only; GameRes에서 호출되는 함수.

	virtual void SetType( UINT32 Type)					{ m_Type = Type;	}
	UINT32				GetType(void)					{ return m_Type;	}

	i3Node*				GetNode(void)					{ return m_pSceneNode; }
	void				SetNode( i3Node * pNode);

	void				InitColliderSet( INT32 GrpIdx, UINT32 mask, i3Node * pRoot = NULL );

	void				VisibleScene(void);
	void				InvisibleScene(void);
	BOOL				IsVisibleScene(void)					{ return ! m_pSceneNode->IsFlag( I3_NODEFLAG_INVISIBLE);}

	void				EnableScene(void);
	void				DisableScene(void);
	BOOL				IsEnableScene(void)						{ return ! m_pSceneNode->IsFlag( I3_NODEFLAG_DISABLE); }

	void				addRefObject( i3ResourceObject * pObj);
	void				removeRefObject( i3ResourceObject * pObj);
	void				removeAllRefObject(void);
	bool				isRefObject( i3ResourceObject * pObj);
	i3ResourceObject *	FindRefObjectByName( const char * pszName);
	INT32				getRefObjectCount(void)
	{ 
		if( m_pRefClassList == NULL)
			return 0;

		return m_pRefClassList->GetCount(); 
	}
	i3ResourceObject *	getRefObject( INT32 idx)			{ return (i3ResourceObject *) m_pRefClassList->Items[idx]; }

	void		AttachScene(void);
	void		DetachScene(void);
	BOOL		IsAttachedScene(void)						{ return (m_State & I3_GAMENODE_STATE_ATTACHED) != 0; }

	virtual I3_NETWORK_GAMEOBJECT_TYPE		getNetworkSyncMode(void)			{ return m_NetworkSyncMode; }
	virtual void							setNetworkSyncMode( I3_NETWORK_GAMEOBJECT_TYPE mode)
	{
		m_NetworkSyncMode = mode;
	}

	i3GameResSceneGraph *	getSgRes(void)						{ return m_pGameRes;	}
	virtual void			BindSgRes( i3Framework * pFramework, i3GameResSceneGraph * pRes, bool bInstancing = true, bool bClone = true);

	//virtual	i3Node *	OnQueryAttachNode(void);
	virtual void		OnBindResource(void);
	virtual void		OnChangeTime( REAL32 tm);
	virtual void		OnInitAfterLoad(void);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual void		OnInstancing( void * pUserDefault = NULL);

	//
	// Level Tool에서 게임 의존적인 추가 Data를 저장하기 위해 호출한다.
	// OnQueryGameData() 함수 내에서는 필요한 메모리를 할당해 반환해야 하며
	// 실제로 할당된 메모리의 크기를 반환한다.
	//
	// Level Tool은 반환된 메모리를 i3MemFree() 함수를 통해 해제하기 때문에
	// 반드시 i3MemAlloc() 함수를 통해 할당된 메모리여야 한다.
	//
	// Paramters
	//		pData	[OUT]		저장할 데이터를 위해 할당한 메모리 버퍼. 함수 내에서 할당된다.
	//
	// Return
	//		할당된 메모리 버퍼의 크기를 Byte 단위로 반환한다.
	//
	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData);

	//
	// i3GameSceneInfo Class에서 게임용 Instance를 Loading하는 중에 호출한다.
	// OnQueryGameData() 함수에 전달된 것과 동일한 처리를 해야한다.
	// 실제로 읽어들인 메모리를 반환해야 하며, 만약 처리 중 Error가 발생했다면
	// STREAM_ERR을 반환해야 한다.
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz);
};

#endif
