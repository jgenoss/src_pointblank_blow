#if !defined( __I3_COLLIDER_SET_H)
#define __I3_COLLIDER_SET_H

#include "i3Node.h"
#include "i3Collider.h"

class I3_EXPORT_SCENE i3ColliderSet : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3ColliderSet, i3Node);
 
protected:
	// Persistent Members
	i3::vector<i3Collider*>		m_ColList;
	UINT32						m_GroupIndex = 0;
	UINT32						m_Style = 0;
	REAL32						m_DistSq = 0.0f;
	i3EventReceiver *			m_pReceiver = nullptr;

	VEC3D						m_Vel;
	REAL32						m_Mass = 0.0f;
	REAL32						m_Damp = 0.0f;
	REAL32						m_Elastic = 0.0f;

	// Volatile Members
	I3_COLLISION_RESULT *		m_pResult = nullptr;
	UINT32						m_ResultCount = 0;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3ColliderSet(void);
	virtual ~i3ColliderSet(void);

	void				AddCollider( i3Collider *	pAttr);
	void				RemoveCollider( i3Collider * pAttr);
	void				RemoveAllColliders(void);
	INT32				GetColliderCount(void)					{ return (INT32)m_ColList.size(); }
	i3Collider *		GetCollider( INT32 idx)					{ return m_ColList[idx]; }

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

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
