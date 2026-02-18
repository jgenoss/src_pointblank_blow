#if !defined( __I3_COLLIDER_SET_H)
#define __I3_COLLIDER_SET_H

#include "i3Node.h"
#include "i3Collider.h"

class I3_EXPORT_SCENE i3ColliderSet : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ColliderSet);
 
protected:
	// Persistent Members
	i3List						m_ColList;
	UINT32						m_GroupIndex;
	UINT32						m_Style;
	REAL32						m_DistSq;
	i3EventReceiver *			m_pReceiver;

	VEC3D						m_Vel;
	REAL32						m_Mass;
	REAL32						m_Damp;
	REAL32						m_Elastic;

	// Volatile Members
	I3_COLLISION_RESULT *		m_pResult;
	UINT32						m_ResultCount;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3ColliderSet(void);
	virtual ~i3ColliderSet(void);

	void				AddCollider( i3Collider *	pAttr);
	void				RemoveCollider( i3Collider * pAttr);
	void				RemoveAllColliders(void);
	INT32				GetColliderCount(void)					{ return m_ColList.GetCount(); }
	i3Collider *		GetCollider( INT32 idx)					{ return (i3Collider *) m_ColList.Items[idx]; }

	UINT32				GetGroup(void)							{ return m_GroupIndex; }
	void				SetGroup( UINT32 group)					{ m_GroupIndex = group; }

	UINT32				GetStyle(void)							{ return m_Style; }
	void				SetStyle( UINT32 style)					{ m_Style = style; }
	void				AddStyle( UINT32 style)					{ m_Style |= style; }
	void				RemoveStyle( UINT32 style)				{ m_Style &= ~style; }

	REAL32				GetDistance(void)						{ return i3Math::Sqrt( m_DistSq); }
	void				SetDistance( REAL32 dist)				{ m_DistSq = (dist * dist); }

	void				SetVelocity( VEC3D * pVel)				{ i3Vector::Copy( &m_Vel, pVel); }
	VEC3D *				GetVelocity(void)						{ return &m_Vel; }

	i3EventReceiver	*	GetEventReceiver(void)					{ return m_pReceiver; }
	void				SetEventReceiver( i3EventReceiver * pObj);

	I3_COLLISION_RESULT *	GetResult(void)						{ return m_pResult; }
	void					SetResult( I3_COLLISION_RESULT * p)	{ m_pResult = p; }

	UINT32					GetResultCount(void)				{ return m_ResultCount; }
	void					SetResultCount( UINT32 count)		{ m_ResultCount = count; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	/*
	static BOOL CollisionTest(i3Node* pCollRoot,i3CollideeLine*   pCollidee2,i3CollisionResult * pResult);
	static BOOL CollisionTest(i3Node* pCollRoot,i3CollideeSphere* pCollidee2,i3CollisionResult * pResult);
	static BOOL CollisionTest(i3CollideeSphere* pCollidee1,i3CollideeLine*   pCollidee2,i3CollisionResult * pResult);
	static BOOL CollisionTest(i3CollideeSphere* pCollidee1,i3CollideeSphere* pCollidee2,i3CollisionResult * pResult);
	*/
};

#endif
