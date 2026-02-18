#include "i3PhysicsType.h"
#include "i3PhysicsParticle.h"
#include "i3Spring.h"
#include "i3ClothObject.h"
#include "i3HairObject.h"
#include "i3Rigidbody.h"
#include "i3PhysicsAdvanceBone.h"
#include "i3PhysicsIK.h"
#include "i3PhysicsContext.h"

I3_CLASS_INSTANCE( i3PhysicsContext, i3ElementBase );

i3PhysicsContext::i3PhysicsContext()
{
}

i3PhysicsContext::~i3PhysicsContext()
{
	_RemoveAllParticleObj();
	_RemoveAllSpringObj();
	_RemoveAllClothObj();
	_RemoveAllHairObj();
	_RemoveAllRigidbodyObj();
	_RemoveAllIK();
}

void i3PhysicsContext::_RemoveAllParticleObj(void)
{
	INT32 i;
	i3PhysicsParticle * pObj;

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getParticle( i);

		pObj->Release();
	}

	m_ParticleObjList.Clear();
}

void i3PhysicsContext::_RemoveAllSpringObj(void)
{
	INT32 i;
	i3Spring * pObj;

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getSpring( i );

		pObj->Release();
	}

	m_SpringObjList.Clear();
}

void i3PhysicsContext::_RemoveAllClothObj(void)
{
	INT32 i;
	i3ClothObject * pObj;

	for( i = 0; i < getClothCount(); i++ )
	{
		pObj = getCloth( i );

		pObj->Release();
	}

	m_ClothObjList.Clear();
}

void i3PhysicsContext::_RemoveAllHairObj(void)
{
	INT32 i;
	i3HairObject * pObj;

	for( i = 0; i < getHairCount(); i++ )
	{
		pObj = getHair( i );

		pObj->Release();
	}

	m_HairObjList.Clear();

}

void i3PhysicsContext::_RemoveAllRigidbodyObj(void)
{
	INT32 i;
	i3Rigidbody * pObj;

	for( i = 0; i < getRigidbodyCount(); i++ )
	{
		pObj = getRigidbody( i );

		pObj->Release();
	}

	m_RigidbodyList.Clear();
}

void i3PhysicsContext::_RemoveAllIK(void)
{
	INT32 i;
	i3PhysicsIK * pObj;

	for( i = 0; i < getIKCount(); i++ )
	{
		pObj = getIK( i );

		pObj->Release();
	}

	m_IKList.Clear();
}
	
void i3PhysicsContext::AddPhysicsObj( void * pObj, _I3_PHYSICS_OBJTYPE type )
{
	if( type == I3_PHYSICS_OBJTYPE_PARTICLE )
	{
		((i3PhysicsParticle *)pObj)->AddRef();

		m_ParticleObjList.Add( pObj );
	}
	else if( type == I3_PHYSICS_OBJTYPE_SPRING )
	{
		((i3Spring *)pObj)->AddRef();

		m_SpringObjList.Add( pObj );
	}
	else if( type == I3_PHYSICS_OBJTYPE_CLOTH )
	{
		((i3ClothObject *)pObj)->AddRef();

		m_ClothObjList.Add( pObj );
	}
	else if( type == I3_PHYSICS_OBJTYPE_HAIR )
	{
		((i3HairObject *)pObj)->AddRef();

		m_HairObjList.Add( pObj );
	}
	else if( type == I3_PHYSICS_OBJTYPE_RIGIDBODY )
	{
		((i3Rigidbody *)pObj)->AddRef();

		m_RigidbodyList.Add( pObj );
	}
	else if( type == I3_PHYSICS_OBJTYPE_IK )
	{
		((i3PhysicsIK *)pObj)->AddRef();

		m_IKList.Add( pObj );
	}
}

void i3PhysicsContext::RemovePhysicsObj( void * pObj )
{

}

void i3PhysicsContext::RemoveAllPhysicsObj(void)
{
	_RemoveAllParticleObj();
	_RemoveAllSpringObj();
	_RemoveAllClothObj();
	_RemoveAllHairObj();
	_RemoveAllRigidbodyObj();
}

void i3PhysicsContext::Reset(void)
{
}

void i3PhysicsContext::CalcParticle( RT_REAL32 dTime )
{
	INT32 i;
	i3PhysicsParticle * pObj;

	for( i = 0; i < getParticleCount(); i++)
	{
		pObj = getParticle( i );

		if( !(pObj->getStyle() & I3_PHYSICS_PARAM_LOCKED) )
		{
			if( pObj->getAccumTime() >= pObj->getCalcTime() )
			{
				pObj->CalcSimulation( pObj->getCalcTime() );

				pObj->setAccumTime( pObj->getAccumTime() - pObj->getCalcTime() );
			}
			else
				pObj->addAccumTime( dTime );
		}
	}
}

void i3PhysicsContext::CalcSpring( RT_REAL32 dTime )
{
	INT32 i;
	i3Spring * pObj;

	for( i = 0; i < getSpringCount(); i++)
	{
		pObj = getSpring( i );

		pObj->CalcSimulation();
	}
}

void i3PhysicsContext::CalcCloth( RT_REAL32 dTime )
{
	INT32 i;
	i3ClothObject * pObj;

	for( i = 0; i < getClothCount(); i++)
	{
		pObj = getCloth( i );

		pObj->StepSimulation( dTime );
	}
}

void i3PhysicsContext::CalcHair( RT_REAL32 dTime )
{
	INT32 i;

	for( i = 0; i < getHairCount(); i++)
	{
	}
}

void i3PhysicsContext::CalcRigidbody( RT_REAL32 dTime )
{
	INT32 i;
	i3Rigidbody * pObj;


	for( i = 0; i < getRigidbodyCount(); i++)
	{
		pObj = getRigidbody( i );

		pObj->StepSimulation( dTime );
	}
}

void i3PhysicsContext::CalcIK( RT_REAL32 dTime )
{
	INT32 i;
	i3PhysicsIK * pObj;

	for( i = 0; i < getIKCount(); i++ )
	{
		pObj = getIK( i );

		pObj->StepSimulation( dTime );
	}
}

void i3PhysicsContext::OnUpdate( RT_REAL32 dTime )
{
	if( m_Style & I3_PHYSICS_PLAY_ALL )
	{
		CalcParticle( dTime );
		CalcSpring( dTime );
		CalcCloth( dTime );
		CalcHair( dTime );
		CalcRigidbody( dTime );
		CalcIK( dTime );
	}
	else
	{
		if( m_Style & I3_PHYSICS_PLAY_PARTICLE )
			CalcParticle( dTime );

		if( m_Style & I3_PHYSICS_PLAY_SPRING )
			CalcSpring( dTime );

		if( m_Style & I3_PHYSICS_PLAY_CLOTH )
			CalcCloth( dTime );

		if( m_Style & I3_PHYSICS_PLAY_HAIR )
			CalcHair( dTime );

		if( m_Style & I3_PHYSICS_PLAY_RIGIDBODY )
			CalcRigidbody( dTime );

		if( m_Style & I3_PHYSICS_PLAY_IK )
			CalcIK( dTime );
	}
}
