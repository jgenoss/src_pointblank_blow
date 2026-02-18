#if !defined( __I3_OPT_BUILD_HULL_H)
#define __I3_OPT_BUILD_HULL_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildHull : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildHull, i3SceneOptimizer);
protected:
	bool				m_bLocalHull = false;
	bool				m_bDynamic = true;
	REAL32				m_Elasticity = 0.3f;
	REAL32				m_sFriction = 0.3f;
	REAL32				m_dFriction = 0.3f;
	REAL32				m_LinearDamping = 0.1f;
	REAL32				m_AngularDamping = 0.1f;
	REAL32				m_Mass = 0.5f;
	INT32				m_Group = 0;

protected:
	void		_BuildGlobalHull(void);
	void		_BuildLocalHull(void);
	void		_OnNodeGlobal( i3Node * pNode);
	void		_OnNodeLocal( i3Node * pNode);
	
	i3Transform *			_FindReplaceTransform( i3Geometry * pGeo);

public:
	i3OptBuildHull(void);

	i3PhysixShapeSet *		CreateConvexHull( i3Node * pNode);
	i3PhysixShapeSet *		CreateConvex( i3Vector3Array * pVA);

	bool		getLocalHullMode(void)				{ return m_bLocalHull; }
	void		setLocalHullMode( bool bFlag)		{ m_bLocalHull = bFlag; }

	bool		getDynamicMode(void)				{ return m_bDynamic; }
	void		setDynamicMode( bool bFlag)			{ m_bDynamic = bFlag; }

	REAL32		getElasticity(void)					{ return m_Elasticity; }
	void		setElasticity( REAL32 val)			{ m_Elasticity = val; }

	REAL32		getStaticFriction(void)				{ return m_sFriction; }
	void		setStaticFriction( REAL32 val)		{ m_sFriction = val; }

	REAL32		getDynamicFriction(void)			{ return m_dFriction; }
	void		setDynamicFriction( REAL32 val)		{ m_dFriction = val; }

	REAL32		getMass(void)						{ return m_Mass; }
	void		setMass( REAL32 mass)				{ m_Mass = mass; }

	REAL32		getLinearDamping(void)				{ return m_LinearDamping; }
	void		setLinearDamping( REAL32 val)		{ m_LinearDamping = val; }
	
	REAL32		getAngularDamping(void)				{ return m_AngularDamping; }
	void		setAngularDamping( REAL32 val)		{ m_AngularDamping = val; }

	INT32		getShapeGroup(void)					{ return m_Group; }
	void		setShapeGroup( INT32 grp)			{ m_Group = grp; }

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pNode) override;
};

#endif
