#include "stdafx.h"
#include "Warn.h"

I3_ABSTRACT_CLASS_INSTANCE( Warn, i3ElementBase);

Warn::Warn(void)
{
	m_bEnable = true;
	m_pUserData = NULL;
	m_bResult = false;
}

Warn::~Warn(void)
{
}

bool Warn::_Rec_CheckTrace( i3Node * pNode, i3StringList * pMsgList)
{
	if( _OnCheckNode( pNode, pMsgList, m_pUserData) == false)
		return false;

	INT32 i;
	i3Node * pChild;

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		if( _Rec_CheckTrace( pChild, pMsgList) == false)
			return false;
	}
	
	return true;
}

void Warn::_CheckTrace( i3Node * pNode, i3StringList * pMsgList, void * pUserData)
{
	m_bResult = false;
	m_pUserData = pUserData;

	_Rec_CheckTrace( pNode, pMsgList);
}

bool Warn::_OnCheckNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData)
{
	return false;
}

bool Warn::_Rec_SolveTrace( i3Node * pNode, i3StringList * pMsgList)
{
	if( _OnSolveNode( pNode, pMsgList, m_pUserData) == false)
		return false;

	INT32 i;
	i3Node * pChild;

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		if( _Rec_SolveTrace( pChild, pMsgList) == false)
			return false;
	}
	
	return true;
}

void Warn::_SolveTrace( i3Node * pNode, i3StringList * pMsgList, void * pUserData)
{
	m_bResult = false;
	m_pUserData = pUserData;

	_Rec_SolveTrace( pNode, pMsgList);
}

bool Warn::_OnSolveNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData)
{
	return false;
}

