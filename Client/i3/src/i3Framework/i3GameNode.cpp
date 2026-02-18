#include "i3FrameworkPCH.h"
#include "i3GameNode.h"
#include "i3Object.h"

//I3_CLASS_INSTANCE( i3GameNode, i3EventReceiver);
I3_CLASS_INSTANCE( i3GameNode);

static	i3GameNode *		s_pGameNodeLinkHead = nullptr;
static  i3Mutex *			s_pGameNodeLinkMutex = nullptr;

// RefCount에 대한 정의.
//
// 기본적인 RefCount의 증가는 아래와 같은 상황에서만 발생한다.
// 1. m_pChild에 들어가는, 첫번째 Child가 되는 경우
// 2. SetNext()에 의해, Next Node로 지정되는 경우.
//
// 그 이외의 다른 경우에는 RefCount를 증가시키지 않는다.

i3GameNode::i3GameNode()
{
	_Link();
}

i3GameNode::~i3GameNode()
{
	I3ASSERT( m_pParent == nullptr);

	RemoveAllChild();

	_Unlink();
}

void i3GameNode::DeleteStaticMember()
{
	I3_SAFE_RELEASE(s_pGameNodeLinkMutex);
}

static bool _GameNodeTerminateProc( void * pData)
{
	I3_SAFE_RELEASE( s_pGameNodeLinkMutex);

	return true;
}

void i3GameNode::_Link(void)
{
	if( s_pGameNodeLinkMutex == nullptr)
	{
		s_pGameNodeLinkMutex = i3Mutex::new_object();

		i3System::AddTerminateProc( 6, _GameNodeTerminateProc, nullptr);
	}

	///
	s_pGameNodeLinkMutex->Enter();

		m_pGameNodeLinkRight = s_pGameNodeLinkHead;

		if( s_pGameNodeLinkHead == nullptr)
		{
			s_pGameNodeLinkHead = this;
		}
		else
		{
			s_pGameNodeLinkHead->m_pGameNodeLinkLeft = this;
		}

		s_pGameNodeLinkHead = this;

	s_pGameNodeLinkMutex->Leave();
}

void i3GameNode::_Unlink(void)
{
	I3ASSERT( s_pGameNodeLinkMutex != nullptr);

	s_pGameNodeLinkMutex->Enter();

	{
		i3GameNode * pLeft = m_pGameNodeLinkLeft;
		i3GameNode * pRight = m_pGameNodeLinkRight;

		if( pLeft != nullptr)
			pLeft->m_pGameNodeLinkRight = pRight;

		if( pRight != nullptr)
			pRight->m_pGameNodeLinkLeft = pLeft;

		if( this == s_pGameNodeLinkHead)
			s_pGameNodeLinkHead = pRight;

		m_pGameNodeLinkLeft = nullptr;
		m_pGameNodeLinkRight = nullptr;
	}

	s_pGameNodeLinkMutex->Leave();
}

void i3GameNode::Create( i3GameNode * pParent)
{
	m_pParent = pParent;
}

bool	i3GameNode::Traverse( TRAVERSEPROC pUserProc, void * pUserData)
{
	if( pUserProc( this, pUserData) == false)
		return false;

	i3GameNode * pChild = getFirstChild();

	while( pChild != nullptr)
	{
		if( pChild->Traverse( pUserProc, pUserData) == false)
			return false;

		pChild = pChild->getNext();
	}

	return true;
}

void i3GameNode::OnChangeTime( REAL32 tm)
{
	i3GameNode * pChild;

	m_rElapsedTime = tm;

	// 모든 Child에 대해서 OnUpdate를 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; pChild = pChild->getNext() )
	{
		pChild->OnChangeTime( tm);
	}
}


void i3GameNode::ReturnInstance()
{
	I3_MUST_RELEASE(this);

	/*if( m_RefCount <= 0)
	{
		I3PRINTLOG(I3LOG_NOTICE,  "레퍼런스 카운트가 맞지 않습니다. 확인해주세요");
	}*/

	/*
	if( m_RefCount != 1)
	{
		I3PRINTLOG(I3LOG_NOTICE,  "다른 위치에서 사용되고 있는 리소스입니다. %d 확인해 주세요", m_RefCount);
	}

	removeInnerState( I3_GAMENODE_STATE_INPROGRESS);
	*/
	/*
	if( isInnerUsed())
	{
		if( m_RefCount == 2)
		{
			removeInnerState( I3_GAMENODE_STATE_INPROGRESS);
			Release();
		}
		else if( m_RefCount > 2)
		{
			I3PRINTLOG(I3LOG_NOTICE,  "다른 위치에서 사용되고 있는 리소스입니다. %d 확인해 주세요", m_RefCount);
			Release();
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "Invalid Instance");
		}
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE,  "사용된적 없는 리소스입니다. 확인부탁합니다");
	}*/
}

void	i3GameNode::_ForceEnable( bool bFlag)
{
	if( bFlag)
	{
		removeGNodeState( I3_GAMENODE_STATE_DISABLE);
	}
	else
	{
		addGNodeState( I3_GAMENODE_STATE_DISABLE);
	}

	_ForceVisible( bFlag);
}

void	i3GameNode::SetEnable( bool bTrue, REAL32 tm)
{
	if( tm <= 0.0f)
	{
		// 즉시 Enable/Disable 시킨다.
		_ForceEnable( bTrue);
	}
	else
	{
		// 서서히 비활성화됨.
		if( bTrue == false)
			addGNodeState( I3_GAMENODE_STATE_DISABLING);
		else
			addGNodeState( I3_GAMENODE_STATE_ENABLING);
	}

	m_timeFade = m_timeTargetFade = tm;
}

void	i3GameNode::_ForceVisible( bool bFlag)
{
	if( bFlag )
	{
		removeGNodeState( I3_GAMENODE_STATE_INVISIBLE );
	}
	else
	{
		addGNodeState( I3_GAMENODE_STATE_INVISIBLE );
	}

	removeGNodeState( I3_GAMENODE_STATE_INVISIBLING);
}

void	i3GameNode::SetVisible( bool bTrue, REAL32 tm)
{
	if( tm <= 0.0f)
	{
		// 즉시 Visible/Invisible 설정함.
		_ForceVisible( bTrue);
	}
	else
	{
		// 서서히 진행 
		if( bTrue == false)
			addGNodeState( I3_GAMENODE_STATE_INVISIBLING);
		else
			addGNodeState( I3_GAMENODE_STATE_VISIBLING);
	}

	m_timeFade = m_timeTargetFade = tm;
}

void i3GameNode::OnFadeStart(void)
{
}

void i3GameNode::OnFading( REAL32 tm, REAL32 rate)
{
}

void i3GameNode::OnFadeEnd(void)
{
}

void i3GameNode::_DriveFade( REAL32 tm)
{
	if( m_timeFade == m_timeTargetFade)
	{
		OnFadeStart();
	}

	m_timeFade -= tm;

	OnFading( m_timeFade, getFadeRate());

	if( m_timeFade <= 0.0f)
	{
		m_timeTargetFade = 0.0f;
		m_timeFade = 0.0f;
		
		if( isGNodeState( I3_GAMENODE_STATE_DISABLING))
			_ForceEnable(false);
		else if( isGNodeState( I3_GAMENODE_STATE_ENABLING))
			_ForceEnable(true);

		removeGNodeState( I3_GAMENODE_STATE_DISABLING);

		if( isGNodeState( I3_GAMENODE_STATE_INVISIBLING))
			_ForceVisible(false);
		else if( isGNodeState( I3_GAMENODE_STATE_VISIBLING))
			_ForceVisible(true);

		OnFadeEnd();
	}
}

//#define TRACE_GAMENODE_ONUPDATE

void i3GameNode::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GameNode * pChild = nullptr;

	m_rElapsedTime += rDeltaSeconds;

	if( m_timeTargetFade > 0.f )
	{
		_DriveFade( rDeltaSeconds);
	}
	
#ifdef TRACE_GAMENODE_ONUPDATE
	static INT32 s_nGameNodeTraceDepth = 0;
	char szPrefix[1024] = "";
	INT32 i;
	for( i = 0; i < s_nGameNodeTraceDepth; i++ )
	{
		strncat( szPrefix, "    ", sizeof(szPrefix)-1 );
	}
	I3TRACE( "%s[i3GameNode::OnUpdate] this = %p, %s\n", szPrefix, this, this->GetMeta()->GetClassName() );
#endif

	// 모든 Child에 대해서 OnUpdate를 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; pChild = pChild->getNext() )
	{
#ifdef TRACE_GAMENODE_ONUPDATE
		I3TRACE( "%s[i3GameNode::OnUpdate] this = %p\n", szPrefix, pChild );
#endif

		if( pChild->isEnable() && (! pChild->isPaused() ) )
		{
#ifdef TRACE_GAMENODE_ONUPDATE
			I3ASSERT( pChild->GetMeta() != nullptr );
			I3ASSERT( pChild->GetMeta()->GetClassName() != nullptr );
			I3TRACE( "%s[i3GameNode::OnUpdate] Enter(%p) : %s\n", szPrefix, pChild, pChild->GetMeta()->GetClassName() );
			s_nGameNodeTraceDepth++;
#endif
			pChild->OnUpdate( rDeltaSeconds );

#ifdef TRACE_GAMENODE_ONUPDATE
			s_nGameNodeTraceDepth--;
			I3TRACE( "%s[i3GameNode::OnUpdate] Leave(%p) : %s\n", szPrefix, pChild, pChild->GetMeta()->GetClassName() );
#endif
		}
	}
}

void i3GameNode::OnReloadProperty(void)
{
	i3GameNode * pChild;

	// 모든 Child에 대해서 OnUpdate를 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; pChild = pChild->getNext() )
	{
		pChild->OnReloadProperty();
	}
}

#ifdef I3_DEBUG
void i3GameNode::AddChild( i3GameNode *pChild, const char *szFile, INT32 nLine )
{
#ifdef TRACE_GAMENODE_UPDATE
		I3TRACE( "%s (%d) : GAMEOBJ_ADDCHILD( %p )\n", szFile, nLine, pChild );
#endif
	AddChild( pChild );
}
#endif

void i3GameNode::AddChildToHead( i3GameNode * pChild)
{
	I3ASSERT( pChild != nullptr);

	pChild->SetNext( m_pChild);

	I3_REF_CHANGE( m_pChild, pChild);

	m_pChild->setParent( this);
}

#ifdef I3_DEBUG
void i3GameNode::AddChildToHead( i3GameNode *pChild, const char *szFile, INT32 nLine )
{
#ifdef TRACE_GAMENODE_UPDATE
		I3TRACE( "%s (%d) : I3_GAMENODE_ADDCHILDTOHEAD( %p )\n", szFile, nLine, pChild );
#endif

	I3ASSERT( pChild != nullptr);

	pChild->SetNext( m_pChild);

	I3_REF_CHANGE( m_pChild, pChild);

	m_pChild->setParent( this);
}
#endif

void i3GameNode::AddChildToTail( i3GameNode * pChild)
{
	i3GameNode * pNode;

	I3ASSERT( pChild);

	if( m_pChild == nullptr)
	{
		m_pChild = pChild;
		I3_SAFE_ADDREF( m_pChild);
	}
	else
	{
		pNode = m_pChild;

	#if defined( I3_DEBUG)
		I3ASSERT( pNode != pChild);
	#endif

		while( pNode->getNext() != nullptr)
		{
#if defined( I3_DEBUG)
			I3ASSERT( m_pChild != pNode->getNext());
			I3ASSERT( pNode->getNext() != pChild);
#endif
			pNode = pNode->getNext();
		}

		I3ASSERT( pNode != pChild);

		pNode->SetNext( pChild);
	}

	pChild->setParent( this);
}

void i3GameNode::RemoveChild( i3GameNode * pChild)
{
	i3GameNode * pPrev;

	I3ASSERT( pChild != nullptr);
	I3ASSERT( m_pChild != nullptr);
	
	pChild->setParent(nullptr);

	if( m_pChild == pChild)
	{
		m_pChild = pChild->getNext();

		I3_SAFE_ADDREF( m_pChild);
	}
	else
	{
		pPrev = m_pChild;

		while((pPrev != nullptr) && (pPrev->getNext() != pChild))
			pPrev = pPrev->getNext();

		if( pPrev == nullptr)
			return;

		I3_MUST_ADDREF( pChild);

		pPrev->SetNext( pChild->getNext());
	}

	pChild->SetNext(nullptr);
	I3_MUST_RELEASE( pChild);
}

void i3GameNode::RemoveAllChild(void)
{	
	i3GameNode * pChild, * pNextChild;

	// 모든 Child에 대해서 OnUpdate를 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; )
	{
#if 0
		I3ASSERT( pChild->GetMeta() != nullptr );
		I3ASSERT( pChild->GetMeta()->GetClassName() != nullptr );
		I3TRACE( "[i3GameNode::RemoveAllChild] (%p) : %s\n", pChild, pChild->GetMeta()->GetClassName() );
#endif

		pNextChild = pChild->getNext();

		pChild->setParent(nullptr);

		I3_SAFE_RELEASE( pChild );
		pChild = pNextChild;
	}

	m_pChild = nullptr;
}
/*
void i3GameNode::DetachChild( i3GameNode * pChild)
{
	i3GameNode * pPrev;

	I3ASSERT( pChild);

	pChild->setParent( nullptr);

	if( m_pChild == pChild)
	{
		m_pChild = pChild->getNext();
	}
	else
	{
		pPrev = m_pChild;

		while((pPrev != nullptr) && (pPrev->getNext() != pChild))
			pPrev = pPrev->getNext();

		if( pPrev == nullptr)
			return;

		pChild->AddRef();

		pPrev->SetNext( pChild->getNext());

		pChild->SetNext( nullptr);

		pChild->Release();
	}
}
*/
// Index로 Child 얻기.
// 모든 자식 노드를 검색해야 되는 경우라면 이 함수를 쓰는 것은 아주 느릴 수 있음.
// 모든 자식 노드를 검색해야 될 경우엔 다음과 같이 사용하기 바람.
//
// for( i3GameNode *pChild = pNode->GetFirstChild(); pChild != nullptr; pChild = pChild->GetNext() )
//
i3GameNode * i3GameNode::GetChildByIndex( INT32 nIndex )
{
	INT32 i;
	i3GameNode *pChild = getFirstChild();
	I3ASSERT( pChild != nullptr );

	for( i = 0; i < nIndex; i++ )
	{
		pChild = pChild->getNext();
		I3ASSERT( pChild != nullptr );
	}

	return pChild;
}

i3GameNode * i3GameNode::FindChildByName( const char * pszName, bool bRecurs)
{
	i3GameNode * pNode = m_pChild;
	i3GameNode * pResult;

	while( pNode != nullptr)
	{
		if( pNode->hasName())
		{
			if( i3::generic_is_iequal( pNode->GetNameString(), pszName) )
				return pNode;
		}

		if( bRecurs)
		{
			pResult = pNode->FindChildByName( pszName, true);

			if( pResult != nullptr)
				return pResult;
		}

		pNode = pNode->getNext();
	}

	return nullptr;
}

i3GameNode * i3GameNode::FindChildByType( const i3::class_meta * pMeta, bool bRecursive)
{
	i3GameNode * pNode = m_pChild;
	i3GameNode * pResult;

	while( pNode != nullptr)
	{
		if( i3::kind_of( pNode, pMeta))
			return pNode;

		if( bRecursive)
		{
			pResult = pNode->FindChildByType( pMeta, true);

			if( pResult != nullptr)
				return pResult;
		}

		pNode = pNode->getNext();
	}

	return nullptr;
}

INT32	i3GameNode::GetChildIndex( i3GameNode * pChild)
{
	i3GameNode * pTemp = m_pChild;
	INT32 i = 0;

	while( pTemp != nullptr)
	{
		if( pTemp == pChild)
			return i;

		i++;
		pTemp = pTemp->getNext();
	}

	return -1;
}

INT32 i3GameNode::GetChildCount(void)
{
	i3GameNode * pTemp = m_pChild;
	INT32 count = 0;

	while( pTemp != nullptr)
	{
		count++;
		pTemp = pTemp->getNext();
	}

	return count;
}

void i3GameNode::ChangeParent( i3GameNode * pParent)
{
	I3_MUST_ADDREF( this);

	this->getParent()->RemoveChild( this);
#if defined(I3_DEBUG)
	pParent->AddChild( this, __FILE__, __LINE__);
#else
	pParent->AddChild( this);
#endif

	I3_MUST_RELEASE( this);
}

void i3GameNode::SetNext( i3GameNode * pNext)
{
	I3_REF_CHANGE( m_pNext, pNext);
}

//	현재 자신을 기준으로 Node들을 그룹화. pNode와 관계된 Node들만 활성화하고 그 이외에는 모두 비활성화 시킨다.
bool i3GameNode::setGroupEnable( i3GameNode * pNode)
{
	bool ret = false;	

	if( this == pNode)
	{
		pNode->SetEnable(true);

		ret = true;
	}
	else
	{
		i3GameNode * pCurNode = m_pChild;
		while( pCurNode != nullptr)
		{
			if( pCurNode->setGroupEnable( pNode) == true)		ret = true;

			pCurNode = pCurNode->getNext();		
		}
	}

	if( ret)
		this->SetEnable( true);		
	else
		this->SetEnable( false);

	return ret;
}

void i3GameNode::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method );

	// Why 예외처리?
	if( !i3::kind_of<i3GameNode* >(pDest))
		return ;

	i3GameNode * pNode = (i3GameNode*)pDest;

	pNode->setGNodeStyle( m_GameNodeStyle );
	pNode->setGNodeState( m_GameNodeState );

	pNode->setUpdateInterval( m_UpdateInterval );
}

void	i3GameNode::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
	{
		// Parent에 대한 Reference는 추가하지 않는다.
		i3EventReceiver::OnBuildObjectList( List);

		if( m_pChild != nullptr )
		{
			m_pChild->OnBuildObjectList( List );
		}
	}

	if( m_pNext != nullptr)
	{
		m_pNext->OnBuildObjectList( List);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 GAME_NODE
	{
		UINT8		m_ID[4] = { 'G', 'N', 'D', '1' };
		UINT32		m_GameNodeStyle = 0;
		UINT32		m_State = 0;
		OBJREF		m_pParent = 0;

		REAL32		m_UpdateInterval = 0.0f;
		//	REAL32		m_LifeTime;
		INT32		reserved_INT32 = 0;
		UINT8		reserved[8] = { 0 };
	};

	struct ALIGN4 GAME_NODE_OLD
	{
		UINT32		m_State = 0;
		OBJREF		m_pParent = 0;
		//OBJREF		m_pNext;
		REAL32		m_UpdateInterval = 0.0f;
		UINT8		reserved[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GameNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAME_NODE data;

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	
	data.m_State = m_GameNodeState;
	data.m_pParent = (OBJREF) pResFile->GetObjectPersistID( m_pParent);

	data.m_UpdateInterval = m_UpdateInterval;

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GameNode::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Rc;
}

UINT32	i3GameNode::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAME_NODE data;

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	{
		UINT8 id[4];

		Rc = pStream->Read( id, 4);
		I3_CHKIO( Rc);

		pStream->SetPosition( -4, STREAM_CURRENT);

		if( memcmp( id, "GND1", 4) != 0)
		{
			pack::GAME_NODE_OLD oldData;

			Rc = pStream->Read( &oldData, sizeof(oldData));
			I3_CHKIO( Rc);
			Result += Rc;

			data.m_State = oldData.m_State;
			data.m_GameNodeStyle = 0;
			data.m_UpdateInterval = oldData.m_UpdateInterval;
			data.m_pParent = oldData.m_pParent;
		}
		else
		{
			Rc = pStream->Read( &data, sizeof(data));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3GameNode::OnLoad()", "Could not read data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	setGNodeState( data.m_State & I3_GAMENODE_STATE_DISABLE);
	
	// Parent
	if( data.m_pParent != 0)
	{
		i3PersistantElement * pObj = pResFile->FindObjectByID( data.m_pParent);

		if( pObj != nullptr)
		{
			if( i3::kind_of<i3GameNode*>(pObj))
			{
				i3GameNode * pParent = (i3GameNode *) pObj;

				if( pParent != nullptr)
					pParent->AddChildToTail( this);
			}
		}
	}

	setUpdateInterval(data.m_UpdateInterval);

	return Result;
}

bool i3GameNode::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3NamedElement::OnSaveXML( pFile, pXML);
	OBJREF ref;

	pXML->addAttr( "GameNodeState", m_GameNodeState);

	ref = pFile->GetObjectPersistID( m_pParent);
	pXML->addAttr( "ParentNode", ref);

	pXML->addAttr( "UpdateInterval", m_UpdateInterval);

	return bResult;
}

bool i3GameNode::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3NamedElement::OnLoadXML( pFile, pXML);
	OBJREF ref;

	pXML->getAttr( "GameNodeState", &m_GameNodeState);
	setGNodeState( m_GameNodeState & I3_GAMENODE_STATE_DISABLE);

	pXML->getAttr( "ParentNode", &ref);
	if( ref != 0)
	{
		i3PersistantElement * pObj = pFile->FindObjectByID( ref);

		if( i3::kind_of<i3GameNode*>(pObj))
		{
			i3GameNode * pParent = (i3GameNode *) pObj;

			if( pParent != nullptr)
				pParent->AddChildToTail( this);
		}
	}

	pXML->getAttr( "UpdateInterval", &m_UpdateInterval);
	setUpdateInterval( m_UpdateInterval);

	return bResult;
}

static i3GameNode * _Rec_CreateClone( i3GameNode * pSrc, i3GameNode * pDest, I3_COPY_METHOD method)
{
	i3GameNode * pNew, * pChild;
//	i3ClassMeta * pMeta;
//	List		ChildList;			// 쓰이지 않아 주석..

	pChild = pSrc->getFirstChild();

	while( pChild != nullptr)
	{
		if( ( pChild->getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) == 0)
		{
//			pMeta = pChild->GetMeta();

			pNew = pChild->create_instance(); // (i3GameNode *) CREATEINSTANCE( pMeta);
			pChild->CopyTo( pNew, method);

			pDest->AddChildToTail( pNew);

			_Rec_CreateClone( pChild, pNew, method);
		}

		pChild = pChild->getNext();
	}

	return pDest;
}

static void _InitAfterLoadProc( i3GameNode * pNode)
{
	i3GameNode * pChild;

	pNode->OnInitAfterLoad();

	pChild = pNode->getFirstChild();
	while( pChild != nullptr)
	{
		_InitAfterLoadProc( pChild);

		pChild = pChild->getNext();
	}
}

i3GameNode * i3GameNode::CreateClone( i3GameNode * pRoot, I3_COPY_METHOD method)
{
	i3GameNode * pNew;
//	i3ClassMeta * pMeta;

//	pMeta = pRoot->GetMeta();
	pNew = pRoot->create_instance(); //(i3GameNode *) CREATEINSTANCE( pMeta);
	pRoot->CopyTo( pNew, method);

	_Rec_CreateClone( pRoot, pNew, method);

	_InitAfterLoadProc( pNew);

	return pNew;
}

void i3GameNode::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	if( m_pChild != nullptr)
	{
		m_pChild->OnInstancing( pInfo);
	}

	if( m_pNext != nullptr)
	{
		m_pNext->OnInstancing( pInfo);
	}
}

void i3GameNode::Instantiate( void)
{	
	i3MatrixStack mtxStack;
	I3_ONINSTANCING_INFO info;
	info._pBase			= this;
	info._pMatrixStack	= &mtxStack;

	OnInstancing( &info);
}

bool i3GameNode::OnLostDevice( bool bLostDevice)
{
	if( isGNodeState( I3_GAMENODE_STATE_LOSTDEVICE) == true)
		return false;	// 이미 호출된 상태

	addGNodeState( I3_GAMENODE_STATE_LOSTDEVICE);
	return true;
}

bool i3GameNode::OnRevive( i3RenderContext * pCtx)
{
	if( isGNodeState( I3_GAMENODE_STATE_LOSTDEVICE) == false)
		return false;	// 이미 호출된 상태

	removeGNodeState( I3_GAMENODE_STATE_LOSTDEVICE);
	return true;
}


void i3GameNode::CallLostDevice( bool bLostDevice)
{
	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Enter();

	i3GameNode * pTemp = s_pGameNodeLinkHead;
	INT32 cnt = 0;

	while( pTemp != nullptr)
	{
		pTemp->OnLostDevice( bLostDevice);
		//I3ASSERT(resLostDevice);

		cnt++;

		pTemp = pTemp->m_pGameNodeLinkRight;
	}

	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Leave();

	I3TRACE( "i3GameNode::CallLostDevice() : total %d game nodes\n", cnt);
}

void i3GameNode::CallRevive( i3RenderContext * pCtx)
{
	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Enter();

	i3GameNode * pTemp = s_pGameNodeLinkHead;
	INT32 cnt = 0;

	while( pTemp != nullptr)
	{
		pTemp->OnRevive( pCtx);

		cnt++;

		pTemp = pTemp->m_pGameNodeLinkRight;
	}

	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Leave();

	I3TRACE( "i3GameNode::CallRevive() : total %d game nodes\n", cnt);
}

void i3GameNode::CallRecreate( void)
{
	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Enter();

	i3GameNode * pTemp = s_pGameNodeLinkHead;
	INT32 cnt = 0;

	while( pTemp != nullptr)
	{
		pTemp->OnRecreate();

		cnt++;

		pTemp = pTemp->m_pGameNodeLinkRight;
	}

	if( s_pGameNodeLinkMutex != nullptr)
		s_pGameNodeLinkMutex->Leave();
}

void _RecusiveChild( i3GameNode * pNode, INT32 nCurDepth)
{
	for(INT32 i=0; i< nCurDepth; i++)
		I3TRACE( "- ");

	I3TRACE( " (%s) RefCount:%d\n", pNode->GetName(), pNode->GetRefCount() );

	i3GameNode * pChild = pNode->getFirstChild();
	
	while( pChild)
	{
		_RecusiveChild( pChild, nCurDepth + 1);

		pChild = pChild->getNext();
	}
}

void i3GameNode::Dump(void)
{
	_RecusiveChild( this, 0);
}
