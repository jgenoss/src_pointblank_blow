#include "i3OptPCH.h"
#include "i3OptChangeShapeSetValue.h"

//#include "i3PhysixDefine.h"

I3_CLASS_INSTANCE( i3OptChangeShapeSetValue);

i3OptChangeShapeSetValue::i3OptChangeShapeSetValue(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

bool i3OptChangeShapeSetValue::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::same_of<i3Body*>(pNode))
	{
		i3Body * pBody = (i3Body*)pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);

			if( pShapeSet != nullptr )
			{
				const char * pszName = pShapeSet->GetName();

				i3PhysixJointInfoD6 * pJointInfo = (i3PhysixJointInfoD6*) pShapeSet->getJointInfo();

				switch( pszName[0])
				{
				case 'H':			
					if( pszName[1] == '_')
					{
						if( pszName[ 5 ] != 'H' )
						{			// Head
							pShapeSet->setRestitution( 0.f);
							pShapeSet->setFriction( 0.01f, 0.01f);
							pShapeSet->setAngularDamping( 0.4f);
							pShapeSet->setSleepLinearVelocity( 3550.f);
							pShapeSet->setSleepAngularVelocity( 3000.f);
							pShapeSet->setSkinWidth( 0.03f);
							pShapeSet->setMass( 0.05f );
							

							//Joint
							if( pJointInfo != nullptr)
							{
								pJointInfo->swing1Limit.value			= 0.34f;//0.34f;
								pJointInfo->swing1Limit.restitution		= 0.004f;
								pJointInfo->swing1Limit.spring			= 10.f;
								pJointInfo->swing1Limit.damping			= 0.8f;//0.4f;

								pJointInfo->swing2Limit.value			= 0.35f;
								pJointInfo->swing2Limit.restitution		= 0.004f;
								pJointInfo->swing2Limit.spring			= 10.f;
								pJointInfo->swing2Limit.damping			= 0.9f; //0.5f;

								pJointInfo->twistLimit.high.value		= 0.35f;
								pJointInfo->twistLimit.high.restitution	= 0.003f;
								pJointInfo->twistLimit.high.spring		= 5.f;
								pJointInfo->twistLimit.high.damping		= 0.5f;//0.3f;

								pJointInfo->twistLimit.low.value		= -0.35f;
								pJointInfo->twistLimit.low.restitution	= 0.003f;
								pJointInfo->twistLimit.low.spring		= 5.f;
								pJointInfo->twistLimit.low.damping		= 0.5f;//0.3f;
							}
						}
					}			
					else if( pszName[1] == 'N')		// Hand
					{
						pShapeSet->setRestitution( 0.f);
						pShapeSet->setFriction( 0.3f, 0.1f);
						pShapeSet->setAngularDamping( 0.5f);
						pShapeSet->setSleepLinearVelocity( 3550.f);
						pShapeSet->setSleepAngularVelocity( 3000.f);
						pShapeSet->setSkinWidth( 0.02f);
						pShapeSet->setMass( 0.1f );

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->swing1Limit.value			= 0.05f;
							pJointInfo->swing1Limit.restitution		= 0.8f;
							pJointInfo->swing1Limit.spring			= 1.f;
							pJointInfo->swing1Limit.damping			= 0.7f;

							pJointInfo->swing2Limit.value			= 0.05f;
							pJointInfo->swing2Limit.restitution		= 0.8f;
							pJointInfo->swing2Limit.spring			= 1.f;
							pJointInfo->swing2Limit.damping			= 0.7f;

							pJointInfo->twistLimit.high.value		= 0.018f;
							pJointInfo->twistLimit.high.restitution	= 0.8f;
							pJointInfo->twistLimit.high.spring		= 1.f;
							pJointInfo->twistLimit.high.damping		= 0.7f;

							pJointInfo->twistLimit.low.value		= -0.006f;
							pJointInfo->twistLimit.low.restitution	= 0.8f;
							pJointInfo->twistLimit.low.spring		= 1.f;
							pJointInfo->twistLimit.low.damping		= 0.7f;
						}
					}
					break;

				// Body
				case 'B':
					{
						if( pszName[2] == 'S' )
						{
							if( pszName[7] == '1')
							{	
								pShapeSet->setFriction( 3.5f, 3.2f);
								pShapeSet->setSleepAngularVelocity( 3000.f);
								pShapeSet->setMass( 6.5f);
								pShapeSet->setLinearDamping( 0.001f);

								//Joint
								if( pJointInfo != nullptr)
								{
									pJointInfo->swing1Limit.value			= 0.0005f;
									pJointInfo->swing1Limit.restitution		= 0.8f;
									pJointInfo->swing1Limit.spring			= 30000.f;
									pJointInfo->swing1Limit.damping			= 0.05f;

									pJointInfo->swing2Limit.value			= 0.0005f;
									pJointInfo->swing2Limit.restitution		= 0.95f;
									pJointInfo->swing2Limit.spring			= 50000.f;
									pJointInfo->swing2Limit.damping			= 0.03f;

									pJointInfo->twistLimit.high.value		= 0.0007f;
									pJointInfo->twistLimit.high.restitution	= 0.98f;
									pJointInfo->twistLimit.high.spring		= 50000.f;
									pJointInfo->twistLimit.high.damping		= 0.03f;

									pJointInfo->twistLimit.low.value		= -0.0007f;
									pJointInfo->twistLimit.low.restitution	= 0.98f;
									pJointInfo->twistLimit.low.spring		= 50000.f;
									pJointInfo->twistLimit.low.damping		= 0.03f;
								}
							}
							else if( pszName[7] == '2')
							{
								pShapeSet->setFriction( 2.1f, 1.8f);
								pShapeSet->setSleepAngularVelocity( 1000.f);
								pShapeSet->setMass( 4.f);
								pShapeSet->setLinearDamping( 0.002f);

								//Joint
								if( pJointInfo != nullptr)
								{
									pJointInfo->swing1Limit.value			= 0.0004f;
									pJointInfo->swing1Limit.restitution		= 0.8f;
									pJointInfo->swing1Limit.spring			= 10000.f;
									pJointInfo->swing1Limit.damping			= 0.01f;

									pJointInfo->swing2Limit.value			= 0.0004f;
									pJointInfo->swing2Limit.restitution		= 0.98f;
									pJointInfo->swing2Limit.spring			= 10000.f;
									pJointInfo->swing2Limit.damping			= 0.01f;

									pJointInfo->twistLimit.high.value		= 0.0007f;
									pJointInfo->twistLimit.high.restitution	= 0.98f;
									pJointInfo->twistLimit.high.spring		= 70000.f;
									pJointInfo->twistLimit.high.damping		= 0.02f;

									pJointInfo->twistLimit.low.value		= -0.0007f;
									pJointInfo->twistLimit.low.restitution	= 0.98f;
									pJointInfo->twistLimit.low.spring		= 70000.f;
									pJointInfo->twistLimit.low.damping		= 0.02f;
								}
							}
							else if( pszName[7] == '3')
							{
								pShapeSet->setFriction( 1.7f, 1.4f);
								pShapeSet->setSleepAngularVelocity( 1000.f);
								pShapeSet->setMass( 2.1f);
								pShapeSet->setLinearDamping( 0.003f);

								//Joint
								if( pJointInfo != nullptr)
								{
									pJointInfo->swing1Limit.value			= 0.04f;
									pJointInfo->swing1Limit.restitution		= 0.8f;
									pJointInfo->swing1Limit.spring			= 30000.f;
									pJointInfo->swing1Limit.damping			= 0.01f;

									pJointInfo->swing2Limit.value			= 0.04f;
									pJointInfo->swing2Limit.restitution		= 0.98f;
									pJointInfo->swing2Limit.spring			= 30000.f;
									pJointInfo->swing2Limit.damping			= 0.01f;

									pJointInfo->twistLimit.high.value		= 0.09f;
									pJointInfo->twistLimit.high.restitution	= 0.98f;
									pJointInfo->twistLimit.high.spring		= 20000.f;
									pJointInfo->twistLimit.high.damping		= 0.02f;

									pJointInfo->twistLimit.low.value		= -0.09f;
									pJointInfo->twistLimit.low.restitution	= 0.98f;
									pJointInfo->twistLimit.low.spring		= 20000.f;
									pJointInfo->twistLimit.low.damping		= 0.03f;
								}
							}
							pShapeSet->setSkinWidth( 0.03f);
						}
						else if( pszName[2] == 'N')
						{	// Neck
							pShapeSet->setFriction( 1.f, 0.3f);
							pShapeSet->setSleepAngularVelocity( 1000.f);
							pShapeSet->setAngularDamping( 0.5f);
							pShapeSet->setSkinWidth( 0.02f);
							pShapeSet->setLinearDamping( 0.001f);
							pShapeSet->setMass( 1.f);

							//Joint
							if( pJointInfo != nullptr)
							{
								pJointInfo->twistLimit.high.value		= 0.02f;
								pJointInfo->twistLimit.high.restitution	= 0.01f;
								pJointInfo->twistLimit.high.spring		= 100.f;
								pJointInfo->twistLimit.high.damping		= 0.08f;

								pJointInfo->twistLimit.low.value		= -0.07f;
								pJointInfo->twistLimit.low.restitution	= 0.01f;
								pJointInfo->twistLimit.low.spring		= 100.f;
								pJointInfo->twistLimit.low.damping		= 0.08f;
							}
						}
						else if( pszName[2] == 'R' && pszName[3] == 'o')
						{	// Root
							pShapeSet->setSkinWidth( 0.04f);
							pShapeSet->setFriction( 4.f, 3.6f);
							pShapeSet->setSleepAngularVelocity( 1000.f);
							pShapeSet->setAngularDamping( 0.05f);
							pShapeSet->setMass( 10.f);
						}
						else if( pszName[2] == 'R' || pszName[2] == 'L')
						{	// Clavicle
							pShapeSet->setFriction( 2.f, 1.6f);
							pShapeSet->setSleepAngularVelocity( 1000.f);
							pShapeSet->setAngularDamping( 0.8f);
							pShapeSet->setSkinWidth( 0.03f);
							pShapeSet->setLinearDamping( 0.f);
							pShapeSet->setMass( 1.f);

							//Joint
							if( pJointInfo != nullptr)
							{
								pJointInfo->twistLimit.low.value		= -0.003f;
								pJointInfo->twistLimit.low.restitution	= 0.5f;
								pJointInfo->twistLimit.low.spring		= 1000.f;
								pJointInfo->twistLimit.low.damping		= 0.9f;
							}
						}

						pShapeSet->setRestitution( 0.f);
						pShapeSet->setSleepLinearVelocity( 3500.f);
					}
					break;

				// Arm
				case 'A':
					if( pszName[1] == 'U')
					{	// Upper
						pShapeSet->setFriction( 1.5f, 1.2f);
						pShapeSet->setAngularDamping( 0.7f);
						pShapeSet->setSleepAngularVelocity( 13585.f);
						pShapeSet->setMass( 0.7f);

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->swing1Limit.value			= 1.f;
							pJointInfo->swing1Limit.restitution		= 0.85f;
							pJointInfo->swing1Limit.spring			= 10000.f;
							pJointInfo->swing1Limit.damping			= 0.02f;

							pJointInfo->swing2Limit.value			= 0.9f;
							pJointInfo->swing2Limit.restitution		= 0.95f;
							pJointInfo->swing2Limit.spring			= 10000.f;
							pJointInfo->swing2Limit.damping			= 0.01f;

							pJointInfo->twistLimit.high.value		= 0.15f;
							pJointInfo->twistLimit.high.restitution	= 0.85f;
							pJointInfo->twistLimit.high.spring		= 8000.f;
							pJointInfo->twistLimit.high.damping		= 0.01f;

							pJointInfo->twistLimit.low.value		= -0.95f;
							pJointInfo->twistLimit.low.restitution	= 0.85f;
							pJointInfo->twistLimit.low.spring		= 8000.f;
							pJointInfo->twistLimit.low.damping		= 0.01f;
						}
					}
					else if( pszName[1] == 'D')
					{	// fore
						pShapeSet->setFriction( 1.1f, 0.8f);
						pShapeSet->setAngularDamping( 0.05f);
						pShapeSet->setSleepAngularVelocity( 1000.f);
						pShapeSet->setMass( 0.5f);

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->twistLimit.high.value		= 0.5f;
							pJointInfo->twistLimit.high.restitution	= 0.7f;
							pJointInfo->twistLimit.high.spring		= 500.f;
							pJointInfo->twistLimit.high.damping		= 0.01f;

							pJointInfo->twistLimit.low.value		= -1.f;
							pJointInfo->twistLimit.low.restitution	= 0.6f;
							pJointInfo->twistLimit.low.spring		= 500.f;
							pJointInfo->twistLimit.low.damping		= 0.01f;
						}
					}

					pShapeSet->setSkinWidth( 0.02f);
					pShapeSet->setRestitution( 0.f);
					pShapeSet->setSleepLinearVelocity( 3500.f);
					break;

				// Leg
				case 'L':
					if( pszName[1] == 'U')
					{
						pShapeSet->setFriction( 3.2f, 2.8f);
						pShapeSet->setSleepAngularVelocity( 3000.f);
						pShapeSet->setSkinWidth( 0.03f);
						pShapeSet->setMass( 5.f);

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->swing1Limit.value			= 0.7f;
							pJointInfo->swing1Limit.restitution		= 0.95f;
							pJointInfo->swing1Limit.spring			= 40000.f;
							pJointInfo->swing1Limit.damping			= 0.04f;

							pJointInfo->swing2Limit.value			= 0.2f;
							pJointInfo->swing2Limit.restitution		= 0.9f;
							pJointInfo->swing2Limit.spring			= 40000.f;
							pJointInfo->swing2Limit.damping			= 0.04f;

							pJointInfo->twistLimit.high.value		= 0.005f;
							pJointInfo->twistLimit.high.restitution	= 1.f;
							pJointInfo->twistLimit.high.spring		= 90000.f;
							pJointInfo->twistLimit.high.damping		= 0.02f;

							pJointInfo->twistLimit.low.value		= -0.005f;
							pJointInfo->twistLimit.low.restitution	= 1.f;
							pJointInfo->twistLimit.low.spring		= 90000.f;
							pJointInfo->twistLimit.low.damping		= 0.02f;
						}
					}
					else if( pszName[1] == 'D')
					{	
						pShapeSet->setFriction( 2.6f, 2.2f);
						pShapeSet->setSleepAngularVelocity( 1000.f);
						pShapeSet->setSkinWidth( 0.02f);
						pShapeSet->setMass( 3.f);

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->twistLimit.high.value		= 0.f;
							pJointInfo->twistLimit.high.restitution = 0.f;
							pJointInfo->twistLimit.high.spring		= 0.f;
							pJointInfo->twistLimit.high.damping		= 0.f;

							pJointInfo->twistLimit.low.value		= -1.4f;
							pJointInfo->twistLimit.low.restitution	= 1.f;
							pJointInfo->twistLimit.low.spring		= 1000.f;
							pJointInfo->twistLimit.low.damping		= 0.01f;
						}
					}
					
					pShapeSet->setAngularDamping( 0.05f);
					pShapeSet->setSleepLinearVelocity( 1400.f);
					pShapeSet->setRestitution( 0.f);
					break;

				case 'F':	// Foot
					{
						pShapeSet->setSkinWidth( 0.02f);
						pShapeSet->setRestitution( 0.f);
						pShapeSet->setFriction( 1.5f, 1.f);
						pShapeSet->setAngularDamping( 0.5f);
						pShapeSet->setSleepLinearVelocity( 3500.f);
						pShapeSet->setSleepAngularVelocity( 1000.f);
						pShapeSet->setMass( 1.f);

						//Joint
						if( pJointInfo != nullptr)
						{
							pJointInfo->twistLimit.high.value		= 0.25f;
							pJointInfo->twistLimit.high.restitution	= 0.9f;
							pJointInfo->twistLimit.high.spring		= 10000.f;
							pJointInfo->twistLimit.high.damping		= 0.01f;

							pJointInfo->twistLimit.low.value		= -0.25f;
							pJointInfo->twistLimit.low.restitution	= 0.9f;
							pJointInfo->twistLimit.low.spring		= 10000.f;
							pJointInfo->twistLimit.low.damping		= 0.01f;
						}
					}
					break;

				default:
					break;
				}
			}
		}
	}
	else
	{
		i3Transform2 * pTrans = (i3Transform2 *)i3Scene::FindNodeByExactType( pNode, i3Transform2::static_meta());
		if( pTrans != nullptr )
		{
			i3PhysixShapeSet * pShapeSet = pTrans->getShapeSet();

			if( pShapeSet != nullptr )
				pShapeSet->setFriction( 1.f, 0.8f);
		}
	}

	return i3SceneOptimizer::OnNode( pNode, pParent);
}