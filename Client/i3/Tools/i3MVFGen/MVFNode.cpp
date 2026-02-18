#include "stdafx.h"
#include "MVFNode.h"
#include "GlobVar.h"

I3_CLASS_INSTANCE( MVFNode, i3ElementBase)

INT32 _CompareProc( MVFNode * p1, MVFNode * p2)
{
	return i3String::Compare( p1->GetName(), p2->GetName());
}

MVFNode::MVFNode(void)
{
	memset( &m_Entry, 0, sizeof( m_Entry));

	m_ChildList.SetCompareProc( (COMPAREPROC) _CompareProc);

	m_pParent = NULL;
}

MVFNode::~MVFNode(void)
{
	m_pParent = NULL;

	RemoveAllChild();
}

void MVFNode::AddChild( MVFNode * pChild)
{
	I3ASSERT( pChild != NULL);

	pChild->AddRef();
	m_ChildList.Add( pChild);

	pChild->SetParent( this);
}

void MVFNode::RemoveChild( MVFNode * pChild)
{
	INT32 idx;

	idx = m_ChildList.IndexOf( pChild);
	I3ASSERT( idx != -1);

	pChild->Release();
	m_ChildList.Delete( idx);
}

void MVFNode::RemoveAllChild(void)
{
	INT32 i;
	MVFNode * pChild;

	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pChild = (MVFNode *) m_ChildList.Items[i];

		pChild->Release();
	}

	m_ChildList.Clear();
}

MVFNode *	MVFNode::FindChildByName( const char * pszName)
{
	INT32 i;
	MVFNode * pChild;

	for( i = 0; i < m_ChildList.GetCount(); i++)
	{
		pChild = (MVFNode *) m_ChildList.Items[i];

		if( i3String::Compare( pChild->GetName(), pszName) == 0)
			return pChild;
	}

	return NULL;
}

void MVFNode::Traverse( char * pszFull, NODETRAVERSEPROC pProc, void * pUserData)
{
	char szTemp[MAX_PATH];
	INT32 i;
	MVFNode * pChild;

	if( pszFull == NULL)
	{
		strcpy( szTemp, g_pProfile->m_szWorkDir);
	}
	else
	{
		sprintf( szTemp, "%s/%s", pszFull, GetName());
	}

	pProc( szTemp, this, pUserData);

	for( i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);

		pChild->Traverse( szTemp, pProc, pUserData);
	}
}

void MVFNode::GetFullPath( char * pszPath)
{
	char szTemp[MAX_PATH];
	MVFNode * pNode = this;

	pszPath[0] = 0;

	while( pNode->GetParent() != NULL)
	{
		if( pszPath[0] != 0)
			sprintf( szTemp, "%s\\", pNode->GetName());
		else
			sprintf( szTemp, "%s", pNode->GetName());

		i3String::InsertStr( pszPath, szTemp, 0, strlen(szTemp));

		pNode = pNode->GetParent();
	}

	sprintf( szTemp, "%s\\", g_pProfile->m_szWorkDir);
	i3String::InsertStr( pszPath, szTemp, 0, strlen( szTemp));
}
