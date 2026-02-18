#include "i3CommonType.h"
#include "i3TreeNode.h"
#include "i3String.h"
#include "i3XMLFile.h"
#include "itl/vector_util.h"
#include "string/compare/generic_is_niequal.h"
#include "string/compare/generic_is_iequal.h"
#include "string/compare/generic_is_equal.h"

I3_CLASS_INSTANCE( i3TreeNode);

i3TreeNode::~i3TreeNode(void)
{
	RemoveAllChild();
	SetParent(nullptr);
}

void i3TreeNode::AddChild( i3TreeNode * pChild)
{
	I3_MUST_ADDREF( pChild);
	m_ChildList.push_back( pChild);
	pChild->SetParent( this);
}

void i3TreeNode::InsertChild( INT32 idx, i3TreeNode * pNode)
{
	I3_MUST_ADDREF( pNode);
	m_ChildList.insert(m_ChildList.begin() + size_t(idx), pNode);
	pNode->SetParent( this);
}

void i3TreeNode::RemoveChild( i3TreeNode * pChild)
{
//	if( m_ChildList.Remove( pChild) != -1)
	if ( i3::vu::remove(m_ChildList, pChild) )
	{
		pChild->SetParent(nullptr);
		I3_SAFE_RELEASE( pChild);
	}
}

void i3TreeNode::RemoveAllChild(void)
{
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3TreeNode * pNode = m_ChildList[i];
		pNode->SetParent(nullptr);
		I3_SAFE_RELEASE( pNode);
	}

	m_ChildList.clear();
}

void i3TreeNode::RemoveFromParent(void)
{
	if( m_pParent != nullptr)
	{
		m_pParent->RemoveChild( this);
	}
}

void i3TreeNode::DetachChild( i3TreeNode * pChild)
{
	i3::vu::remove(m_ChildList, pChild);
}

bool i3TreeNode::IsChild( i3TreeNode * pChild)
{
	return i3::vu::index_of(m_ChildList, pChild) != m_ChildList.size(); 
}

void i3TreeNode::SetParent( i3TreeNode * pParent)
{
	m_pParent = pParent;
}

i3TreeNode * i3TreeNode::FindChildByName( const char * pszName)
{
	INT32 i;
	i3TreeNode * pChild;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = getChild( i);

		if( pChild->hasName() && i3::generic_is_iequal( pChild->GetNameString(), pszName) )
		{
			return pChild;
		}
	}

	return nullptr;
}

void i3TreeNode::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3TreeNode * pNode;

	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == false)
		{
			pNode->OnBuildObjectList( List);
		}
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TREENODE
	{
		INT8			m_ID[4] = { 'T', 'R', 'N', '3' };
		UINT32			m_Style = 0;
		UINT32			m_State = 0;
		INT32			m_nChildCount = 0;
		UINT32			pad[15] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TreeNode::OnSave( i3ResourceFile * pResFile)
{
	pack::TREENODE data;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	// Named Element
	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Header
	data.m_Style = getNodeStyle();
	data.m_State = getNodeState();

	i3TreeNode * pNode;

	//Valid ChildCount
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == false)
		{
			++data.m_nChildCount;
		}
	}

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write Node Information Header.");
		return STREAM_ERR;
	}
	Result += Rc;

	//Save Child
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == false)
		{
			OBJREF ref = (OBJREF)pResFile->GetObjectPersistID( pNode);

			Rc = pStream->Write( &ref, sizeof( OBJREF));
			I3_CHKIO( Rc);
			Result += Rc;
		}
	}

	return Result;
} 

UINT32	i3TreeNode::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	OBJREF	ref;
	i3ElementBase * pObj;

	pack::TREENODE data;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	// Named Element
	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Header
	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read Node information head.");
		return STREAM_ERR;
	}
	Result += Rc;

	setNodeStyle( data.m_Style);
	setNodeState( data.m_State);

//	if( i3String::NCompare((char*)data.m_ID, "TRN2", 4) == 0)
	if ( i3::generic_is_niequal((const char*)data.m_ID, "TRN2", 4) )
	{
		Rc = LoadFromResourceFileToList(m_ChildList, pResFile, true);
			//m_ChildList.LoadFrom( pResFile, true);
		I3_CHKIO( Rc);
		Result += Rc;

		for( i = 0; i < getChildCount(); i++)
		{
			i3TreeNode * pChild = getChild( i);

			pChild->SetParent( this);
		}
	}
	else
	{
		if( data.m_nChildCount > 0)
		{
			for( i = 0; i < data.m_nChildCount ; ++i)
			{
				Rc = pStream->Read( &ref, sizeof( ref));
				I3_CHKIO( Rc);
				Result += Rc;

				pObj = (i3ElementBase *) pResFile->FindObjectByID( ref);

				I3ASSERT( i3::kind_of(pObj, static_meta() ) );

				AddChild((i3TreeNode*) pObj);
			}
		}
	}

	return Result;
}

bool i3TreeNode::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 ChildCount = 0;
	char conv[MAX_PATH];
	i3TreeNode * pNode;

	pXML->addAttr( "TreeStyle", getNodeStyle());
	pXML->addAttr( "TreeState", getNodeState());

	//Valid ChildCount
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == false)
		{
			i3::snprintf( conv, MAX_PATH -1, "Child_%d", ChildCount);

			pXML->addAttr( conv, (INT32)pFile->GetObjectPersistID( (i3PersistantElement *)pNode));

			++ChildCount;
		}
	}

	pXML->addAttr( "ChildCount", ChildCount);

	return true;
}

bool i3TreeNode::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 i, iVal, ChildCount;
	char conv[MAX_PATH];

	pXML->getAttr( "TreeStyle", &m_NodeStyle);
	pXML->getAttr( "TreeState", &m_NodeState);
	pXML->getAttr( "ChildCount", &ChildCount);

	for( i = 0; i < ChildCount; i++)
	{
		i3::snprintf( conv, MAX_PATH -1, "Child_%d", i);

		pXML->getAttr( conv, &iVal);
		I3ASSERT( iVal > 0);

			i3TreeNode * pNode = (i3TreeNode*)pFile->FindObjectByID( iVal);
			I3ASSERT( pNode != nullptr);

			AddChild( pNode);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
i3TreeNode * i3TreeNode::FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	INT32 i;
	i3TreeNode * pResult;

	if( pRoot->kind_of( pMeta) )
		return pRoot;

	if( bRecursive)
	{
		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByType( pRoot->getChild( i), pMeta, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3TreeNode::FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3::vector<i3TreeNode*>& List, bool bRecursive)
{
	if( pRoot->kind_of( pMeta))
		List.push_back( pRoot);

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByType( pRoot->getChild( i), pMeta, List, true);
		}
	}
}

i3TreeNode * i3TreeNode::FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	if( pRoot->same_of( pMeta))
		return pRoot;

	if( bRecursive)
	{
		INT32 i;
		i3TreeNode * pResult;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByExactType( pRoot->getChild( i), pMeta, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3TreeNode::FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3::vector<i3TreeNode*>& List, bool bRecursive)
{
	if( pRoot->same_of( pMeta))
		List.push_back( pRoot);

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByExactType( pRoot->getChild(i), pMeta, List, true);
		}
	}
}

i3TreeNode * i3TreeNode::FindNodeByName( i3TreeNode * pRoot, const char * pszName, bool bRecursive)
{
	if( pRoot->hasName())
	{
		if( i3::generic_is_iequal( pRoot->GetNameString(), (char *) pszName) )
			return pRoot;
	}

	if( bRecursive)
	{
		INT32 i;
		i3TreeNode * pResult;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByName( pRoot->getChild(i), pszName, true);
			if( pResult != nullptr)
				return pResult;
		}
	}

	return nullptr;
}

void i3TreeNode::FindNodeByName( i3TreeNode * pRoot, const char * pszName, i3::vector<i3TreeNode*>& List, bool bRecursive)
{
	if( pRoot->hasName())
	{
		if( i3::generic_is_equal( pRoot->GetNameString(), (char *) pszName) )		// 대소문자 가리는 것으로 변경..
		{
			List.push_back( pRoot);
		}
	}

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByName( pRoot->getChild(i), pszName, List, true);
		}
	}
}

static i3TreeNode * _Rec_CreateClone( i3TreeNode * pSrc, i3TreeNode * pDest, I3_COPY_METHOD method)
{
	INT32 i, Count, idx;
	i3TreeNode * pNew, * pChild;
//	i3ClassMeta * pMeta;
//	i3::vector<i3TreeNode*>	ChildList;

	for( i = 0, Count = 0; i < pSrc->getChildCount(); i++)
	{
		pChild = pSrc->getChild( i);

		if( pChild->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == false)
			Count++;
	}

	if( Count > 0)
	{
		i3::vector<i3TreeNode*>& List = pDest->getChildList();
		List.resize( Count);
	}

	for( i = 0, idx = 0; i < pSrc->getChildCount(); i++)
	{
		pChild = pSrc->getChild( i);

		if( pChild->isNodeStyle( I3_TREENODE_STYLE_VOLATILE))
			continue;

		pNew = pChild->create_instance();
		I3_MUST_ADDREF( pNew);

		pChild->CopyTo( pNew, method);

		pDest->setChild( idx, pNew);
		pNew->SetParent( pDest);
		idx++;

		_Rec_CreateClone( pChild, pNew, method);
	}

	return pDest;
}


i3TreeNode * i3TreeNode::CreateClone( i3TreeNode * pRoot, I3_COPY_METHOD method)
{
	i3TreeNode * pNew;
//	i3ClassMeta * pMeta;
//	pMeta = pRoot->GetMeta();
	pNew = pRoot->create_instance(); //(i3TreeNode *) CREATEINSTANCE( pMeta);
	pRoot->CopyTo( pNew, method);

	_Rec_CreateClone( pRoot, pNew, method);

	return pNew;
}

static bool	_Rec_Traverse( INT32 depth, i3TreeNode * pNode, I3_TREE_TRAVERSE_PROC pProc, void * pUserData)
{
	INT32 i;
	i3TreeNode * pChild;

	if( pProc != nullptr)
	{
		if( pProc( depth, pNode, pUserData) == false)
			return false;
	}

	for( i = 0; i < pNode->getChildCount(); i++)
	{
		pChild = pNode->getChild( i);

		if( _Rec_Traverse( depth + 1, pChild, pProc, pUserData) == false)
			return false;
	}

	return true;
}

bool	i3TreeNode::Traverse( i3TreeNode * pRoot, I3_TREE_TRAVERSE_PROC pProc, void * pUserData)
{
	return _Rec_Traverse( 0, pRoot, pProc, pUserData);
}
