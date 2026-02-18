#include "i3OptPCH.h"
#include "i3OptVertexTree.h"

I3_CLASS_INSTANCE( i3OptVertexTree);

////////////////////////////////////////////////////////
void VT_INFO::Config( i3VertexArray * pVA, INT32 idx)
{
	INT32 i;

	m_idxOld = idx;
	m_idxNew = -1;
	m_Format = *pVA->GetFormat();

	if( m_Format.GetHasPosition())
		pVA->GetPosition( idx, &m_Pos);

	if( m_Format.GetHasColor())
		pVA->GetColor( idx, &m_Color);

	if( m_Format.GetHasNormal())
		pVA->GetNormal( idx, &m_Normal);

	if( m_Format.GetHasTangent())
		pVA->GetTangent( idx, &m_Tangent);

	if( m_Format.GetHasBinormal())
		pVA->GetBinormal( idx, &m_Binormal);

	for( i = 0; i < m_Format.GetTextureCoordSetCount(); i++)
	{
		pVA->GetTextureCoord( i, idx, &m_TexCoord[i]);
	}

	for( i = 0; i < m_Format.GetBlendIndexCount(); i++)
		m_Index[i] = (UINT8) pVA->GetBlendIndex( i, idx);

	for( i = 0; i < m_Format.GetBlendWeightCount(); i++)
		m_Weight[i] = pVA->GetBlendWeight( i, idx);
}

void VT_INFO::SetVertex( i3VertexArray * pVA, INT32 idx)
{
	INT32 i;

	if( m_Format.GetHasPosition())
		pVA->SetPosition( idx, &m_Pos);

	if( m_Format.GetHasColor())
		pVA->SetColor( idx, &m_Color);

	if( m_Format.GetHasNormal())
		pVA->SetNormal( idx, &m_Normal);

	if( m_Format.GetHasTangent())
		pVA->SetTangent( idx, &m_Tangent);

	if( m_Format.GetHasBinormal())
		pVA->SetBinormal( idx, &m_Binormal);

	for( i = 0; i < m_Format.GetTextureCoordSetCount(); i++)
	{
		pVA->SetTextureCoord( i, idx, &m_TexCoord[i]);
	}

	for( i = 0; i < m_Format.GetBlendIndexCount(); i++)
		pVA->SetBlendIndex( i, idx, (UINT32) m_Index[i]);

	for( i = 0; i < m_Format.GetBlendWeightCount(); i++)
		pVA->SetBlendWeight( i, idx, m_Weight[i]);
}

bool VT_INFO::operator < (const VT_INFO & p)
{
	INT32 i;

	// Position
	if( m_Format.GetHasPosition())
	{
		//if( m_Pos != p.m_Pos)
		if( (i3Math::isAlike( m_Pos.x, p.m_Pos.x, I3_EPS) == false) ||
			(i3Math::isAlike( m_Pos.y, p.m_Pos.y, I3_EPS) == false) ||
			(i3Math::isAlike( m_Pos.z, p.m_Pos.z, I3_EPS) == false) )
		{
			if( m_Pos < p.m_Pos)
				return true;
			else
				return false;
		}
	}

	if( m_Format.GetHasColor())
	{
		if( m_Color != p.m_Color)
		{
			if( m_Color < p.m_Color)
				return true;
			else
				return false;
		}
	}

	if( m_Format.GetHasNormal())
	{
		if( m_Normal != p.m_Normal)
		{
			if( m_Normal < p.m_Normal)
				return true;
			else
				return false;
		}
	}

	if( m_Format.GetHasTangent())
	{
		if( m_Tangent != p.m_Tangent)
		{
			if( m_Tangent < p.m_Tangent)
				return true;
			else
				return false;
		}
	}

	if( m_Format.GetHasBinormal())
	{
		if( m_Binormal != p.m_Binormal)
		{
			if( m_Binormal < p.m_Binormal)
				return true;
			else
				return false;
		}
	}

	for( i = 0; i < m_Format.GetTextureCoordSetCount(); i++)
	{
		if( m_TexCoord[i] != p.m_TexCoord[i])
		{
			if( m_TexCoord[i] < p.m_TexCoord[i])
				return true;
			else
				return false;
		}
	}

	for( i = 0; i < m_Format.GetBlendIndexCount(); i++)
	{
		if( m_Index[i] != p.m_Index[i])
		{
			if( m_Index[i] < p.m_Index[i])
				return true;
			else
				return false;
		}
	}

	for( i = 0; i < m_Format.GetBlendWeightCount(); i++)
	{
		if( m_Weight[i] != p.m_Weight[i])
		{
			if( m_Weight[i] < p.m_Weight[i])
				return true;
			else
				return false;
		}
	}

	return false;
}

bool VT_INFO::operator == (const VT_INFO & p)
{
	INT32 i;

	// Position
	if( m_Format.GetHasPosition())
	{
		if( (i3Math::isAlike( m_Pos.x, p.m_Pos.x, I3_EPS) == false) ||
			(i3Math::isAlike( m_Pos.y, p.m_Pos.y, I3_EPS) == false) ||
			(i3Math::isAlike( m_Pos.z, p.m_Pos.z, I3_EPS) == false) )
		//if( m_Pos != p.m_Pos)
			return false;
	}

	if( m_Format.GetHasColor())
		if( m_Color != p.m_Color)
			return false;

	if( m_Format.GetHasNormal())
		if( m_Normal != p.m_Normal)
			return false;

	if( m_Format.GetHasTangent())
		if( m_Tangent != p.m_Tangent)
			return false;

	if( m_Format.GetHasBinormal())
		if( m_Binormal != p.m_Binormal)
			return false;

	for( i = 0; i < m_Format.GetTextureCoordSetCount(); i++)
	{
		if( m_TexCoord[i] != p.m_TexCoord[i])
			return false;
	}

	for( i = 0; i < m_Format.GetBlendIndexCount(); i++)
	{
		if( m_Index[i] != p.m_Index[i])
			return false;
	}

	for( i = 0; i < m_Format.GetBlendWeightCount(); i++)
	{
		if( m_Weight[i] != p.m_Weight[i])
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////

i3OptVertexTree::i3OptVertexTree(void) {}

i3OptVertexTree::~i3OptVertexTree(void)
{
	I3_SAFE_RELEASE( m_pNewVA);
	I3_SAFE_RELEASE( m_pVA);
}

bool i3OptVertexTree::_CountProc( i3AVLNode<VT_INFO> * pNode, INT32 level, void * pUserData)
{
	INT32 * pCount = (INT32 *) pUserData;

	pNode->m_Data.m_idxNew = *pCount;
	(*pCount) ++;

	return true;
}

bool i3OptVertexTree::_SetupVAProc( i3AVLNode<VT_INFO> * pNode, INT32 level, void * pUserData)
{
	i3VertexArray * pVA = (i3VertexArray *) pUserData;
	VT_INFO * pInfo = & pNode->m_Data;
	INT32 idxNew = pInfo->m_idxNew;

	I3ASSERT( idxNew < (INT32) pVA->GetCount());

	pInfo->SetVertex( pVA, idxNew);

	return true;
}

/*
static void _DumpDataProc( VT_INFO * pData)
{
	I3TRACE( "%f, %f, %f\n", pData->m_Pos.x, pData->m_Pos.y, pData->m_Pos.z);
}
*/

bool i3OptVertexTree::Create( i3VertexArray * pSrcVA, INT32 * pIndexTable)
{
	INT32 i;
	VT_INFO info;
	i3AVLTree< VT_INFO>	* pTree;

	I3_REF_CHANGE( m_pVA, pSrcVA);

	m_pVA->Lock();

	{
		pTree = new i3AVLTree< VT_INFO>;

		//pTree->SetDumpProc( (i3AVLTree<VT_INFO>::DUMPPROC) _DumpDataProc);

		for( i = 0; i < (INT32) m_pVA->GetCount(); i++)
		{
			info.m_idxNew = -1;
			info.m_idxOld = i;

			info.Config( m_pVA, i);

			pTree->Add( info);
		}

		//pTree->CheckIntegrity();
	}

	// 개수
	{
		INT32 count = 0;

		pTree->Traversal( _CountProc, &count);

		I3TRACE( "TO INDEXED : %d -> %d\n", m_pVA->GetCount(), count);

		m_pNewVA = i3VertexArray::new_object();
		m_pNewVA->Create( m_pVA->GetFormat(), count, 0);
	}

	// Vertex 설정
	{
		m_pNewVA->Lock();

		pTree->Traversal( _SetupVAProc, m_pNewVA);

		m_pNewVA->Unlock();
	}

	// Index Table 설정.
	{
		for( i = 0; i < (INT32) m_pVA->GetCount(); i++)
		{
			info.m_idxNew = -1;
			info.m_idxOld = i;

			info.Config( m_pVA, i);

			VT_INFO * pFind = pTree->Find( info);

			if( pFind == nullptr)
			{
				pFind = pTree->FindLinear( info);
			}

			I3ASSERT( pFind != nullptr);

			pIndexTable[i] = pFind->m_idxNew;
		}

	}

	m_pVA->Unlock();

	delete pTree;

	return true;
}
