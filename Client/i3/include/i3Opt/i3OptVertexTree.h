#if !defined( __I3_OPT_VERTEX_TREE_H)
#define __I3_OPT_VERTEX_TREE_H

#include "i3Base/i3AVLTree.h"

struct VT_INFO
{
	INT32		m_idxOld = 0;
	INT32		m_idxNew = 0;

	i3VertexFormat	m_Format;
	VEC3D		m_Pos;
	VEC3D		m_Normal;
	VEC3D		m_Tangent;
	VEC3D		m_Binormal;
	COLORREAL	m_Color;
	UINT8		m_Index[4] = { 0 };
	REAL32		m_Weight[4] = { 0.0f };
	VEC2D		m_TexCoord[4];

	void		Config( i3VertexArray * pVA, INT32 idx);
	void		SetVertex( i3VertexArray * pVA, INT32 idx);

	bool operator < ( const VT_INFO& p);

	// Whether the two points are equal
	//
	bool operator == (const VT_INFO & p);

	// Whether a point is greater than another point
	//
	bool operator > (const VT_INFO& p)
	{
		// A point is greater than the other if it is not less than or equal
		//
		return !( (*this) < p || (*this)==p );
	}

	// Whether a point is less than or equal
	//
	bool operator <= (const VT_INFO& p)
	{
		return ((*this) < p || (*this == p));
	}

	// Whether a point is greater than or equal
	//
	bool operator >= (const VT_INFO& p)
	{
		// A point is greater-than or equal if it is NOT less than the other
		// 
		return !((*this) < p);
	}
};

// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3OptVertexTree : public i3ElementBase
{
	I3_CLASS_DEFINE( i3OptVertexTree, i3ElementBase);
protected:

protected:
	i3VertexArray *	m_pVA = nullptr;
	i3VertexArray *	m_pNewVA = nullptr;

protected:
	bool			Add( INT32 idxNode, INT32 idxOld, VEC3D * pPos);

	static bool		_CountProc( i3AVLNode<VT_INFO> * pNode, INT32 level, void * pUserData);
	static bool		_SetupVAProc( i3AVLNode<VT_INFO> * pNode, INT32 level, void * pUserData);

public:
	i3OptVertexTree(void);
	virtual ~i3OptVertexTree(void);

	i3VertexArray *	getNewVA(void)			{ return m_pNewVA; }

	bool			Create( i3VertexArray * pSrcVA, INT32 * pIndexTable);

};

#endif

