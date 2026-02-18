#include "stdafx.h"
#include "i3ExportTracer.h"

i3ExportTracer::i3ExportTracer(void)
{
	m_pUserProc = NULL;
	m_pUserData = NULL;

	m_pRoot = NULL;

	m_bSelectedOnly = false;
}

i3ExportTracer::~i3ExportTracer(void)
{

}

bool i3ExportTracer::_Rec_Traverse( INT32 Level, INode * pNode)
{
	INT32 i;
	bool bResult = true;

	{
		bool bShow = true, bSelected = true, bBone = false, bExport;

		bSelected = (!m_bSelectedOnly) || pNode->Selected();

		bExport = (bShow && bSelected) || bBone;

		if( bExport)
		{
			if( m_pUserProc != NULL)
			{
				if( m_pUserProc( Level, pNode, m_pUserData) == false)
				{
					goto ExitPart;
				}
			}
		}
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		INode * pChild = pNode->GetChildNode( i);

		if( _Rec_Traverse( Level + 1, pChild) == false)
			goto ExitPart;
	}

	bResult = true;

ExitPart:

	return bResult;
}

void i3ExportTracer::Trace( INode * pRoot, INODE_TRAVERSE_PROC pProc, void * pData)
{
	m_pUserProc = pProc;
	m_pUserData = pData;

	m_pRoot = pRoot;

	_Rec_Traverse( 0, pRoot);
}
