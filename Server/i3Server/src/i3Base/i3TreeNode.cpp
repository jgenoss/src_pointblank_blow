#include "i3CommonType.h"
#include "i3TreeNode.h"
#include "i3String.h"
#include "i3XMLFile.h"

I3_CLASS_INSTANCE( i3TreeNode, i3NamedElement);

i3TreeNode::i3TreeNode(void)
{
	m_NodeStyle	= 0;
	m_NodeState	= 0;
	m_pParent	= NULL;
}

i3TreeNode::~i3TreeNode(void)
{
	RemoveAllChild();
	SetParent( NULL);
}

void i3TreeNode::AddChild( i3TreeNode * pChild)
{
	m_ChildList.Add( pChild);

	pChild->AddRef();

	pChild->SetParent( this);
}

void i3TreeNode::InsertChild( INT32 idx, i3TreeNode * pNode)
{
	m_ChildList.Insert( idx, pNode);

	pNode->AddRef();
	pNode->SetParent( this);
}

void i3TreeNode::RemoveChild( i3TreeNode * pChild)
{
	if( m_ChildList.Remove( pChild) != -1)
	{
		pChild->SetParent( NULL);
		pChild->Release();
	}
}

void i3TreeNode::RemoveAllChild(void)
{
	INT32 i;

	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		i3TreeNode * pNode;

		pNode = (i3TreeNode *) m_ChildList.Items[i];

		pNode->SetParent( NULL);
		pNode->Release();
	}

	m_ChildList.Clear();
}

void i3TreeNode::RemoveFromParent(void)
{
	if( m_pParent != NULL)
	{
		m_pParent->RemoveChild( this);
	}
}

void i3TreeNode::DetachChild( i3TreeNode * pChild)
{
	m_ChildList.Remove( pChild);
}

BOOL i3TreeNode::IsChild( i3TreeNode * pChild)
{
	return m_ChildList.IndexOf( pChild) != -1;
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

		if( pChild->hasName() && (i3String::Compare( pChild->GetName(), pszName) == 0))
		{
			return pChild;
		}
	}

	return NULL;
}

void i3TreeNode::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	I3ASSERT( pDest->IsTypeOf( i3TreeNode::GetClassMeta()));

	//i3TreeNode * pNode = (i3TreeNode *) pDest;
}

void i3TreeNode::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3TreeNode * pNode;

	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == FALSE)
		{
			pNode->OnBuildObjectList( List);
		}
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagi3NodePersistHeader2_0
{
	INT8			m_ID[4];
	UINT32			m_Style;
	UINT32			m_State;
	INT32			m_nChildCount;
	UINT32			pad[15];
} I3_PERSIST_TREENODE;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TreeNode::OnSave( i3ResourceFile * pResFile)
{
	I3_PERSIST_TREENODE data;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	// Named Element
	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Header
	i3mem::Fill( &data, 0, sizeof(data));
	i3mem::Copy( data.m_ID, "TRN3", 4);
	data.m_Style = getNodeStyle();
	data.m_State = getNodeState();

	INT32 i;
	i3TreeNode * pNode;

	//Valid ChildCount
	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == FALSE)
		{
			++data.m_nChildCount;
		}
	}

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not write Node Information Header.");
		return STREAM_ERR;
	}
	Result += Rc;

	//Save Child
	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == FALSE)
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

	I3_PERSIST_TREENODE data;
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
		I3FATAL( "Could not read Node information head.");
		return STREAM_ERR;
	}
	Result += Rc;

	setNodeStyle( data.m_Style);
	setNodeState( data.m_State);

	if( i3String::NCompare((char*)data.m_ID, "TRN2", 4) == 0)
	{
		Rc = m_ChildList.LoadFrom( pResFile, true);
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

				I3ASSERT( pObj != NULL);
				I3ASSERT( pObj->IsTypeOf( GetClassMeta()) == TRUE);

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

	INT32 i, ChildCount = 0;
	char conv[MAX_PATH];
	i3TreeNode * pNode;

	pXML->addAttr( "Style", getNodeStyle());
	pXML->addAttr( "State", getNodeState());

	//Valid ChildCount
	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pNode = getChild( i);

		if( pNode->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == FALSE)
		{
			i3String::Format( conv, MAX_PATH -1, "Child_%d", ChildCount);

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

	pXML->getAttr( "Style", &m_NodeStyle);
	pXML->getAttr( "State", &m_NodeState);
	pXML->getAttr( "ChildCount", &ChildCount);

	for( i = 0; i < ChildCount; i++)
	{
		i3String::Format( conv, MAX_PATH -1, "Child_%d", i);

		pXML->getAttr( conv, &iVal);
		I3ASSERT( iVal > 0);

		i3TreeNode * pNode = (i3TreeNode*)pFile->FindObjectByID( iVal);
		I3ASSERT( pNode != NULL);

		AddChild( pNode);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
i3TreeNode * i3TreeNode::FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	INT32 i;
	i3TreeNode * pResult;

	if( pRoot->IsTypeOf( pMeta))
		return pRoot;

	if( bRecursive)
	{
		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByType( pRoot->getChild( i), pMeta, true);
			if( pResult != NULL)
				return pResult;
		}
	}

	return NULL;
}

void i3TreeNode::FindNodeByType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3List * pList, bool bRecursive)
{
	if( pRoot->IsTypeOf( pMeta))
		pList->Add( pRoot);

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByType( pRoot->getChild( i), pMeta, pList, true);
		}
	}
}

i3TreeNode * i3TreeNode::FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, bool bRecursive)
{
	if( pRoot->IsExactTypeOf( pMeta))
		return pRoot;

	if( bRecursive)
	{
		INT32 i;
		i3TreeNode * pResult;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByExactType( pRoot->getChild( i), pMeta, true);
			if( pResult != NULL)
				return pResult;
		}
	}

	return NULL;
}

void i3TreeNode::FindNodeByExactType( i3TreeNode * pRoot, i3ClassMeta * pMeta, i3List * pList, bool bRecursive)
{
	if( pRoot->IsExactTypeOf( pMeta))
		pList->Add( pRoot);

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByExactType( pRoot->getChild(i), pMeta, pList, true);
		}
	}
}

i3TreeNode * i3TreeNode::FindNodeByName( i3TreeNode * pRoot, const char * pszName, bool bRecursive)
{
	if( pRoot->hasName())
	{
		if( i3String::Compare( pRoot->GetName(), (char *) pszName) == 0)
			return pRoot;
	}

	if( bRecursive)
	{
		INT32 i;
		i3TreeNode * pResult;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			pResult = FindNodeByName( pRoot->getChild(i), pszName, true);
			if( pResult != NULL)
				return pResult;
		}
	}

	return NULL;
}

void i3TreeNode::FindNodeByName( i3TreeNode * pRoot, const char * pszName, i3List * pList, bool bRecursive)
{
	if( pRoot->hasName())
	{
		if( i3String::Compare( pRoot->GetName(), (char *) pszName) == 0)
		{
			pList->Add( pRoot);
		}
	}

	if( bRecursive)
	{
		INT32 i;

		for( i = 0; i < pRoot->getChildCount(); i++)
		{
			FindNodeByName( pRoot->getChild(i), pszName, pList, true);
		}
	}
}

static i3TreeNode * _Rec_CreateClone( i3TreeNode * pSrc, i3TreeNode * pDest, I3_COPY_METHOD method)
{
	INT32 i, Count, idx;
	i3TreeNode * pNew, * pChild;
	i3ClassMeta * pMeta;
	i3List		ChildList;

	for( i = 0, Count = 0; i < pSrc->getChildCount(); i++)
	{
		pChild = pSrc->getChild( i);

		if( pChild->isNodeStyle( I3_TREENODE_STYLE_VOLATILE) == FALSE)
			Count++;
	}

	if( Count > 0)
	{
		i3List * pList = pDest->getChildList();

		pList->SetCount( Count);
	}

	for( i = 0, idx = 0; i < pSrc->getChildCount(); i++)
	{
		pChild = pSrc->getChild( i);

		if( pChild->isNodeStyle( I3_TREENODE_STYLE_VOLATILE))
			continue;

		pMeta = pChild->GetMeta();

		pNew = (i3TreeNode *) CREATEINSTANCE( pMeta);
		pChild->CopyTo( pNew, method);

		pNew->AddRef();

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
	i3ClassMeta * pMeta;

	pMeta = pRoot->GetMeta();
	pNew = (i3TreeNode *) CREATEINSTANCE( pMeta);
	pRoot->CopyTo( pNew, method);

	_Rec_CreateClone( pRoot, pNew, method);

	return pNew;
}

static bool	_Rec_Traverse( INT32 depth, i3TreeNode * pNode, I3_TREE_TRAVERSE_PROC pProc, void * pUserData)
{
	INT32 i;
	i3TreeNode * pChild;

	if( pProc != NULL)
	{
		if( pProc( depth, pNode, pUserData) == FALSE)
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
