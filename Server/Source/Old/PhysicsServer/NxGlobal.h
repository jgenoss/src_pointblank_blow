#ifndef __NxGlobal_H_
#define __NxGlobal_H_

#include "NxuLib\NxuStream2\NXU_Helper.h"
#include "NxuLib\NxuStream2\NXU_String.h"
#include "PhysX\SampleCommonCode\src\Stream.h"
#include "PhysX\SampleCommonCode\src\ErrorStream.h"
#include "PhysX\SampleCommonCode\src\DebugRenderer.h"
#include "PhysX\SampleCommonCode\src\UserAllocator.h"
#include "PhysX\SampleCommonCode\src\SamplesVRDSettings.h"

#include "..\..\source_new\DedicationServer\NxShapeSet.h"


// test
extern NxRaycastHit     gHits[100];


// ------------------------------------------------
// forward declarations
// ------------------------------------------------
class UserAllocator;

// ------------------------------------------------
// global variables
// ------------------------------------------------

// physX sdk
extern	NxPhysicsSDK*		gPhysicsSDK;

// user report
extern DebugRenderer		gDebugRenderer;
extern UserAllocator*		gAllocator;

// ------------------------------------------------
// global functions
// ------------------------------------------------
BOOL	InitNx();
void	ResetNx();
void	ReleaseNx();
BOOL	IsHardwarePresent();

// ------------------------------------------------
// user classes
// ------------------------------------------------

class PxRaycastQueryReport : public NxSceneQueryReport
{
	virtual	NxQueryReportResult	onBooleanQuery(void* userData, bool result){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits){ return NX_SQR_CONTINUE; };
	
	virtual	NxQueryReportResult	onRaycastQuery(void* userData, NxU32 nbHits, const NxRaycastHit* hits)
	{
		unsigned int i = (unsigned int)userData;
		
		if( (size_t)i >= m_nSize )
		{
			DSERROR("[NxQueryReportResult] Invalid Index\n");
			return NX_SQR_ABORT_QUERY;
		}
		
		if ( nbHits > 0) 
		{
			m_pHits[i] = hits[0];
		} else {
			m_pHits[i].shape = NULL;
		}
		return NX_SQR_CONTINUE;
	}
	
	NxRaycastHit*	m_pHits;
	size_t			m_nSize;

public:
	PxRaycastQueryReport()
	{
		m_pHits = NULL;
		m_nSize = 0;
	}

	void SetOutputBuffer( NxRaycastHit* pHits, size_t nSize )
	{
		m_pHits = pHits;
		m_nSize = nSize;
	}
};

class PxUserNotify : public NXU_userNotify
{
public:
	NxShapeSet*				m_pShapeSet;
	INT32					m_MaterialIndex;
	void  *					m_pCookedMesh;
	INT32					m_ShapeGroup;

	PxUserNotify()
	{
		m_pShapeSet			= NULL;
		m_MaterialIndex		= 0;
		m_pCookedMesh		= NULL;
		m_ShapeGroup		= 0;
	}
	
	virtual void NXU_notifyActor( NxActor * actor, const char * userProperties)
	{	
		UINT32 i;
		NxShape * const * ppShape = actor->getShapes();

		m_pShapeSet->setNxActor( actor);
		actor->userData = m_pShapeSet;

		for( i = 0; i < actor->getNbShapes(); i++)
		{
			ppShape[i]->setMaterial( (NxMaterialIndex) m_MaterialIndex);
			ppShape[i]->userData = m_pShapeSet;
		}
	}

	virtual bool	NXU_preNotifyActor(NxActorDesc &actor, const char	*userProperties)
	{
		for (NxU32 i=0; i<actor.shapes.size(); i++)
		{
			NxShapeDesc *s = actor.shapes[i];
			s->shapeFlags|=NX_SF_VISUALIZATION; // make sure the shape visualization flags are on so we can see things!
		}
		return true;
	}
};


#endif