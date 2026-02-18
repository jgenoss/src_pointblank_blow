#include "i3FrameworkPCH.h"
#include "i3GameObjBase.h"
#include "i3Framework.h"
#include "i3GameSceneInfo.h"
#include "i3GameObjRef.h"

#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE( i3GameObjBase);


i3GameObjBase::~i3GameObjBase(void)
{
	I3_SAFE_NODE_RELEASE( m_pSceneNode);

	removeAllRefObject();

	
	BindSgRes( nullptr, nullptr);
}

bool i3GameObjBase::Create( i3Node * pParentNode, bool bEnable)
{
	I3ASSERT( m_pSceneNode != nullptr);

	pParentNode->AddChild( m_pSceneNode);

	SetEnable( bEnable);

	return true;
}

void i3GameObjBase::SetInstanceSg( i3Node * pNode)
{
	I3_REF_CHANGE( m_pSceneNode, pNode);
}

void i3GameObjBase::_ForceEnable( bool bTrue)
{
	i3GameNode::_ForceEnable( bTrue );

	if( bTrue)	EnableScene();
	else		DisableScene();
}

void i3GameObjBase::_ForceVisible( bool bTrue)
{ 
	i3GameNode::_ForceVisible( bTrue);

	if( bTrue )		VisibleScene();
	else			InvisibleScene();
}

void i3GameObjBase::VisibleScene(void)
{
	if( m_pSceneNode != nullptr)
		m_pSceneNode->RemoveFlag( I3_NODEFLAG_INVISIBLE);
}

void i3GameObjBase::InvisibleScene(void)
{
	if( m_pSceneNode != nullptr)
		m_pSceneNode->AddFlag( I3_NODEFLAG_INVISIBLE);
}

void i3GameObjBase::EnableScene(void)
{ 
	if( m_pSceneNode != nullptr)
		m_pSceneNode->ActivateNode();
}

void i3GameObjBase::DisableScene(void)
{ 
	if( m_pSceneNode != nullptr)
		m_pSceneNode->DeactivateNode();
}

void i3GameObjBase::SetNode( i3Node * pNode)
{
	I3_REF_CHANGE( m_pSceneNode, pNode);
}

void i3GameObjBase::BindSgRes( i3Framework * pFramework, i3GameResSceneGraph * pRes, bool bInstancing, bool bClone)
{
	/////////////////////////////////
	if( m_pGameRes != nullptr)
	{
		m_pGameRes->RemoveInstance( this);
		I3_MUST_RELEASE( m_pGameRes); 
	}

	if( m_pSceneNode != nullptr)
	{
		if( m_pSceneNode->GetParent() != nullptr)
		{
			m_pSceneNode->RemoveFromParent();
			removeGNodeState( I3_GAMENODE_STATE_ATTACHED);
		}

		/*#if defined( I3_DEBUG)
		if((pRes != nullptr) && (pRes->hasName()))
		{
			m_pSceneNode->SetName( pRes->GetName());
		}
		#endif*/
	}

	/////////////////////////////////
	m_pGameRes = pRes;

	if( m_pGameRes != nullptr)
	{
		I3_MUST_ADDREF( m_pGameRes);

		// 만약 GameRes가 처음으로 사용된다면, Sg에 우선 연결해야 한다.
		m_pGameRes->Bind( pFramework, this, bInstancing, bClone);
	}

	//if (m_pGameRes->getCommonScene())
		OnBindResource();
}

void i3GameObjBase::InitColliderSet( INT32 GrpIdx, UINT32 grpMask, i3Node * pRoot )
{
	if( pRoot == nullptr)
	{
		pRoot = m_pSceneNode;
	}

	i3Scene::InitColliderSet( pRoot, GrpIdx, grpMask, 0, this);
}

void i3GameObjBase::AttachScene(void)
{
	if( IsAttachedScene())
		return;

	if (m_pGameRes)
	{
		i3Node * pNode = m_pGameRes->getAttachRoot();
		I3ASSERT( pNode != nullptr);

		addGNodeState( I3_GAMENODE_STATE_ATTACHED);

		pNode->AddChild( m_pSceneNode);
	}
}

void i3GameObjBase::DetachScene(void)
{
	if( ! IsAttachedScene())
		return;

	if (m_pGameRes)
	{
		i3Node * pNode = m_pGameRes->getAttachRoot();
		I3ASSERT( pNode != nullptr);

		removeGNodeState( I3_GAMENODE_STATE_ATTACHED);

		pNode->RemoveChild( m_pSceneNode);
	}
}

void i3GameObjBase::addRefObject( i3ResourceObject * pObj)
{
	I3_MUST_ADDREF( pObj);
	m_RefClassList.push_back( pObj);
}

void i3GameObjBase::removeRefObject( i3ResourceObject * pObj)
{
	
	I3ASSERT( pObj != nullptr);

	INT32 idx = i3::vu::index_of(m_RefClassList, pObj);
		//m_pRefClassList->IndexOf( pObj);
	I3ASSERT( idx != (INT32)m_RefClassList.size() );

	I3_MUST_RELEASE( pObj);

	m_RefClassList.erase( m_RefClassList.begin() + idx);
}

void i3GameObjBase::removeAllRefObject(void)
{
	i3ResourceObject * pObj;

	for(size_t i = 0; i < m_RefClassList.size(); i++)
	{
		pObj = m_RefClassList[i];
		I3_MUST_RELEASE( pObj);
	}

	m_RefClassList.clear();
}

bool i3GameObjBase::isRefObject( i3ResourceObject * pObj)
{
	size_t idx = i3::vu::index_of(m_RefClassList, pObj); 

	return (idx != m_RefClassList.size());
}

i3ResourceObject * i3GameObjBase::FindRefObjectByName( const char * pszName)
{
	char szName[MAX_PATH];

	for(size_t i = 0; i < m_RefClassList.size(); i++)
	{
		i3ResourceObject * pObj = m_RefClassList[i];

		i3::extract_filetitle( pObj->GetNameString(), szName);
	
		if( i3::generic_is_iequal( pszName, szName) )
			return pObj;
	}

	return nullptr;
}

void i3GameObjBase::OnBindResource(void)
{
}

void i3GameObjBase::OnChangeTime( REAL32 tm)
{
	if( m_pSceneNode != nullptr)
	{
		m_pSceneNode->OnChangeTime( tm);
	}

	i3GameNode::OnChangeTime( tm);
}

void i3GameObjBase::OnInitAfterLoad(void)
{
	if( m_pSceneNode != nullptr)
	{
		m_pSceneNode->OnInitAfterLoad();
	}

	i3GameNode::OnInitAfterLoad();
}

void i3GameObjBase::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GameNode::CopyTo( pDest, method );

	if( !i3::kind_of<i3GameObjBase*>(pDest))
	{
		return ;
	}

	i3GameObjBase * pObj = (i3GameObjBase*)pDest;

	pObj->SetType( m_Type );

	if( m_pGameRes != nullptr )
	{
		// i3Framework로 nullptr은 허용되지 않으나, 실제로 i3Framework가 필요할 가능성이 없기 때문에...
		pObj->BindSgRes( nullptr, getSgRes());
	}

	pObj->setNetworkSyncMode( m_NetworkSyncMode);

	// Referenced Objects
	
	for( size_t i = 0; i < m_RefClassList.size(); i++)
	{
		i3ResourceObject * pTemp = m_RefClassList[i];
		pObj->addRefObject( pTemp);
	}
	
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 GAME_OBJ
	{
		UINT32			m_Type = 0;
		OBJREF			m_pGameRes = 0;
		UINT8			m_NetworkSyncMode = 0;
		UINT8			pad[3] = { 0 };
		UINT32			m_RefObjCount = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void i3GameObjBase::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pGameRes != nullptr)
		m_pGameRes->OnBuildObjectList( List);


	for( size_t i = 0; i < m_RefClassList.size(); i++)
	{
		i3PersistantElement * pObj = m_RefClassList[i];

		pObj->OnBuildObjectList( List);
	}


	i3GameNode::OnBuildObjectList( List);
}

UINT32 i3GameObjBase::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();	

	Result = i3GameNode::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::GAME_OBJ data;
	data.m_pGameRes = (OBJREF) pResFile->GetObjectPersistID( m_pGameRes);
	data.m_Type = m_Type;
	data.m_NetworkSyncMode  = (UINT8)m_NetworkSyncMode;
	
	data.m_RefObjCount = m_RefClassList.size(); 
	
	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GameObjBase::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;


	// 관련 Object의 Reference 저장.
	for( size_t i = 0; i < m_RefClassList.size(); i++)
	{
		OBJREF ref = pResFile->GetObjectPersistID( getRefObject( i));

		Rc = pResFile->Write( &ref, sizeof(ref));
		I3_CHKIO( Rc);
		Result += Rc;
	}


	return Result;
}

UINT32 i3GameObjBase::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAME_OBJ data;

	Result = i3GameNode::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GameObjBase::OnLoad()", "Could not load data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Game Resource
	if( data.m_pGameRes != 0)
	{
		i3GameResSceneGraph * pRes = (i3GameResSceneGraph *) pResFile->FindObjectByID( data.m_pGameRes);
		I3ASSERT( pRes);

		BindSgRes( i3Framework::getCurrentFramework(), pRes);
	}

	SetType( data.m_Type);

	setNetworkSyncMode( (I3_NETWORK_GAMEOBJECT_TYPE) data.m_NetworkSyncMode);

	if( data.m_RefObjCount > 0)
	{
		// 관련 Object가 있다. 읽어 들이도록...
		for( UINT32 i = 0; i < data.m_RefObjCount; i++)
		{
			OBJREF ref;

			Rc = pResFile->Read( &ref, sizeof(ref));
			I3_CHKIO( Rc);
			Result += Rc;

			i3ResourceObject * pObj = (i3ResourceObject *) pResFile->FindObjectByID( ref);

			if( pObj != nullptr)
			{
				addRefObject( pObj);
			}
			else
			{
				I3TRACE( "%x Referenced Object is not found!!!!\n", ref);
			}
		}
	}

	return Result;
}


void i3GameObjBase::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	if( m_pSceneNode != nullptr)
	{
		m_pSceneNode->OnInstancing( pInfo);
	}

	i3GameNode::OnInstancing( pInfo);
}

UINT32 i3GameObjBase::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void ** ppData)
{
	return 0;
}

UINT32 i3GameObjBase::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	return 0;
}
