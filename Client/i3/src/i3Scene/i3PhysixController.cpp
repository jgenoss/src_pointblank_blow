#include "i3SceneDef.h"
#include "i3PhysixController.h"
#include "i3PhysixContext.h"

I3_CLASS_INSTANCE( i3PhysixController);

///////////////////////////////////////////////////////////////////////////////
//
class _i3NxControllerHitReport : public NxUserControllerHitReport
{
	public:

	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		i3PhysixController * pCtrl = (i3PhysixController *) hit.controller->getUserData();

		return pCtrl->OnShapeHit( &hit);
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		i3PhysixController * pCtrl = (i3PhysixController *) hit.controller->getUserData();

		return pCtrl->OnControllerHit( &hit);
	}
} __i3ControllerHitReport;
//
/////////////////////////////////////////////////////////////////////////////////

i3PhysixController::~i3PhysixController(void)
{
	if((m_pController != nullptr) && (g_pPhysixCtx != nullptr))
	{
		NxActor * pActor = m_pController->getActor();
		INT32 i;
		NxShape * const * ppShapes = pActor->getShapes();

		for( i = 0; i < (INT32) pActor->getNbShapes(); i++)
		{
			ppShapes[i]->userData = nullptr;
		}

		pActor->userData = nullptr;

		g_pPhysixCtx->CMD_releaseController( m_pController);
		m_pController = nullptr;
	}
}

bool i3PhysixController::Create( INT32 group, NxControllerDesc * pDesc)
{
	I3ASSERT( pDesc->isValid());

	if( m_pController == nullptr)
	{
		pDesc->interactionFlag = NXIF_INTERACTION_USE_FILTER;
		pDesc->callback = &__i3ControllerHitReport;
		pDesc->userData = this;
		pDesc->upDirection	= NX_Y;

		m_pController = g_pPhysixCtx->createController( pDesc);
		I3ASSERT( m_pController != nullptr);
		I3ASSERT( m_pController->getActor() != nullptr);

		m_pController->getActor()->setName("__NxContoller");
		m_pController->getActor()->userData = this;
	}
	else
	{
		I3ASSERT( m_pController->getActor() != nullptr);
		NxCapsuleControllerDesc * pCapsuleDesc = (NxCapsuleControllerDesc*) pDesc;

		NxShape * const * ppShape = getActor()->getShapes();

		for( UINT32 i = 0; i < getActor()->getNbShapes(); i++)
		{	
			NxCapsuleShape* pCapsule = ppShape[i]->isCapsule();
			
			if(pCapsule == nullptr)
			{
				continue;
			}

			pCapsule->setRadius( pCapsuleDesc->radius);
			pCapsule->setHeight( pCapsuleDesc->height);
			pCapsule->setSkinWidth( pCapsuleDesc->skinWidth);

			NxVec3 vec( (NxReal)pCapsuleDesc->position.x, (NxReal)pCapsuleDesc->position.y, (NxReal)pCapsuleDesc->position.z);
			pCapsule->setLocalPosition( vec);
		}
	}

	setGroup( group);

	return true;
}

void i3PhysixController::setGroup( INT32 group)
{
	m_Group = group;
	m_DestGroupMask = g_pPhysixCtx->getCollidableDestMask( group);

	if( m_pController != nullptr)
	{
		// Controller가 내부적으로 가지고 있는 Actor의 Shape들도
		// Group을 변경해 주어야 한다.
		NxActor * pActor = m_pController->getActor();
		pActor->setGroup( (NxActorGroup) group);
		I3ASSERT( pActor != nullptr);

		UINT32 i;
		NxShape * const * ppShape = pActor->getShapes();

		for( i = 0; i < pActor->getNbShapes(); i++)
		{
			ppShape[i]->setGroup( (NxCollisionGroup) group);
		}
	}
}

void i3PhysixController::setEventReceiver( i3EventReceiver * pObj)
{
	m_pEventReceiver = pObj;

	if( m_pController != nullptr)
	{
		// Controller가 내부적으로 가지고 있는 Actor의 Shape들도
		// Userdata를 변경해 주어야 한다.
		NxActor * pActor = m_pController->getActor();
		
		I3ASSERT( pActor != nullptr);

		UINT32 i;
		NxShape * const * ppShape = pActor->getShapes();

		for( i = 0; i < pActor->getNbShapes(); i++)
		{
			ppShape[i]->userData = pObj;
		}
	}
}

void i3PhysixController::Move( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness)
{
	I3ASSERT( m_pController != nullptr);

	NxVec3 disp;

	disp.x = pMoveVec->x;
	disp.y = pMoveVec->y;
	disp.z = pMoveVec->z;

	//if( i3Math::isValid( pMoveVec->x) == false)
	//{
	//	int* iii = nullptr;
	//	*iii = 10;
	//}
	//if( i3Math::isValid( pMoveVec->y) == false)
	//{
	//	int* iii = nullptr;
	//	*iii = 10;
	//}
	//if( i3Math::isValid( pMoveVec->z) == false)
	//{
	//	int* iii = nullptr;
	//	*iii = 10;
	//}


	m_pController->move( disp, m_DestGroupMask, minDist, *pHintFlag, sharpness);
}

NxControllerAction	i3PhysixController::OnShapeHit( const NxControllerShapeHit * pHit)
{
	return NX_ACTION_NONE;
}

NxControllerAction	i3PhysixController::OnControllerHit( const NxControllersHit * pHit)
{
	return NX_ACTION_PUSH;
}

void i3PhysixController::getPosition( VEC3D * pPos)
{
	if( m_pController == nullptr )	return;
	NxExtendedVec3 pos;
	REAL32 rCenterHeight = 0;

	if( m_pController->getType() == NX_CONTROLLER_CAPSULE )
	{
		
		rCenterHeight = (((NxCapsuleController*)m_pController)->getHeight() * 0.5f) + ((NxCapsuleController*)m_pController)->getRadius();
		// rCenterHeight = m_CenterHeight;
	}
	else
	{
		rCenterHeight = m_CenterHeight;
	}
	

	pos = m_pController->getPosition();
	pPos->x = (REAL32) pos.x;
	pPos->y = (REAL32) pos.y - rCenterHeight;
	pPos->z = (REAL32) pos.z;
}

void i3PhysixController::setPosition( VEC3D * pPos)
{
	if( m_pController == nullptr )		return;

	NxExtendedVec3 pos;
	REAL32 rCenterHeight = 0;

	if( m_pController->getType() == NX_CONTROLLER_CAPSULE )
	{
		
		rCenterHeight = (((NxCapsuleController*)m_pController)->getHeight() * 0.5f) + ((NxCapsuleController*)m_pController)->getRadius();
		// rCenterHeight = m_CenterHeight;
	}
	else
	{
		rCenterHeight = m_CenterHeight;
	}

	pos.x = pPos->x;
	pos.y = pPos->y + rCenterHeight;
	pos.z = pPos->z;

	m_pController->setPosition( pos);
}

void i3PhysixController::SetCenterHeightAndHeight( REAL32 rCenterHeight, REAL32 rHeight)
{
	m_CenterHeight = rCenterHeight;

	if( m_pController->getType() == NX_CONTROLLER_BOX )
	{
		return;
	}
	else
	{
		NxShape * const * ppShape = getActor()->getShapes();

		for( UINT32 i = 0; i < getActor()->getNbShapes(); i++)
		{	
			NxCapsuleShape* pCapsule = ppShape[i]->isCapsule();
			
			if(pCapsule == nullptr)
			{
				continue;
			}

			pCapsule->setHeight(rHeight);
		}
	}
}

//void i3PhysixController::SetPositionFromCapsule( VEC3D * pPos, REAL32 rRadius)
//{
//	if( m_pController == nullptr )		return;
//
//	NxExtendedVec3 pos;]
//	REAL32 rCenterHeight = (rHeight * 0.5f) + RADIUS
//
//	pos.x = pPos->x;
//	pos.y = pPos->y + m_CenterHeight;
//	pos.z = pPos->z;
//
//	m_pController->setPosition( pos);
//}

void i3PhysixController::MakeCapsuleTrigger()
{
	if( m_bEventTrigger == false)
	{
		I3ASSERT(m_pController);

		if(m_pController->getType() != NX_CONTROLLER_CAPSULE )
		{
			return;
		}

		NxCapsuleController* pCapsuleCtrl = (NxCapsuleController*)m_pController;

		NxCapsuleShapeDesc nxDesc;

		nxDesc.radius = pCapsuleCtrl->getRadius();
		// nxDesc.radius = 0.36f;
		nxDesc.height = pCapsuleCtrl->getHeight();
		// nxDesc.skinWidth = 0.025f;
		nxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
			
		getActor()->createShape(nxDesc);

		m_bEventTrigger = true;
	}
}

