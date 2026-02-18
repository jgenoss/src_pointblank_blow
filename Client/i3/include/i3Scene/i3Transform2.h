#if !defined( __I3_TRANSFORM2_H)
#define __I3_TRANSFORM2_H

#if defined ( I3_PHYSX)

#include "i3Node.h"
#include "i3Transform.h"
#include "i3PhysixDefine.h"
#include "i3PhysixShapeSet.h"

class i3PhysixShapeSet;

/** \brief
	\desc leaf class */

class I3_EXPORT_SCENE i3Transform2 : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3Transform2, i3Transform);
 
protected:
	// Persistent Members
	i3PhysixShapeSet *		m_pShapeSet = nullptr;

	// Volatile Members

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3PhysixShapeSet *	getShapeSet(void)								{ return m_pShapeSet; }
	void				setShapeSet( i3PhysixShapeSet * pShapeSet)
	{ 
		I3_REF_CHANGE( m_pShapeSet, pShapeSet); 
	}

	void	addDynamicState( UINT32 state)
	{
		if( m_pShapeSet != nullptr)
			m_pShapeSet->modifyDynamicState( state, 0);
	}

	void	removeDynamicState( UINT32 state)
	{
		if( m_pShapeSet != nullptr)
			m_pShapeSet->modifyDynamicState( 0, state);
	}

	void	modifyDynamicState( UINT32 addState, UINT32 removeState)
	{
		if( m_pShapeSet != nullptr)
			m_pShapeSet->modifyDynamicState( addState, removeState);
	}

	bool	isDynamicState( UINT32 mask)	{ return (m_pShapeSet != nullptr) && (m_pShapeSet->isDynamicState( mask)); }
	bool	isDynamic(void)					{ return (m_pShapeSet != nullptr) && (m_pShapeSet->isDynamicState( I3_PHYSIX_STATE_KINEMATIC) == false); }

	void	setLinearVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setLinearVelocity( pVec );
	}
	void	setLinearMomentum( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setLinearMomentum( pVec );
	}
	void	setFBLinearVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setFBLinearVelocity( pVec );

	}
	void	addFBLinearVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->addFBLinearVelocity( pVec );
	}

	REAL32	getLinearDamping( void )
	{
		if( m_pShapeSet != nullptr )
			return m_pShapeSet->getLinearDamping();

		return (REAL32) 0xFFFFFFFF;
	}
	void	setLinearDamping( REAL32 fDamp )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setLinearDamping( fDamp );
	}

	void	setAngularVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setAngularVelocity( pVec );
	}
	void	setAngularMomentum( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setAngularMomentum( pVec );
	}
	void	setFBAngularVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setFBAngularVelocity( pVec );
	}
	void	addFBAngularVelocity( VEC3D * pVec )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->addFBAngularVelocity( pVec );
	}

	REAL32	getAngularDamping( void )
	{
		if( m_pShapeSet != nullptr )
			return m_pShapeSet->getAngularDamping();

		return (REAL32) 0xFFFFFFFF;
	}
	void	setAngularDamping( REAL32 fDamp )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setAngularDamping( fDamp );
	}

	void	setSleeping( bool bEnable )
	{
		if( m_pShapeSet != nullptr )
			m_pShapeSet->setSleeping( bEnable );
	}

public:
	i3Transform2(void);
	virtual ~i3Transform2(void);

	virtual void	PreTrace(i3SceneTracer * pTracer) override;
	virtual void	OnUpdate( i3SceneTracer * pTracer) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void	OnInitAfterLoad(void) override;
	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
};

#endif

#endif
