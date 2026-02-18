#if !defined( __COLLIDEE_H)
#define __COLLIDEE_H

#define COLL_STATE_SELECTED			0x00000001
#define COLL_STATE_FREEZED			0x00000002

class Collidee;

typedef		struct _tagSelectedTriInfo
{
	Collidee *		m_pCollidee;
	INT32			m_TriIndex;
	REAL32			m_Distance;
} SELECT_TRI_INFO;

class Collidee : public i3ElementBase
{
	I3_CLASS_DEFINE( Collidee);
protected:
	UINT32				m_State;
	i3ColliderSet *		m_pColliderSet;
	i3Collidee *		m_pCollidee;
	MATRIX				m_Matrix;

	i3Node *			m_pVisibleMesh;
	i3AttrSet *			m_pTriGeoRoot;
	i3AttrSet *			m_pLineGeoRoot;

	i3GeometryAttr *	m_pTriGeoAttr;
	i3GeometryAttr *	m_pLineGeoAttr;

	i3Vector3Array *	m_pTransPosArray;

protected:
	void				_UpdateAllTriColor(void);
	void				_UpdateAllLineColor(void);
	bool				_IsTouchTri( I3_COLLIDEE_TRI32 * pTri, I3RECT * pRect, BOOL bBothSide, VEC3D * pNorm, REAL32 * pDist);

public:
	Collidee(void);
	virtual ~Collidee(void);

	UINT32				getState(void)								{ return m_State; }
	void				setState( UINT32 state)						{ m_State = state; }
	void				addState( UINT32 mask)						{ m_State |= mask; }
	void				removeState( UINT32 mask)					{ m_State &= ~mask; }
	bool				isSelected(void)							{ return (m_State & COLL_STATE_SELECTED) != 0; }
	bool				isFreezed(void)								{ return (m_State & COLL_STATE_FREEZED) != 0; }

	void				Select(void);
	void				Unselect(void);

	i3ColliderSet *		getColliderSet(void)						{ return m_pColliderSet; }

	i3Collidee *		getCollidee(void)							{ return m_pCollidee; }
	void				SetCollidee( i3ColliderSet * pCollSet, i3Collidee * pColl, MATRIX * pMatrix);

	INT32				getTriangleCount(void)
	{
		if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()))
		{
			return ((i3CollideeMesh *) m_pCollidee)->GetTriangleCount();
		}
		
		return 0;
	}

	I3_COLLIDEE_TRI32 *	getTriangle( INT32 idx)
	{
		if( m_pCollidee->IsTypeOf( i3CollideeMesh::GetClassMeta()))
		{
			return ((i3CollideeMesh *) m_pCollidee)->GetTriangle( idx);
		}
		
		return NULL;
	}

	i3Node *			getVisibleMesh(void)						{ return m_pVisibleMesh; }

	INT32				GetTouchedTri( I3RECT * pRect, MATRIX * pViewProj, BOOL bBothSide, i3List * pList);
	void				SelectTri( INT32 idx, BOOL bSelect);

	void				SelectAllTri(void);
	void				UnselectAllTri(void);

	void				Update(void);
};

#endif
