#include "stdafx.h"
#include "MeshInfo.h"
#include "Util.h"
#include "i3Export.h"

CMeshInfo::CMeshInfo(void)
{
	m_pNode = NULL;
	m_pMesh = NULL;
	m_pSrcObject = NULL;
	m_pVA = NULL;
	m_pIA = NULL;
	m_pOrgVA = NULL;
	m_pMapIA = NULL;

	m_pVerts = NULL;
	m_VertCount = m_MaxVertCount = 0;

	m_pFaces = NULL;
	m_FaceCount = 0;

	m_TexSetCount = 0;
	m_BlendBoneCount = 0;
	m_OrgVertCount = 0;
}

CMeshInfo::~CMeshInfo(void)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);
	I3_SAFE_RELEASE( m_pOrgVA);
	I3_SAFE_RELEASE( m_pMapIA);
	
	I3MEM_SAFE_FREE( m_pVerts);
	I3MEM_SAFE_FREE( m_pFaces);
}
/*
static INT32	_VertCompProc( CVertInfo * p1, CVertInfo * p2)
{
	return p1->m_idxPos - p2->m_idxPos;
}
*/
bool CMeshInfo::Create( INode * pNode, Mesh * pMesh, Matrix3 * pPosTX, bool bVertexColor, INT32 texSetCount, INT32 blendBoneCount, bool bTangent)
{
	m_pNode = pNode;
	m_pMesh = pMesh;

	m_pSrcObject = pNode->EvalWorldState( g_pExport->getStartTime()).obj;

	m_bVertexColor = bVertexColor;
	m_bTangent = bTangent;
	m_OrgVertCount = m_pMesh->getNumVerts();
	m_MaxVertCount = pMesh->getNumFaces() * 3;

	m_PosTX = *pPosTX;

	I3MEM_SAFE_FREE( m_pVerts);

	if( m_MaxVertCount <= 0)
		return false;

	m_pVerts = (CVertInfo *) i3MemAlloc( sizeof(CVertInfo) * m_MaxVertCount);
	i3mem::FillZero( m_pVerts, sizeof(CVertInfo) * m_MaxVertCount);

	m_VertCount = 0;

	// Mapping Data
	m_IndexList.reserve( m_MaxVertCount);
	//	m_IndexList.SetCompareProc( (COMPAREPROC) _VertCompProc);

	m_TexSetCount = texSetCount;
	m_BlendBoneCount = blendBoneCount;

	// Faces
	m_MaxFaceCount = pMesh->getNumFaces();
	m_FaceCount = 0;

	m_pFaces = (CFaceInfo *) i3MemAlloc( sizeof(CFaceInfo) * m_MaxFaceCount);
	I3ASSERT( m_pFaces);

	return true;
}

INT32 CMeshInfo::FindVert( CVertInfo * pFace)
{
	i3::pair<i3::vector_multiset<CVertInfo*>::iterator, i3::vector_multiset<CVertInfo*>::iterator>	pr = m_IndexList.equal_range(pFace);

	if ( pr.first == pr.second) 
		return -1;

	for (i3::vector_multiset<CVertInfo*>::iterator it = pr.first; it != pr.second; ++it )
	{
		INT32 idx = i3::distance( m_IndexList.begin(), it);

		if( m_pVerts[idx].isSame( pFace, m_TexSetCount, m_BlendBoneCount))
			return idx;
	}

	return -1;
}

INT32 CMeshInfo::AddVert( CVertInfo * pFace)
{
	INT32 idx;

	idx = FindVert( pFace);
	if( idx != -1)
	{
		CVertInfo * pVert = & m_pVerts[ idx];

		i3Vector::Add( & pVert->m_Tangent, &pVert->m_Tangent, & pFace->m_Tangent);
		i3Vector::Add( & pVert->m_Binormal, &pVert->m_Binormal, & pFace->m_Binormal);
		return idx;
	}

	idx = m_VertCount;
	m_VertCount++;
	
	I3_BOUNDCHK( idx, m_MaxVertCount);

	m_pVerts[ idx].CopyFrom( pFace);
	m_IndexList.insert( & m_pVerts[ idx]);

	//m_pMapIndex[ idx] = pFace->m_idxPos;

	return idx;
}

void CMeshInfo::AddFace( INT32 idx0, INT32 idx1, INT32 idx2)
{
	INT32 idx = m_FaceCount;

	m_FaceCount++;

	I3_BOUNDCHK( idx, m_MaxFaceCount);

	m_pFaces[idx].m_Index[0] = idx0;
	m_pFaces[idx].m_Index[1] = idx1;
	m_pFaces[idx].m_Index[2] = idx2;
}

void CMeshInfo::BuildVertexArray( bool bOrgVA)
{
	INT32 i, j;

	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pOrgVA);
	I3_SAFE_RELEASE( m_pMapIA);

	if( m_VertCount <= 0)
		return;

	{
		i3VertexFormat fmt;

		fmt.SetHasPosition( TRUE);
		fmt.SetHasNormal( TRUE);

		if( m_bVertexColor)
			fmt.SetHasColor( TRUE);

		fmt.SetTextureCoordSetCount( m_TexSetCount);
		fmt.SetTextureCoordCount( 2);

		fmt.SetBlendIndexCount( m_BlendBoneCount);
		if( m_BlendBoneCount > 0)
			fmt.SetBlendWeightCount( m_BlendBoneCount - 1);

		if( m_bTangent )
		{
			fmt.SetHasTangent( TRUE);
			fmt.SetHasBinormal( TRUE);
		}

		m_pIA = i3IndexArray::new_object();

		m_pIA->Create( m_FaceCount * 3, 0, m_FaceCount * 3);
		m_idxIA = 0;

		m_pVA = i3VertexArray::new_object();
		m_pVA->Create( &fmt, m_VertCount, 0);
		m_pVA->Lock();

		for( i = 0; i < m_VertCount; i++)
		{
			CVertInfo * pV = & m_pVerts[ i];

			// Position
			m_pVA->SetPosition( i, &pV->m_Pos);

			// Normal
			m_pVA->SetNormal( i, &pV->m_Normal);

			// Color
			if( fmt.GetHasColor())
			{
				COLORREAL c;

				i3Color::Set( &c, & pV->m_Color);
				m_pVA->SetColor( i, &c);
			}

			// Texture Coords
			for( j = 0; j < m_TexSetCount; j++)
			{
				m_pVA->SetTextureCoord( j, i, &pV->m_TexCoord[j]);
			}

			// Blend Index & Weights
			for( j = 0; j < m_BlendBoneCount; j++)
			{
				m_pVA->SetBlendIndex( j, i, pV->m_Index[j]);

				if( j < (m_BlendBoneCount - 1))
					m_pVA->SetBlendWeight( j, i, pV->m_Weight[j]);
			}

			// Tangent & Binormal
			if( fmt.GetHasTangent())
			{
				VEC3D temp;

				i3Vector::Normalize( &temp, & pV->m_Tangent);
				m_pVA->SetTangent( i, &temp);

				i3Vector::Normalize( &temp, & pV->m_Binormal);
				m_pVA->SetBinormal( i, & temp);
			}
		}

		m_pVA->Unlock();
	}

	///////////////////////////////////
	// Original Vertex Data
	if( bOrgVA)
	{
		// Vertex Array
		{
			i3VertexFormat fmt;
			INT32 cnt;

			fmt.SetHasPosition( TRUE);

			cnt = m_pMesh->getNumVerts();

			m_pOrgVA = i3VertexArray::new_object();
			m_pOrgVA->Create( &fmt, cnt, 0);

			m_pOrgVA->Lock();

			for( i = 0; i < cnt; i++)
			{
				VEC3D pos;

				Convert( &pos, m_pMesh->getVert( i));

				m_pOrgVA->SetPosition( i, &pos);
			}

			m_pOrgVA->Unlock();
		}

		// Mapping Data
		{
			m_pMapIA = i3IndexArray::new_object();

			if( m_VertCount > 0xFFFF)
				m_pMapIA->SetDataType( I3G_IDX_32BITS);

			m_pMapIA->Create( m_VertCount, 0, m_pOrgVA->GetCount());

			m_pMapIA->Lock();

			for( i = 0; i < m_VertCount; i++)
			{
				m_pMapIA->SetIndex( i, m_pVerts[i].m_idxPos);
			}

			m_pMapIA->Unlock();
		}
	}
}

void CMeshInfo::BeginSetIndexArray(void)
{
	m_pIA->Lock();
}

void CMeshInfo::SetIndex( INT32 idxFace)
{
	I3_BOUNDCHK( idxFace, m_MaxFaceCount);

	CFaceInfo * pFace = & m_pFaces[ idxFace];

	m_pIA->SetIndex( m_idxIA + 0, pFace->m_Index[0]);
	m_pIA->SetIndex( m_idxIA + 1, pFace->m_Index[1]);
	m_pIA->SetIndex( m_idxIA + 2, pFace->m_Index[2]);

	m_idxIA += 3;
}

void CMeshInfo::EndSetIndexArray(void)
{
	m_pIA->Unlock();
}

void CMeshInfo::Dump(void)
{
	//INT32 i;

	I3TRACE( " ----------- MESH : %s  ----------------\n", m_pNode->GetName());
	I3TRACE( " NODE:%p    OBJECT:%p    MESH:%p\n", m_pNode, m_pSrcObject, m_pMesh);
	I3TRACE( " Face Count : %d (MAX:%d)\n", m_VertCount, m_MaxVertCount);
	I3TRACE( " Org V Cnt  : %d\n", m_OrgVertCount);

	/*
	for( i = 0; i < m_VertCount; i++)
	{
		CVertInfo * p = & m_pVerts[i];

		I3TRACE( "[%d]\t", i);

		I3TRACE( "\n");
	}
	*/
}
