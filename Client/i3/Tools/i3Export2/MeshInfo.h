#if !defined( __MESH_INFO_H)
#define __MESH_INFO_H

#include "i3Export_Define.h"

class CVertInfo
{
public:
	INT32		m_idxPos;
	VEC3D		m_Pos;
	VEC3D		m_Normal;
	VEC3D		m_Tangent;
	VEC3D		m_Binormal;
	COLOR		m_Color;
	VEC2D		m_TexCoord[16];
	UINT8		m_Index[4];
	REAL32		m_Weight[4];

	void		reset(void)
	{
		i3mem::Fill( this, 0, sizeof(CVertInfo));
	}

	bool		isSame( CVertInfo * pOther, INT32 TexSetCount, INT32 BlendCount)
	{
		INT32 i;

		if( m_idxPos != pOther->m_idxPos)
			return false;

		if( i3Vector::isAlike( &m_Normal, & pOther->m_Normal) == FALSE)
			return false;

		/*
		if( i3Vector::isAlike( &m_Tangent, & pOther->m_Tangent) == FALSE)
			return false;

		if( i3Vector::isAlike( &m_Binormal, & pOther->m_Binormal) == FALSE)
			return false;
			*/
		
		if( i3Color::IsSame( &m_Color, &pOther->m_Color) == FALSE)
			return false;

		for( i = 0; i < TexSetCount; i++)
		{
			if( i3Vector::isAlike( &m_TexCoord[i], &pOther->m_TexCoord[i]) == FALSE)
				return false;
		}

		for( i = 0; i < BlendCount; i++)
		{
			if( m_Index[i] != pOther->m_Index[i])
				return false;

			if( i < (BlendCount - 1))
			{
				if( i3Math::isAlike( m_Weight[i], pOther->m_Weight[i]) == FALSE)
					return false;
			}
		}

		return true;
	}

	void	CopyFrom( CVertInfo * pSrc)
	{
		i3mem::Copy( this, pSrc, sizeof(CVertInfo));
	}
};

template<> struct i3::less<CVertInfo*> 
{
	bool operator()(const CVertInfo* p1, const CVertInfo* p2) const { return p1->m_idxPos < p2->m_idxPos;  }
};

class CFaceInfo
{
public:
	INT32		m_Index[3];
};

class CMeshInfo
{
protected:
	INode *			m_pNode;
	Object *		m_pSrcObject;
	Mesh *			m_pMesh;
	i3VertexArray *	m_pVA;
	i3IndexArray *	m_pIA;
	i3VertexArray *	m_pOrgVA;
	i3IndexArray *	m_pMapIA;

	CVertInfo *		m_pVerts;
	INT32			m_MaxVertCount;
	INT32			m_VertCount;

	CFaceInfo *		m_pFaces;
	INT32			m_FaceCount;
	INT32			m_MaxFaceCount;

	bool			m_bVertexColor;
	bool			m_bTangent;
	INT32			m_TexSetCount;
	INT32			m_BlendBoneCount;

	INT32			m_OrgVertCount;

	INT32			m_idxIA;

	Matrix3			m_PosTX;

	i3::vector_multiset<CVertInfo*>		m_IndexList;

public:
	CMeshInfo(void);
	virtual ~CMeshInfo(void);

	INode *			getNode(void)					{ return m_pNode; }
	Mesh *			getMesh(void)					{ return m_pMesh; }
	Object *		getSrcObject(void)				{ return m_pSrcObject; }
	i3VertexArray *	getVertexArray(void)			{ return m_pVA; }
	i3IndexArray *	getIndexArray(void)				{ return m_pIA; }
	i3VertexArray *	getOriginalVertices(void)		{ return m_pOrgVA; }
	i3IndexArray *	getMappingData(void)			{ return m_pMapIA; }

	void			setTexCoordSetCount( INT32 cnt)	{ m_TexSetCount = cnt; }
	void			setBlendBoneCount( INT32 cnt)	{ m_BlendBoneCount = cnt; }

	bool			Create( INode * pNode, Mesh * pMesh, Matrix3 * pPosTX, bool bVertexColor, INT32 texSetCount, INT32 blendBoneCount, bool bTangent);
	INT32			FindVert( CVertInfo * pFace);
	INT32			AddVert( CVertInfo * pFace);

	void			AddFace( INT32 idx0, INT32 idx1, INT32 idx2);

	void			BuildVertexArray( bool bOrgVA = false);

	void			BeginSetIndexArray(void);
	void			SetIndex( INT32 idxFace);
	INT32			getDrawStartIndex(void)				{ return m_idxIA; }
	void			EndSetIndexArray(void);

	void			Dump(void);

	INT32			getVertCount( void)					{ return m_VertCount; }
	CVertInfo *		getVertInfo( INT32 idx)				{ return &m_pVerts[ idx]; }
};

#endif
