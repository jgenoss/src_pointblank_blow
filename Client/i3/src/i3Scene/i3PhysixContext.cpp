#include "i3SceneDef.h"
#include "i3PhysixContext.h"
#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"
#include "i3PhysixShapeSet.h"

#include "i3ParticleRender.h"

#include "i3PhysixMemManager.h"

#include "NXU_helper.h"
#include "NXU_schema.h"

//#define I3_PHYSIX_HARDWARE
//#define I3_PHYSIX_MULTITHREAD

#if defined( I3_DEBUG)
#define I3_PHYSIX_DEBUG_OUTPUT
#endif

//#define I3_PHYSIX_MEMPOOL
//#define I3_PHS_MM

//#define OWN_PAIR_FILTER

I3_CLASS_INSTANCE( i3PhysixContext);

I3_EXPORT_SCENE i3PhysixContext *	g_pPhysixCtx = nullptr;

static I3_TERRAIN_MATERIAL		s_TerrainMtlTable[] = 
{
	//	정지마찰	운동마찰	탄성계수
	{	0.3f,		0.3f,		0.007f	},	// NA
	{	0.7f,		0.7f,		0.002f	},	// I3_TERRAIN_CONCRETE = 1,				// 콘크리트
	{	0.3f,		0.3f,		0.007f	},	// I3_TERRAIN_STEEL,					// 철제
	{	0.5f,		0.5f,		0.0005f	},	// I3_TERRAIN_GROUND,					// 흙
	{	0.6f,		0.6f,		0.001f	},	// I3_TERRAIN_WOOD,						// 나무
	{	0.6f,		0.6f,		0.0f	},	// I3_TERRAIN_SNOW,						// 눈
	{	0.0f,		0.0f,		0.0f	},	// I3_TERRAIN_WATER_DEEP,				// 물 (깊은)
	{	0.0f,		0.0f,		0.0f	},	// I3_TERRAIN_WATER_SHALLOW,			// 물 (얕은)
	{	0.5f,		0.5f,		0.001f	},	// I3_TERRAIN_WET_CLAY,					// 진흙
	{	0.6f,		0.6f,		0.001f	},	// I3_TERRAIN_GRASS,					// 잔디
	{	0.2f,		0.2f,		0.009f	},	// I3_TERRAIN_MARBLE,					// 대리석
	{	0.7f,		0.7f,		0.001f	},	// I3_TERRAIN_FALLLEAF,
	{	0.7f,		0.7f,		0.002f	},	// I3_TERRAIN_CONCRETE_THIN,
	{	0.3f,		0.3f,		0.007f	},	// I3_TERRAIN_STEEL_THIN,
	{	0.6f,		0.6f,		0.001f	},	// I3_TERRAIN_WOOD_THIN,
	{	0.2f,		0.2f,		0.009f	},	// I3_TERRAIN_MARBLE_THIN,
	{	0.4f,		0.4f,		0.002f	},	// I3_TERRAIN_PAPER,						// 종이
	{	0.1f,		0.1f,		0.008f	},	// I3_TERRAIN_GLASS,						// 유리
	{	0.3f,		0.3f,		0.006f	},	// I3_TERRAIN_PLASTIC,						// 플라스틱
	{	0.9f,		0.9f,		0.001f	},	// I3_TERRAIN_RUBBER,						// 고무
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_CLOTH,						// 천
	{	0.5f,		0.5f,		0.0005f	},	// I3_TERRAIN_GROUND_THIN
	{	0.6f,		0.6f,		0.0f	},	// I3_TERRAIN_SNOW_THIN
	{	0.5f,		0.5f,		0.001f	},	// I3_TERRAIN_WET_CLAY_THIN
	{	0.5f,		0.5f,		0.001f	},	// I3_TERRAIN_GRASS_THIN
	{	0.4f,		0.4f,		0.002f	},	// I3_TERRAIN_PAPER_THIN
	{	0.7f,		0.7f,		0.001f	},	// I3_TERRAIN_FALLLEAF_THIN
	{	0.1f,		0.1f,		0.008f	},	// I3_TERRAIN_GLASS_THIN
	{	0.3f,		0.3f,		0.006f	},	// I3_TERRAIN_PLASTIC_THIN
	{	0.9f,		0.9f,		0.001f	},	// I3_TERRAIN_RUBBER_THIN
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_CLOTH_THIN
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_BARBEDWIRE
	{	0.9f,		0.9f,		0.001f	},	// I3_TERRAIN_BLOOD
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_TEMP1
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_TEMP2
	{	0.7f,		0.7f,		0.0005f	},	// I3_TERRAIN_CLOTH_NO_DECAL
	{	0.3f,		0.3f,		0.007f	},	//I3_TERRAIN_STEEL_WIRE,					
	{	0.6f,		0.6f,		0.001f	},	//I3_TERRAIN_WOOD_WIRE,				
	{	0.3f,		0.3f,		0.006f	},	//I3_TERRAIN_PLASTIC_WIRE,
	{	0.0f,		0.0f,		0.0f },	//I3_TERRAIN_NONE,
};

static ControllerManager*		s_pCtrlManager = nullptr;
static NxScene *				s_pNxScene;

i3PhysixContext::i3PhysixContext(void)
{
	g_pPhysixCtx = this;
	
	m_ConvexList.reserve( 256);
	m_MeshList.reserve( 256);
	
	I3ASSERT( s_pCtrlManager == nullptr);
	s_pCtrlManager = new ControllerManager;
	NetworkDump_Malloc( s_pCtrlManager, sizeof( ControllerManager), __FILE__, __LINE__);
	
	m_pContactPointPool = new i3FixedMemoryPool;
	NetworkDump_Malloc( m_pContactPointPool, sizeof( i3FixedMemoryPool), __FILE__, __LINE__);
}

i3PhysixContext::~i3PhysixContext(void)
{
	if( m_pSDK != nullptr )
	{
		#if defined( I3_DEBUG)
			//this->Dump();
		#endif

		ReleaseScene();

		m_pSDK->release();
		m_pSDK	= nullptr;
	}

	g_pPhysixCtx = nullptr;

	I3MEM_SAFE_FREE( m_pCmdRing);
	I3MEM_SAFE_FREE_POINTER( m_pResultList ); 

	RemovePool();

	#if defined( I3_PHS_MM)
		_PHS_Close();
	#endif

	I3_SAFE_DELETE( s_pCtrlManager);
	I3_SAFE_DELETE( m_pContactPointPool);
}

#if !defined( I3_NO_PROFILE)
static UINT32 s_nPhysix_MemAllocSize = 0;
static UINT32 s_nPhysix_MemAllocCountPerFrame = 0;
static UINT32 s_nPhysix_MemReAllocCountPerFrame = 0;
static UINT32 s_nPhysix_MemFreeCountPerFrame = 0;

static INT64	s_timeAlloc = 0L;
static INT64	s_timeAllocTotal = 0L;
static INT64	s_timeFree = 0L;

static INT64	s_timeRaycast = 0L;
static UINT32	s_Physix_RaycastCount = 0;
#endif

#if defined( I3_PHYSIX_MEMPOOL)
/*
static i3PhysixMemoryPool		s_MemPool[MPT_MAX];

static char s_szMemPoolName[MPT_MAX][32] =
{
	"22",
	"110",
	"282",
	"350"
};
*/
#endif

//  일단, static_pool로 먼저 처리한다..


class MyAllocator : public NxUserAllocator
{
public:
	i3PhysixContext *	m_pCtx;
public:
	virtual void * malloc( NxU32 size) {	I3ASSERT_0;	return nullptr; }

	virtual void * mallocDEBUG( NxU32 size, const char * filename, int line) { I3ASSERT_0; 	return nullptr; }

	// 22	NX_MEMORY_KinematicTarget
	// 110	NX_MEMORY_LowLevelDebug			NX_MEMORY_LowLevel
	// 282	NX_MEMORY_Generic_Array_Container
	// 350	ICE_MEMORY_IceContainer_NewEntries

    void * malloc(NxU32 size, NxMemoryType type)
    {	
		return	i3::static_pool_malloc(size);
    }
    
    void * mallocDEBUG( NxU32 size,const char *fileName, int line, const char * className, NxMemoryType type)
    {
#ifdef _DEBUG
		return	i3::static_pool_malloc_dbg(size, fileName, line);
#else
		return	i3::static_pool_malloc(size);
#endif
    }
    
    void * realloc(void * memory, NxU32 size)
    {
		if (memory == nullptr)
		{
			return i3::static_pool_malloc(size);
		}

		if (size == 0) 
		{
			i3::static_pool_free(memory); return nullptr;
		}

		// copy부분이 필요하다..  
		void* new_p = i3::static_pool_malloc(size);
		::memcpy(new_p, memory, size);					// 좀더 다른값까지 모두 카피되지만..안전할것으로 예측..
		i3::static_pool_free(memory);
		return new_p;
    }
    
    void free(void * memory)
    {
		i3::static_pool_free(memory);
    }
} myAllocator;


/*
class MyAllocator : public NxUserAllocator
{
public:

	i3PhysixContext *	m_pCtx;

public:

	virtual void * malloc( NxU32 size)
	{
		I3ASSERT_0;
		return nullptr;
	}

	virtual void * mallocDEBUG( NxU32 size, const char * filename, int line)
	{
		I3ASSERT_0;
		return nullptr;
	}

	// 22	NX_MEMORY_KinematicTarget
	// 110	NX_MEMORY_LowLevelDebug			NX_MEMORY_LowLevel
	// 282	NX_MEMORY_Generic_Array_Container
	// 350	ICE_MEMORY_IceContainer_NewEntries

    void * malloc(NxU32 size, NxMemoryType type)
    {	
		void * p;
		
		#if !defined( I3_NO_PROFILE )
			INT64 delta;
			INT64 startTime = i3Timer::GetProfileTime();
			s_nPhysix_MemAllocCountPerFrame++;
		#endif

		#if defined( I3_PHYSIX_MEMPOOL)
			INT32 idx = -1;

			if( m_pCtx->isSimulating())
			{
				switch( type)
				{
					case NX_MEMORY_KinematicTarget :			idx = MPT_22;	break;
					case NX_MEMORY_LowLevel :
					case NX_MEMORY_LowLevelDebug :				idx = MPT_110;	break;
					case NX_MEMORY_Generic_Array_Container :	idx = MPT_282;	break;
					case ICE_MEMORY_IceContainer_NewEntries :	idx = MPT_350;	break;
				}

				if( idx != -1)
				{
					p = s_MemPool[idx].Alloc( size);

					#if !defined( I3_NO_PROFILE )
					delta = i3Timer::GetProfileTime() - startTime;
					s_timeAlloc += delta;
					s_timeAllocTotal += delta;
					#endif
					return p;
				}
			}
		#endif

		#if defined( I3_PHS_MM)
			p = _PHS_Alloc( size);
		#else
			p = i3MemAlloc( size);
		#endif

#if !defined( I3_NO_PROFILE)
		delta = i3Timer::GetProfileTime() - startTime;
		s_timeAlloc += delta;
		s_timeAllocTotal += delta;
#endif
		return p;
    }
    
    void * mallocDEBUG( NxU32 size,const char *fileName, int line, const char * className, NxMemoryType type)
    {
		void * p;

		#if !defined( I3_NO_PROFILE )
			INT64 delta;
			INT64 startTime = i3Timer::GetProfileTime();
			s_nPhysix_MemAllocCountPerFrame++;
		#endif

		#if defined( I3_PHYSIX_MEMPOOL)
			INT32 idx = -1;

			if( m_pCtx->isSimulating())
			{
				switch( type)
				{
					case NX_MEMORY_KinematicTarget :			idx = MPT_22;	break;
					case NX_MEMORY_LowLevel :
					case NX_MEMORY_LowLevelDebug :				idx = MPT_110;	break;
					case NX_MEMORY_Generic_Array_Container :	idx = MPT_282;	break;
					case ICE_MEMORY_IceContainer_NewEntries :	idx = MPT_350;	break;
				}

				if( idx != -1)
				{
					p = s_MemPool[idx].Alloc( size);

					#if !defined( I3_NO_PROFILE )
					delta = i3Timer::GetProfileTime() - startTime;
					s_timeAllocTotal += delta;
					#endif

					return p;
				}
			}
		#endif

		//
		#if defined( I3_PHS_MM)
			p = _PHS_Alloc( size);
		#else
			p = i3mem::Alloc( size, 0, I3MEMORY_MINIMUM_ALLOCSIZE, fileName, line);
		#endif

#if !defined( I3_NO_PROFILE)
		delta = i3Timer::GetProfileTime() - startTime;
		s_timeAllocTotal += delta;
#endif
		return p;
    }
    
    void * realloc(void * memory, NxU32 size)
    {
#if !defined( I3_NO_PROFILE)
		INT64 delta;
		INT64 startTime = i3Timer::GetProfileTime();
		s_nPhysix_MemReAllocCountPerFrame++;
#endif

		void * pPointer = nullptr;

		#if defined( I3_PHYSIX_MEMPOOL)
		{
			INT32 i;

			for( i = 0; i < MPT_MAX; i++)
			{
				pPointer = s_MemPool[i].Realloc( (char *) memory, size);

				if( pPointer != nullptr)
				{
					#if !defined( I3_NO_PROFILE )
					delta = i3Timer::GetProfileTime() - startTime;
					s_timeAllocTotal += delta;
					#endif

					return pPointer;
				}
			}
		}
		#endif

        //
		#if defined( I3_PHS_MM)
			pPointer = _PHS_Realloc( memory, size);
		#else
			pPointer = i3MemReAlloc( memory, size);
		#endif

#if !defined( I3_NO_PROFILE)
		delta = i3Timer::GetProfileTime() - startTime;
		s_timeAllocTotal += delta;
#endif

		return pPointer;
    }
    
    void free(void * memory)
    {
		#if !defined( I3_NO_PROFILE)
			INT64 delta;
			INT64 startTime = i3Timer::GetProfileTime();
			s_nPhysix_MemFreeCountPerFrame++;
		#endif

		#if defined( I3_PHYSIX_MEMPOOL)
		INT32 i;

		for( i = 0; i < MPT_MAX; i++)
		{
			if( s_MemPool[i].Free( memory) == true)
			{
				#if !defined( I3_NO_PROFILE)
				delta = i3Timer::GetProfileTime() - startTime;
				s_timeFree += delta;
				#endif
				return;
			}				
		}
		#endif

		#if defined( I3_PHS_MM)
			_PHS_Free( memory);
		#else
			i3MemFree( memory);
		#endif


		#if !defined( I3_NO_PROFILE)
			delta = i3Timer::GetProfileTime() - startTime;
			s_timeFree += delta;
		#endif
    }
} myAllocator;
*/


class MyOutputStream : public NxUserOutputStream
{
    void reportError ( NxErrorCode code, const char *message, const char* file, int line)
    {
        //this should be routed to the application
        //specific error handling. If this gets hit
        //then you are in most cases using the SDK
        //wrong and you need to debug your code!
        //however, code may  just be a warning or
        //information.
		I3TRACE( "[PHYSX] [%08X] %s\n", code, message);
    
        if (code < NXE_DB_INFO)
        {
			//NxSceneStats stat;

			//s_pNxScene->getStats( stat);

			#if defined( I3_DEBUG)
				//g_pPhysixCtx->Dump();
			#endif

			//I3TRACE( "# of pairs : %d\n", stat.numPairs);
			//I3TRACE( "max of pairs : %d\n", stat.maxPairs);
            //I3PRINTLOG(I3LOG_WARN,  "[PHYSX] [%08X] %s", code, message);
        }
    }
            
    NxAssertResponse reportAssertViolation (const char *message, const char *file,int line)
    {
        //this should not get hit by
        // a properly debugged SDK!
        //assert(0);
		I3PRINTLOG(I3LOG_FATAL,  (char*) message );
        return NX_AR_CONTINUE;
    }
            
    void print (const char *message)
    {
        I3TRACE("SDK says: %s\n", message);
    }

} myOutputStream;

class MyContactReport : public NxUserContactReport
{
protected :

	struct NotifyReport
	{
		UINT32				m_Events = 0;
		i3PhysixShapeSet *	m_pOwnShapeSet = nullptr;
		i3PhysixShapeSet *	m_pDestShapeSet = nullptr;
	};

	NotifyReport			m_Report;
//	I3_PHYSIX_CONTACT_REPORT m_Report;

	i3PhysixShapeSet * pShape1, * pShape2;

	UINT32	m_ShapeCount;

	VEC3D	m_vContactNormal;
	VEC3D	m_vContactForce;

public:
	//
	// 현 코드에서 해당 콜백 호출 1회 당 단 1회의 m_Events, m_pOwnShapeSet, m_pDestShapeSet을 받는듯 하며
	// 내부에 n번의 패치가 있을때 2번째부터는 m_Events, m_pOwnShapeSet, m_pDestShapeSet은 nullptr로 초기화된 상태로 계산진행되고, 
	// i3PhysixShapeSet에 적재된다. (현재 이런 방식이 올바른 방식인지 잘 모르겠음.) 2014.10.16.수빈
	//

	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
	{
		bool bDecal = false;
		// Iterate through contact points
		if( pair.actors[0] == nullptr )	return;
		pShape1 = (i3PhysixShapeSet*) pair.actors[0]->userData;
		if( pShape1 == nullptr )			return;
		if( pair.actors[1] == nullptr)		return;
		pShape2 = (i3PhysixShapeSet*) pair.actors[1]->userData;
		if( pShape2 == nullptr )			return;

		if( pShape1->getEventReceiver() != nullptr )
		{
			if( i3::same_of<i3ParticleRender*>(pShape1->getEventReceiver()))
			{	// is decal object.
				pShape1->clearContactReport();
				m_Report.m_Events			= events;
				m_Report.m_pOwnShapeSet	= pShape1;
				m_Report.m_pDestShapeSet	= pShape2;

				bDecal = true;
			}
		}

		if( pShape2->getEventReceiver() != nullptr)
		{	
			if( !bDecal )
			{	// is object.
				pShape2->clearContactReport();
				m_Report.m_Events			= events;
				m_Report.m_pOwnShapeSet	= pShape2;
				m_Report.m_pDestShapeSet	= pShape1;
			}
			else
			{
				if( i3::same_of<i3ParticleRender*>(pShape2->getEventReceiver()))
				{
					return;
				}
			}
		}

		if (!pShape1->isEnableContactReport() && !pShape2->isEnableContactReport() )	// 
			return;

		NxContactStreamIterator iterator(pair.stream);

		bool bShape1Event = false;
		bool bShape2Event = false;
		
		//user can call getNumPairs() here
		while( iterator.goNextPair() )
		{
			//user can also call getShape() and getNumPatches() here
			if( pShape1->getContactCount() < I3PHYSIX_CONTACT_REPORT_COUNT && pShape2->getContactCount() < I3PHYSIX_CONTACT_REPORT_COUNT )
			{	
				m_ShapeCount = 0;
				
				while( iterator.goNextPatch() )
				{
									//if( pShape1->getContactCount() < I3PHYSIX_CONTACT_REPORT_COUNT && pShape2->getContactCount() < I3PHYSIX_CONTACT_REPORT_COUNT )
					{
						I3_PHYSIX_CONTACT_REPORT_PTR new_obj_ptr( new I3_PHYSIX_CONTACT_REPORT );

						I3_PHYSIX_CONTACT_REPORT* pNewReport = new_obj_ptr.get();
						pNewReport->m_Events = m_Report.m_Events;
						pNewReport->m_pOwnShapeSet = m_Report.m_pOwnShapeSet;
						pNewReport->m_pDestShapeSet = m_Report.m_pDestShapeSet;

						//user can also call getPatchNormal() and getNumPoints() here
						const NxVec3& contactNormal = iterator.getPatchNormal();

						m_vContactNormal.x = contactNormal.x; m_vContactNormal.y = contactNormal.y; m_vContactNormal.z = contactNormal.z;

						while( iterator.goNextPoint() )
						{
							//user can also call getPoint() and getSeparation() here
							//if( m_PointCount < I3_PHYSIX_CONTACT_REPORT_MAX_POINT && iterator.getSeparation() < 0.f )
							if( iterator.getSeparation() < 0.f)
							{
								I3_PHYSIX_CONTACT_INFO * pNewInfo = g_pPhysixCtx->AllocContactPoint();
								pNewReport->m_ContactList.push_back(pNewInfo);
							//	m_Report.m_ContactList.push_back( pNewInfo);

								const NxVec3& contactPoint = iterator.getPoint();

								NxVec3 contactForce = ( iterator.getShapeFlags() & NX_SF_POINT_CONTACT_FORCE ) ?  contactNormal * iterator.getPointNormalForce() : pair.sumNormalForce;
								m_vContactForce.x = contactForce.x; m_vContactForce.y = contactForce.y; m_vContactForce.z = contactForce.z;

								pNewInfo->m_rNormalForce = i3Vector::Length( &m_vContactForce);

								if( bDecal )
								{
									i3Vector::Copy( &pNewInfo->m_Normal, &m_vContactNormal);
									i3Vector::Set( &pNewInfo->m_Intersect, contactPoint.x, contactPoint.y, contactPoint.z );
									pNewInfo->m_pShape = iterator.getShape(1);

									if( iterator.getShapeFlags() & NX_SF_HAS_FEATURES_PER_POINT  )
									{
										NxU32 index = iterator.getFeatureIndex1();
										if( index != 0xffffffff )
											pNewInfo->m_FaceIndex = (UINT32) index;
									}
									else
									{
										pNewInfo->m_FaceIndex = 0xFFFFFFFF;
									}
								}
								else
								{
									i3Vector::Scale( &pNewInfo->m_Normal, &m_vContactNormal, -1.f);
									i3Vector::Set( &pNewInfo->m_Intersect, contactPoint.x, contactPoint.y, contactPoint.z );
									pNewInfo->m_pShape = iterator.getShape(0);

									if( iterator.getShapeFlags() & NX_SF_HAS_FEATURES_PER_POINT  )
									{	
										NxU32 index = iterator.getFeatureIndex0();
										if(index != 0xffffffff)
											pNewInfo->m_FaceIndex = (UINT32) index;
									}
									else
									{	
										pNewInfo->m_FaceIndex = 0xFFFFFFFF;
									}
								}
							}
						}

						bShape1Event |= (pShape1->addContactReport( new_obj_ptr ) != false);		// 경고제거코드
						bShape2Event |= (pShape2->addContactReport( new_obj_ptr ) != false);	//(2014.07.03.수빈)

					//	i3Physix::ContactReportClear( &m_Report);
						m_Report.m_Events = 0;
						m_Report.m_pOwnShapeSet = nullptr;
						m_Report.m_pDestShapeSet = nullptr;		// ContactReportClear()함수 대신 이렇게 처리..

					}

					m_ShapeCount++;
				}
			}
		}

		if( bShape1Event )
			pShape1->getEventReceiver()->OnEvent( I3_EVT_CONTACT, pShape1, (UINT32) pShape2->getEventReceiver(), I3_EVT_CODE_ACTIVATE);
		if( bShape2Event)
			pShape2->getEventReceiver()->OnEvent( I3_EVT_CONTACT, pShape2, (UINT32) pShape1->getEventReceiver(), I3_EVT_CODE_ACTIVATE );
	}
} myContactReport;


///////////////////////////////////////////////////
/*
typedef void * (ALLOCPROC)( UINT32 sz);
typedef void  (FREEPROC)( void * p);

#define  LOOP_COUNT 480000
#define  ALLOC_RANGE  4096

static INT32 AllocCnt, FreeCnt;

void _MemState(void)
{
 MEMORYSTATUSEX statex;

 statex.dwLength = sizeof (statex);

 GlobalMemoryStatusEx (&statex);

 I3TRACE( "Alloced System Memory : %ld\n", statex.ullTotalPhys - statex.ullAvailPhys);
}


static void _MemTest( ALLOCPROC pAllocProc, FREEPROC pFreeProc)
{
 INT32 i, sz, idx = 0;
 static void * p[LOOP_COUNT];

 srand( 23);
 memset( p, 0, sizeof(p));

 AllocCnt = 0;
 FreeCnt = 0;

 for( i = 0, idx = 0; i < (LOOP_COUNT >> 1); i++)
 {
  p[idx] = pAllocProc( rand() % 1024);
 }

 _MemState();

 for( i = 0; i < LOOP_COUNT; i++)
 {
  bool bAlloc;

  if( idx <= 0)
   bAlloc = true;
  else if( idx > LOOP_COUNT - 30)
   bAlloc = false;
  else
   if( rand() & 1)
    bAlloc = true;
   else
    bAlloc = false;

  if( bAlloc)
  {
   sz = rand() % 1024;

   p[idx] = pAllocProc( sz);
   idx++;
   AllocCnt++;
  }
  else
  {
   idx--;
   pFreeProc( p[idx]);
   p[idx] = nullptr;
   FreeCnt++;
  }
 }

 I3TRACE( "Alloc : %d    Free : %d\n", AllocCnt, FreeCnt);

 for( i = 0; i < LOOP_COUNT; i++)
 {
  if( p[i] != nullptr)
   pFreeProc( p[i]);
 }
}

void * Alloc1( UINT32 sz)
{
 return LocalAlloc( LPTR, sz);
}

void Free1( void * p)
{
 LocalFree( p);
}

void * Alloc2( UINT32 sz)
{
 return _PHS_Alloc( sz);
}

void Free2( void * p)
{
 _PHS_Free( p);
}

void * Alloc3( UINT32 sz)
{
 return s_MemPool[0].Alloc( sz);
}

void Free3( void * p)
{
 s_MemPool[0].Free( p);
}

void * Alloc4( UINT32 sz)
{
 return new char[sz];
}

void Free4( void * p)
{
 delete p;
}
*/

class i3ActorPairFilter : public NxUserActorPairFiltering
{
public:
	UINT32 *				m_pMask;
	NxScene *				m_pScene;

public:
	/**
	\brief Callback to allow the user to decide whether to filter a certain actor pair.

	Use the actor member of the NxActorPairFilter objects to decide whether to filter out the contact
	between the pair. Set the filtered member to true to apply filtering.

	\param filterArray An array of actor pairs for which filtering is to be decided.
	\param arraySize The number of elements in filterArray.

	@see NxActorPairFilter NxActor::resetUserActorPairFiltering NxScene::setUserActorPairFiltering
	*/
	virtual void onActorPairs(NxActorPairFilter* pTable, NxU32 arraySize)
	{
		UINT32 i;

		for( i = 0; i < arraySize; i++)
		{
			NxActor * pActor0 = pTable[i].actor[0];
			NxActor * pActor1 = pTable[i].actor[1];

			INT32 grp0 = pActor0->getGroup();
			INT32 grp1 = pActor1->getGroup();

			if( (m_pMask[ grp0] & (1 << grp1)) == 0)
			{
				// 처리하지 않음.
				pTable[i].filtered = true;
				continue;
			}
		}
	}
} g_i3ActorPairFilter;

bool i3PhysixContext::Create( NxTimeStepMethod timeStepMethod, REAL32 rSimTimeSlop, UINT32 numResult )
{
	#if defined( I3_PHS_MM)
		_PHS_Init();
	#endif

		/*
	{
		// Memory Allocation Performance Test
		//
		// LocalAlloc
		__SP( 10);
		_MemTest( Alloc1, Free1);
		__EP( 10);

		// PHS
		__SP( 11);
		_MemTest( Alloc2, Free2);
		__EP( 11);

		// MemPool
		__SP( 12);
		_MemTest( Alloc3, Free3);
		__EP( 12);

		// new/delete
		__SP( 13);
		_MemTest( Alloc4, Free4);
		__EP( 13);

		RESET_PROFILE();
		REPORT_PROFILE();
	}
	*/

	// Create the physics SDK
	myAllocator.m_pCtx = this;

	m_pSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, &myAllocator, &myOutputStream);

	if( m_pSDK == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "PhysX Engine을 초기화할 수 없습니다.");
		return false;
	}

	//m_pSDK->getFoundationSDK().getRemoteDebugger()->connect("192.168.1.42", 5425);
#if defined( I3_PHYSIX_DEBUG_OUTPUT ) && defined( I3_DEBUG)
	m_pSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
#endif

	m_rSimTimeSlop = rSimTimeSlop;

	// Set the physics parameters
	m_pSDK->setParameter( NX_SKIN_WIDTH, 0.01f);
	m_pSDK->setParameter( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.1f );
	m_pSDK->setParameter( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, 0.1f );
	m_pSDK->setParameter( NX_BOUNCE_THRESHOLD, -2.f );
	m_pSDK->setParameter( NX_DYN_FRICT_SCALING, 1.0f );
	m_pSDK->setParameter( NX_STA_FRICT_SCALING, 1.12f );
	m_pSDK->setParameter( NX_MAX_ANGULAR_VELOCITY, 7.f );
	/*m_pSDK->setParameter(NX_CONTINUOUS_CD, 1);
	m_pSDK->setParameter(NX_CCD_EPSILON, 0.01);*/

	//m_pSDK->setParameter( NX_COLL_VETO_JOINTED, 0.f );
	
	// Set the debug visualization parameters
	//m_pSDK->setParameter( NX_VISUALIZATION_SCALE, 1);
	//m_pSDK->setParameter( NX_VISUALIZE_COLLISION_SHAPES, 0);
	//m_pSDK->setParameter( NX_VISUALIZE_ACTOR_AXES, 0);

    // Create the scene
	CreateScene( timeStepMethod);

	// Result List;
	{
		m_ResultBuffSize = numResult;
		I3MEM_SAFE_FREE_POINTER( m_pResultList ); 

		m_pResultList = (I3_PHYSIX_HIT_RESULT *) i3MemAlloc( sizeof(I3_PHYSIX_HIT_RESULT) * m_ResultBuffSize);
		I3ASSERT( m_pResultList != nullptr );
	}

	m_pContactPointPool->Create( "Contact_Point_Pool", sizeof(I3_PHYSIX_CONTACT_INFO), 1024);
	m_pContactPointPool->SetAutoCompactEnable(false);

	_initCmd();

	return true;
}

bool i3PhysixContext::CreateScene( NxTimeStepMethod timeStepMethod)
{
	NxSceneDesc						sceneDesc;
	NxVec3							DefaultGravity( 0.0f, -9.81f, 0.0f);
	sceneDesc.setToDefault();
	if( IsHardwarePresent() )
	{
 		sceneDesc.simType				= NX_SIMULATION_HW;
	}
	
	sceneDesc.gravity               = DefaultGravity;
	//sceneDesc.flags					= 0;
	//sceneDesc.flags					|= NX_SF_ENABLE_ACTIVETRANSFORMS;
	//sceneDesc.flags					|= NX_SF_DISABLE_SCENE_MUTEX;
#if defined( I3_PHYSIX_MULTITHREAD )
	sceneDesc.flags					|= NX_SF_SIMULATE_SEPARATE_THREAD;
	sceneDesc.simThreadStackSize	= 1024 * 1024 * 16;
#else
	sceneDesc.flags					&= ~NX_SF_SIMULATE_SEPARATE_THREAD;
#endif
	sceneDesc.groundPlane			= false;
	sceneDesc.userContactReport		= &myContactReport;

	m_pScene = m_pSDK->createScene( sceneDesc);
	if( m_pScene == nullptr )
	{
		sceneDesc.simType = NX_SIMULATION_SW;
		m_pScene = m_pSDK->createScene( sceneDesc);
		if( m_pScene == nullptr )
			return false;
	}
	s_pNxScene = m_pScene;

	m_pScene->setTiming( 1.f / 30.f, 1, timeStepMethod );
	// m_pScene->setTiming( 1.f / 240.f, 4, NX_TIMESTEP_FIXED );

	#if defined( OWN_PAIR_FILTER)
		m_pScene->setUserActorPairFiltering( &g_i3ActorPairFilter);
	#endif

	// 초기값으로 0을 제외한, 모든 Group 간에 Collision이 불가능한 것으로 한다.
	//
	{
		for( INT32 i = 0; i < 32; i++)
		{
			m_GroupMask[i] = 0;

			for( INT32 j = 0; j < 32; j++)
			{
				m_pScene->setGroupCollisionFlag( (NxCollisionGroup) i, (NxCollisionGroup) j, false);
			}
		}

		//setGroupCollisionEnable( 0, 0, true);
	}

	// Terrain에 대한 기본 Material들을 모두 생성해 둔다.
	{
		NxMaterialDesc desc;
		NxMaterial * pMtl;

		for( INT32 i = 0; i < I3_TERRAIN_TYPE_COUNT-1; i++)
		{
			desc.restitution = s_TerrainMtlTable[i + 1].m_Restitution;
			desc.staticFriction = s_TerrainMtlTable[ i + 1].m_StaticFriction + 0.1f;
			desc.dynamicFriction = s_TerrainMtlTable[ i + 1].m_DynamicFriction;
		
			pMtl = m_pScene->createMaterial( desc);
			I3ASSERT( pMtl != nullptr);
		}
	}

	g_i3ActorPairFilter.m_pMask = m_GroupMask;
	g_i3ActorPairFilter.m_pScene = m_pScene;

	return true;
}

void i3PhysixContext::ReleaseScene( void)
{
	if( isValidScene() )
	{
		if( m_pGroundPlaneActor != nullptr )
		{
			m_pScene->releaseActor( *m_pGroundPlaneActor );
			m_pGroundPlaneActor = nullptr;
		}
		
		m_pSDK->releaseScene( *m_pScene );
		m_pScene = nullptr;
	}
}

bool i3PhysixContext::IsHardwarePresent()
{
#if defined( I3_PHYSIX_HARDWARE )
	return m_pSDK->getHWVersion() != NX_HW_VERSION_NONE;
#else
	return false;
#endif
}

void i3PhysixContext::reset(void)
{
#if defined( I3_PHYSIX_MEMPOOL)
	INT32 i;

	for( i = 0; i < MPT_MAX; i++)
	{
		s_MemPool[i].Contact();
	}
#endif
}

void i3PhysixContext::RemovePool(void)
{
	#if defined( I3_PHYSIX_MEMPOOL)
	INT32 i;

	for( i = 0; i < MPT_MAX; i++)
	{
		s_MemPool[i].ReleaseAllocList();
	}
#endif
}


NxController *	 i3PhysixContext::createController( NxControllerDesc * pDesc)
{
	if( m_pScene != nullptr)
		return s_pCtrlManager->createController( m_pScene, (const NxControllerDesc&)(*pDesc) ); 
	return nullptr;
}

void	i3PhysixContext::simulate( REAL32 tm)
{
	//NxActiveTransform * pTxTable = m_pScene->getActiveTransforms( activeTxCount);

	if( isValidScene() == false )
		return;

	if( !m_bSimulate )
		return;

	s_pCtrlManager->updateControllers();

	// Start simulation (non-blocking)
	if( tm > 0.0f)
	{
		if( (tm ) > 0.016666f)
		{
			//I3TRACE( "SIM %f\n", tm * m_rSimTimeSlop);
			tm = 0.016666f;
		}

		m_bInSimulating = true;

		// Group Mask에 대한 초기화...
		// NXU Library에서 Loading 중에 임의로 Filter 값들을 변경하기 때문에
		// 항상 재설정한다.
		{
			setFilterConstant( PHF_FILTER, PHF_FILTER);
			setFilterOps( NX_FILTEROP_AND, NX_FILTEROP_AND, NX_FILTEROP_AND);
			setFilterBool( false);
		}

		m_pScene->simulate( tm * m_rSimTimeSlop );

		m_pScene->flushStream();

		#if !defined( I3_PHYSIX_MULTITHREAD)
		bool bFetchResult = m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true);
		I3ASSERT(bFetchResult);
		#endif

		m_bInSimulating = false;
		m_bSimStarted = true;
	}
	else
	{
		m_bSimStarted = false;
	}
}

void	i3PhysixContext::fetchResults( REAL32 tm)
{
	if( m_bSimStarted == true)
	{
		m_bSimStarted = false;

		#if defined( I3_PHYSIX_MULTITHREAD)
			m_pScene->fetchResults( NX_RIGID_BODY_FINISHED, true);
		#endif
	}

	_processCmd( tm);
}

void i3PhysixContext::_getProfileString(i3::string& inout)
{
#if !defined( I3_NO_PROFILE)
	inout += "---- Physix\n";
	
	char conv[256];

	double timeRes = i3Timer::GetProfileResolution();

	#if 0
	// Time
	sprintf( conv, "allocTime:%.4f(%.4f), totalTime:%.4f(%.4f), freeTime:%.4f(%.4f)\n",
		s_timeAlloc * timeRes, m_timeAllocMax * timeRes,
		s_timeAllocTotal * timeRes, m_timeAllocTotalMax * timeRes,
		s_timeFree * timeRes, m_timeFreeMax * timeRes);
	inout += conv;
	
	// Count
	sprintf( conv, "allocCount:%d(%d), reallocCount:%d(%d), freeCount:%d(%d)\n",
		s_nPhysix_MemAllocCountPerFrame, m_nProfileMemAllocMaxCount,
		s_nPhysix_MemReAllocCountPerFrame, m_nProfileMemReAllocMaxCount,
		s_nPhysix_MemFreeCountPerFrame, m_nProfileMemFreeMaxCount );
	inout += conv;

	#if defined( I3_PHYSIX_MEMPOOL)
	INT32 i;

		for( i = 0; i < MPT_MAX; i++)
		{
			sprintf( conv, "%-3s (%d, A(%d), F(%d)) - %d bytes, HitRate:%.1f%%\n", s_szMemPoolName[i],
				s_MemPool[i].getTotalCount(),
				s_MemPool[i].getAllocCount(),
				s_MemPool[i].getFreeCount(),
				s_MemPool[i].getTotalSize(),
				(REAL32) s_MemPool[i].getHitAllocRequest() / s_MemPool[i].getTotalAllocRequest() * 100.0f);

			inout += conv;
		}
	#endif

	sprintf( conv, "Actors: %d, Pairs: %d, %s: %d, %s: %d\n", m_pScene->getNbActors(), m_pScene->getNbPairs(), m_pScene->getStats2()->stats[0].name, m_pScene->getStats2()->stats[0].curValue, m_pScene->getStats2()->stats[4].name, m_pScene->getStats2()->stats[4].curValue  );
	inout += conv;
#endif

	sprintf( conv, "RaycastTime : %.4f(%.4f)  Count : %d(%d)\n", s_timeRaycast * timeRes, m_timeRaycastMax * timeRes, s_Physix_RaycastCount, m_nProfileRaycastMaxCount );
	inout += conv;

	if( m_timeRaycastMax < s_timeRaycast )
		m_timeRaycastMax = s_timeRaycast;

	if( m_nProfileRaycastMaxCount < s_Physix_RaycastCount )
		m_nProfileRaycastMaxCount = s_Physix_RaycastCount;

	if( m_timeAllocMax < s_timeAlloc)
		m_timeAllocMax = s_timeAlloc;

	if( m_timeAllocTotalMax < s_timeAllocTotal)
		m_timeAllocTotalMax = s_timeAllocTotal;

	if( m_timeFreeMax < s_timeFree)
		m_timeFreeMax = s_timeFree;

	if( m_nProfileMemAllocMaxCount < s_nPhysix_MemAllocCountPerFrame)
		m_nProfileMemAllocMaxCount = s_nPhysix_MemAllocCountPerFrame;

	if( m_nProfileMemReAllocMaxCount < s_nPhysix_MemReAllocCountPerFrame)
		m_nProfileMemReAllocMaxCount = s_nPhysix_MemReAllocCountPerFrame;

	if( m_nProfileMemFreeMaxCount < s_nPhysix_MemFreeCountPerFrame)
		m_nProfileMemFreeMaxCount = s_nPhysix_MemFreeCountPerFrame;
#endif
}
void i3PhysixContext::_resetProfileData()
{
#if !defined( I3_NO_PROFILE)
	s_timeRaycast = 0;
	s_Physix_RaycastCount = 0;
	s_timeAlloc = 0L;
	s_timeAllocTotal = 0L;
	s_timeFree = 0L;
	s_nPhysix_MemAllocCountPerFrame = 0;
	s_nPhysix_MemReAllocCountPerFrame = 0;
	s_nPhysix_MemFreeCountPerFrame = 0;
#endif
}

I3_TERRAIN_MATERIAL *	i3PhysixContext::getTerrainMaterialPreset( I3_TERRAIN_TYPE terrain)
{
	return &s_TerrainMtlTable[ terrain];
}

void i3PhysixContext::RemoveAllMaterials(void)
{
	UINT32 i;
	NxMaterial * pMaterial;

	if( isValidScene())
	{
		for( i = 0; i < m_pScene->getHighestMaterialIndex (); i++)
		{
			pMaterial = m_pScene->getMaterialFromIndex( (NxMaterialIndex) i);

			if( pMaterial != nullptr)
			{
				if( pMaterial->userData != nullptr)
				{
					i3MemFree( pMaterial->userData);
				}

				m_pScene->releaseMaterial( *pMaterial);
			}
		}
	}
}

NxMaterial * i3PhysixContext::validateMaterial( REAL32 restitution, REAL32 sFriction, REAL32 dFriction)
{
	NxMaterialIndex i;

	if( isValidScene() == false)
		return nullptr;

	for( i = 0; i <= m_pScene->getHighestMaterialIndex(); i++)
	{
		NxMaterial * pMtl = m_pScene->getMaterialFromIndex( i);

		if( pMtl != nullptr)
		{
			if(	i3Math::isAlike( pMtl->getRestitution(), restitution, I3_EPS) &&
				i3Math::isAlike( pMtl->getStaticFriction(), sFriction, I3_EPS) &&
				i3Math::isAlike( pMtl->getDynamicFriction(), dFriction, I3_EPS))
			{
				return pMtl;
			}
		}
	}

	NxMaterialDesc desc;

	desc.setToDefault();

	desc.restitution = restitution;
	desc.staticFriction = sFriction;
	desc.dynamicFriction = dFriction;
	//desc.flags = 0;

	return createMaterial( &desc);
}

void i3PhysixContext::setGroupCollisionEnable( UINT16 GrpIdx1,	UINT16 GrpIdx2, bool bFlag)
{
	UINT16 i;

	if( isValidScene())
	{
		if( GrpIdx2 == 0xFFFF)
		{
			for( i = 0; i < 32; i++)
			{
				m_pScene->setGroupCollisionFlag( GrpIdx1, i, bFlag);
			}
		}
		else
		{
			m_pScene->setGroupCollisionFlag( GrpIdx1, GrpIdx2, bFlag);
		}
	}
}

void i3PhysixContext::setGroupMaskEnable( UINT16 MaskGrpIdx1, UINT16 GrpIdx2, bool bFlag)
{
	if( bFlag)
		m_GroupMask[ MaskGrpIdx1] |= (1 << GrpIdx2);
	else
		m_GroupMask[ MaskGrpIdx1] &= ~(1 << GrpIdx2);
}

UINT32	i3PhysixContext::getCollidableDestMask( INT32 group)
{
	INT32 i;
	UINT32 val, mask;

	if( isValidScene() == false)
		return 0;

	val = 0;
	mask = 1;

	for( i = 0; i < 32; i++)
	{
		if( m_pScene->getGroupCollisionFlag( (NxCollisionGroup) group, (NxCollisionGroup) i))
			val |= mask;

		mask = mask << 1;
	}

	return val;
}

inline void _SET_RAYCAST_RESULT( NxScene * pScene, I3_PHYSIX_HIT_RESULT * pResult, NxRaycastHit * pHit)
{
	NxShape * pShape = pHit->shape;
	
	i3Vector::Copy( & pResult->m_Intersect, (VEC3D *) &pHit->worldImpact);
	i3Vector::Copy( & pResult->m_Normal,	(VEC3D *) &pHit->worldNormal);
	pResult->m_pDestShapeSet	= (i3PhysixShapeSet *) pShape->userData;
	pResult->m_T				= pHit->distance;
	pResult->m_pHitShape		= pShape;
	pResult->m_FaceIndex		= pHit->faceID;
	pResult->m_pTerrain			= nullptr;

	if( pResult->m_pDestShapeSet != nullptr )
	{
		if((pShape->isTriangleMesh()) )
		{
			I3_PHYSIX_TRI * pTri = pResult->m_pDestShapeSet->getFace( pHit->faceID);

			pResult->m_pTerrain = pResult->m_pDestShapeSet->getTerrain( pTri->m_MaterialIndex);
		}
		else
		{
			for( INT32 i = 0; i < pResult->m_pDestShapeSet->getNxShapeCount(); i++ )
			{
				if( pResult->m_pDestShapeSet->getNxShape( i ) == pShape )
				{
					pResult->m_pTerrain = pResult->m_pDestShapeSet->getTerrain( i );
					break;
				}
			}

		}
	}
}

I3_PHYSIX_HIT_RESULT *	i3PhysixContext::RaycastClosest( i3CollideeLine * pLine, INT32 grpIndex, UINT32 mask)
{
#if !defined( I3_NO_PROFILE)
	INT64 ntimeStart = i3Timer::GetProfileTime();
#endif
	NxRay ray;
	NxRaycastHit hit;
	I3_PHYSIX_HIT_RESULT * pResult = getResult();
	NxShape * pShape;
	NxGroupsMask * pGroupMask, grpMask;

	if( isValidScene() == false)
		return nullptr;

	m_ResultCount = 0;

	ray.orig = *((NxVec3 *)pLine->GetStart());
	ray.dir = *((NxVec3 *)pLine->GetDir());
	ray.dir.normalize();

	if( mask != 0xFFFFFFFF)
	{
		pGroupMask = &grpMask;
		grpMask.bits0 = mask;
		grpMask.bits1 = grpMask.bits2 = grpMask.bits3 = 0;
	}
	else
	{
		pGroupMask = nullptr;
	}
	
	pShape = m_pScene->raycastClosestShape( ray, NX_ALL_SHAPES, hit, m_GroupMask[ grpIndex], NX_MAX_F32, 
				NX_RAYCAST_SHAPE | NX_RAYCAST_IMPACT | NX_RAYCAST_FACE_INDEX | NX_RAYCAST_FACE_NORMAL | NX_RAYCAST_DISTANCE | NX_RAYCAST_MATERIAL,
				pGroupMask);

	if( pShape != nullptr)
	{
		m_ResultCount = 1;
	
		_SET_RAYCAST_RESULT( m_pScene, pResult, &hit);

#if !defined( I3_NO_PROFILE)
	s_timeRaycast += i3Timer::GetProfileTime() - ntimeStart;
	s_Physix_RaycastCount ++;
#endif

		return pResult;
	}

#if !defined( I3_NO_PROFILE)
	s_timeRaycast += i3Timer::GetProfileTime() - ntimeStart;
	s_Physix_RaycastCount ++;
#endif

	return nullptr;
}

class i3NxRaycastReport : public NxUserRaycastReport
{
public:
	I3_PHYSIX_HIT_RESULT *	m_pResult;
	UINT32					m_Count;
	NxScene *				m_pScene;
	UINT32					m_Limit;

    virtual bool onHit(const NxRaycastHit& hit)
    {
        //NxActor *hitActor = hit.shape->getActor();

		I3ASSERT( m_Count < m_Limit);
		_SET_RAYCAST_RESULT( m_pScene, & m_pResult[ m_Count], (NxRaycastHit *) &hit);
		m_Count++;

        return true;
    }
};

UINT32	i3PhysixContext::Raycast( i3CollideeLine * pLine, INT32 grpIndex, UINT32 mask)
{
#if !defined( I3_NO_PROFILE)
	INT64 ntimeStart = i3Timer::GetProfileTime();
#endif
	NxRay ray;
	NxRaycastHit hit;
	i3NxRaycastReport report;
	NxGroupsMask grpMask, * pGroupMask;

	I3_PHYSIX_HIT_RESULT * pResult = getResult();

	if( isValidScene() == false)
		return 0;

	ray.orig = *((NxVec3 *)pLine->GetStart());
	ray.dir = *((NxVec3 *)pLine->GetDir());
	ray.dir.normalize();

	report.m_pResult = pResult;
	report.m_Count = 0;
	report.m_pScene = m_pScene;
	report.m_Limit = m_ResultBuffSize;

	if( mask != 0xFFFFFFFF)
	{
		pGroupMask = &grpMask;
		grpMask.bits0 = mask;
		grpMask.bits1 = grpMask.bits2 = grpMask.bits3 = 0;
	}
	else
	{
		pGroupMask = nullptr;
	}

	m_ResultCount = m_pScene->raycastAllShapes( ray, report, NX_ALL_SHAPES, m_GroupMask[ grpIndex], NX_MAX_F32, 
						NX_RAYCAST_SHAPE | NX_RAYCAST_IMPACT | NX_RAYCAST_FACE_NORMAL | NX_RAYCAST_FACE_INDEX | 
						NX_RAYCAST_DISTANCE | NX_RAYCAST_MATERIAL,
						pGroupMask);

#if !defined( I3_NO_PROFILE)
	s_timeRaycast += i3Timer::GetProfileTime() - ntimeStart;
	s_Physix_RaycastCount++;
#endif

	return m_ResultCount;
}

I3_PHYSIX_HIT_RESULT *	i3PhysixContext::OverlapSphereClosest( i3CollideeSphere * pSphere, INT32 grpIndex)
{
	return nullptr;
}

class i3NxShapeReport : public NxUserEntityReport<NxShape*>
{
public:
	virtual bool onEvent(NxU32 nbShapes, NxShape** shapes)
	{
		//while (nbShapes--)
		//{
			//NxActor* actor = &shapes[nbShapes]->getActor();
			//((ActorUserData *)(actor->userData))->flags |= UD_PASSES_INTERSECTION_TEST;
		//}
		return true;
	}

};

/*
class i3NxSphereContactReport : public NxUserContactReport
{        
    void onContactNotify( NxContactPair& pair, NxU32 events)
    {        
        //... you can read the contact information out of the
        // contact pair data here.        
    } 
} myReport;
*/


UINT32	i3PhysixContext::OverlapSphere( i3CollideeSphere * pSphere, INT32 grpIndex)
{
	NxSphere sphere;

	sphere.center = *((NxVec3 *) pSphere->GetCenter());
	sphere.radius = pSphere->GetRadius();

	//m_ResultCount = m_pScene->overlapSphereShapes( sphere, NX_ALL_SHAPES, 0, nullptr, grpMask, nullptr, true);

	return 0;
}

#if defined( I3_DEBUG)

void i3PhysixContext::Dump( bool bDumpActor)
{
	UINT32 i;

	I3TRACE( "--------------------- PHYSIX ------------------------\n");

	// Actors
	if( bDumpActor)
	{
		NxActor ** ppActors = m_pScene->getActors();

		for( i = 0; i < m_pScene->getNbActors(); i++)
		{
			I3TRACE( "ACTOR[%05d] : ", i);

			if( ppActors[i]->getName() != nullptr)
				I3TRACE( "%s : ", ppActors[i]->getName());
			else
				I3TRACE( "Nonamed : ");

			i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) ppActors[i]->userData;

			if( i3::same_of<i3PhysixShapeSet*>(pShapeSet))
			{
				i3EventReceiver * pOwner = pShapeSet->getEventReceiver();

				if( pOwner != nullptr)
					I3TRACE( "OWNER(%s) :", pOwner->meta()->class_name());
				else
					I3TRACE( "OWNER(-) :");
			}

			if( ppActors[i]->readActorFlag( NX_AF_DISABLE_COLLISION) == false)
				I3TRACE( "C");

			if( ppActors[i]->readActorFlag( NX_AF_DISABLE_RESPONSE) == false)
				I3TRACE( "R");

			I3TRACE( "\n");
		}
	}

	I3TRACE( "Total %d Actors\n", m_pScene->getNbActors());

	// Joints
	{
		I3TRACE( "Total %d Joints\n", m_pScene->getNbJoints());
	}

	// Shapes
	{
		I3TRACE( "Total %d static-shapes\n", m_pScene->getNbStaticShapes());
		I3TRACE( "Total %d dynamic-shapes\n", m_pScene->getNbDynamicShapes());
	}

	I3TRACE( "-------------------------------------------------------\n");
}

void i3PhysixContext::DumpPairs(void)
{
}

void i3PhysixContext::LogDebugRenderable(void)
{
	UINT32 i;
	NxDebugRenderable * pData = (NxDebugRenderable *) m_pScene->getDebugRenderable();

	if( isValidScene() == false )
		return;

	// Points
	{
		NxDebugPoint * pPt = (NxDebugPoint *) pData->getPoints();

		for( i = 0; i < pData->getNbPoints(); i++)
		{
			REAL32 r, g, b;

			r = (((pPt->color >> 16) & 0xFF) / 255.0f);
			g = (((pPt->color >> 8) & 0xFF) / 255.0f);
			b = (((pPt->color >> 0) & 0xFF) / 255.0f);

			///i3VLog( "c %f %f %f", r, g, b);
			//i3VLog( "r %f %f %f %f", pPt->p.x, pPt->p.y, pPt->p.z, 1.0f);
			pPt++;
		}
	}

	//i3VLog( "u");
}

#endif

#define MAX_CMD		1000

void	i3PhysixContext::_initCmd(void)
{
	m_pCmdRing = (I3_PHYSIX_CMD_INFO *) i3MemAlloc( sizeof(I3_PHYSIX_CMD_INFO) * MAX_CMD);
	I3ASSERT( m_pCmdRing != nullptr);

	for( UINT32 i = 0; i < MAX_CMD; i++)
	{
		m_pCmdRing[i].Reset();
	}

	m_idxCmdHead = 0;	// Position to push
	m_idxCmdTail = 0;	// position to pop
}

I3_PHYSIX_CMD_INFO * i3PhysixContext::_allocCmd(void)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	pInfo = &m_pCmdRing[ m_idxCmdHead];

	m_idxCmdHead++;
	if( m_idxCmdHead >= MAX_CMD)
	{
		m_idxCmdHead = 0;
	}

	#if defined( I3_DEBUG)
		if( m_idxCmdHead == m_idxCmdTail)
		{
			I3PRINTLOG(I3LOG_WARN,  "PH-Cmd buffer overflow!!!");
		}
	#endif

	return pInfo;
}

void	i3PhysixContext::_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, UINT32 param1, UINT32 param2)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	pInfo = _allocCmd();

	I3_SAFE_ADDREF( pShapeSet);

	pInfo->m_Command = cmd;
	pInfo->m_pShapeSet = pShapeSet;
	pInfo->m_pObject = pObj;
	pInfo->m_Param.m_i[0] = param1;
	pInfo->m_Param.m_i[1] = param2;
}

void	i3PhysixContext::_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, MATRIX * pMtx)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	pInfo = _allocCmd();

	I3_SAFE_ADDREF( pShapeSet);

	pInfo->m_Command = cmd;
	pInfo->m_pShapeSet = pShapeSet;
	pInfo->m_pObject = pObj;
	NXU::Copy( (NxMat34 *) & pInfo->m_Param.m_r, pMtx);
}

void i3PhysixContext::_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, VEC3D * pVec)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	pInfo = _allocCmd();

	I3_SAFE_ADDREF( pShapeSet);

	pInfo->m_Command = cmd;
	pInfo->m_pShapeSet = pShapeSet;
	pInfo->m_pObject = pObj;
	pInfo->m_Param.m_r[0] = pVec->x;
	pInfo->m_Param.m_r[1] = pVec->y;
	pInfo->m_Param.m_r[2] = pVec->z;
}

void i3PhysixContext::_pushCmd( I3_PHYSIX_CMD cmd, i3PhysixShapeSet * pShapeSet, void * pObj, VEC3D * pVec1, VEC3D * pVec2)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	pInfo = _allocCmd();

	I3_SAFE_ADDREF( pShapeSet);

	pInfo->m_Command = cmd;
	pInfo->m_pShapeSet = pShapeSet;
	pInfo->m_pObject = pObj;
	pInfo->m_Param.m_r[0] = pVec1->x;
	pInfo->m_Param.m_r[1] = pVec1->y;
	pInfo->m_Param.m_r[2] = pVec1->z;
	pInfo->m_Param.m_r[3] = pVec2->x;
	pInfo->m_Param.m_r[4] = pVec2->y;
	pInfo->m_Param.m_r[5] = pVec2->z;
}

void	i3PhysixContext::_resetCmd(void)
{
	m_idxCmdHead = m_idxCmdTail = 0;
}

void	i3PhysixContext::_processCmd( REAL32 tm)
{
	I3_PHYSIX_CMD_INFO * pInfo;

	while( m_idxCmdHead != m_idxCmdTail)
	{
		pInfo = & m_pCmdRing[ m_idxCmdTail];

		switch( pInfo->m_Command)
		{
			case I3_PHYSIX_CMD_UPDATE_DYNAMIC_STATE :		_OnUpdateDynamicState( pInfo);	break;
			case I3_PHYSIX_CMD_RELEASE_ACTOR :				
				//_OnReleaseActor( (NxActor *) pInfo->m_pObject);
				{
					I3_PHYSIX_RELEASE_ACTOR_INFO * pActorInfo = (I3_PHYSIX_RELEASE_ACTOR_INFO *) i3MemAlloc( sizeof(I3_PHYSIX_RELEASE_ACTOR_INFO));

					pActorInfo->m_pActor = (NxActor *) pInfo->m_pObject;
					pActorInfo->m_time = 0.0f;

					m_RelActorList.push_back( pActorInfo);
				}
				break;
			case I3_PHYSIX_CMD_RELEASE_JOINT :				_OnReleaseJoint( pInfo);		break;
			case I3_PHYSIX_CMD_RELEASE_CONTROLLER :			_OnReleaseController( pInfo);	break;
			case I3_PHYSIX_CMD_UPDATE_GLOBAL_POSE :			
				_OnUpdateGlobalPose( pInfo);	
				break;
			case I3_PHYSIX_CMD_SET_LINEAR_VELOCITY :		_OnSetLinearVelocity( pInfo);	break;
			case I3_PHYSIX_CMD_SET_ANGULAR_VELOCITY :		_OnSetAngularVelocity( pInfo);	break;
			case I3_PHYSIX_CMD_SET_DIMENSION :				_OnSetDimension( pInfo);		break;
		}

		I3_SAFE_RELEASE( pInfo->m_pShapeSet);

		m_idxCmdTail++;
		if( m_idxCmdTail >= MAX_CMD)
			m_idxCmdTail = 0;
	}

	_resetCmd();

	{
		
		I3_PHYSIX_RELEASE_ACTOR_INFO * pActorInfo;

		for( size_t i = 0; i < m_RelActorList.size(); i++)
		{
			pActorInfo = m_RelActorList[i];

			pActorInfo->m_time += tm;

			//if( pActorInfo->m_time > 1.0f)
			{
				_OnReleaseActor( pActorInfo->m_pActor);

				i3MemFree( pActorInfo);

				m_RelActorList.erase( m_RelActorList.begin() + i);
				i--;
			}
		}
	}
}

#define		__CHK_RUN()			(m_pScene->pollForWork( NX_WAIT_NONE) == NX_THREAD_MOREWORK)

void i3PhysixContext::CMD_releaseActor( NxActor * pActor)
{ 
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			pActor->raiseActorFlag( (NxActorFlag)(NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE));

			NxShape * const * ppShapes = pActor->getShapes();

			for( UINT32 i = 0; i < pActor->getNbShapes(); i++)
			{
				ppShapes[i]->setFlag( NX_TRIGGER_ENABLE, false);
				ppShapes[i]->setFlag( NX_SF_DISABLE_RAYCASTING, true);

			}

			_pushCmd( I3_PHYSIX_CMD_RELEASE_ACTOR, nullptr, pActor);
		}
		else
		{
			_OnReleaseActor( pActor);
		}
	}
}

void i3PhysixContext::CMD_releaseJoint( NxJoint * pJoint)			
{ 
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_RELEASE_JOINT, nullptr, pJoint);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_RELEASE_JOINT;
			info.m_pObject = pJoint;

			_OnReleaseJoint( &info);
		}
	}
}

void i3PhysixContext::CMD_releaseController( NxController * pCtrl)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_RELEASE_CONTROLLER, nullptr, pCtrl);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_RELEASE_CONTROLLER;
			info.m_pObject = pCtrl;

			_OnReleaseController( &info);
		}
	}
}

void i3PhysixContext::CMD_updateDynamicState( i3PhysixShapeSet * pShapeSet)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_UPDATE_DYNAMIC_STATE, pShapeSet, nullptr);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_UPDATE_DYNAMIC_STATE;
			info.m_pShapeSet = pShapeSet;

			_OnUpdateDynamicState( &info);
		}
	}
}

void i3PhysixContext::CMD_updateGlobalPose( i3PhysixShapeSet * pShapeSet, MATRIX * pMtx)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_UPDATE_GLOBAL_POSE, pShapeSet, nullptr, pMtx);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_UPDATE_GLOBAL_POSE;
			info.m_pShapeSet = pShapeSet;
			NXU::Copy( (NxMat34 *) info.m_Param.m_r, pMtx);
			
			_OnUpdateGlobalPose( &info);
		}
	}
}

void i3PhysixContext::CMD_setLinearVelocity( i3PhysixShapeSet * pShapeSet, VEC3D * pLinVel)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_SET_LINEAR_VELOCITY, pShapeSet, nullptr, pLinVel);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_SET_LINEAR_VELOCITY;
			info.m_pShapeSet = pShapeSet;
			info.m_Param.m_r[0] = pLinVel->x;
			info.m_Param.m_r[1] = pLinVel->y;
			info.m_Param.m_r[2] = pLinVel->z;

			_OnSetLinearVelocity( &info);
		}
	}
}

void i3PhysixContext::CMD_setAngularVelocity( i3PhysixShapeSet * pShapeSet, VEC3D * pLinVel)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_SET_ANGULAR_VELOCITY, pShapeSet, nullptr, pLinVel);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_SET_ANGULAR_VELOCITY;
			info.m_pShapeSet = pShapeSet;
			info.m_Param.m_r[0] = pLinVel->x;
			info.m_Param.m_r[1] = pLinVel->y;
			info.m_Param.m_r[2] = pLinVel->z;

			_OnSetAngularVelocity( &info);
		}
	}
}

void i3PhysixContext::CMD_setDimension( NxShape * pShape, VEC3D * pDimension)
{
	if( isValidScene())
	{
		if( __CHK_RUN())
		{
			_pushCmd( I3_PHYSIX_CMD_SET_DIMENSION, nullptr, pShape, pDimension);
		}
		else
		{
			I3_PHYSIX_CMD_INFO info;

			info.m_Command = I3_PHYSIX_CMD_SET_ANGULAR_VELOCITY;
			info.m_pObject = pShape;
			info.m_Param.m_r[0] = pDimension->x;
			info.m_Param.m_r[1] = pDimension->y;
			info.m_Param.m_r[2] = pDimension->z;

			_OnSetDimension( &info);
		}
	}
}

void i3PhysixContext::_OnUpdateDynamicState( I3_PHYSIX_CMD_INFO * pInfo)
{
	I3ASSERT(	pInfo->m_pShapeSet != nullptr);

	pInfo->m_pShapeSet->_UpdateDynamicState();
}

void i3PhysixContext::_OnReleaseActor( NxActor * pActor)
{
	I3ASSERT( isValidScene());

	NxShape * const * ppShapes;

	ppShapes = pActor->getShapes();

	//I3TRACE( "Actor release!!!!\n");

	// Actor를 제거하면서 사용하던 Shape들 중에 Convex, Tri-Mesh가
	// 있는지 확인한다.
	for(NxU32 i = 0; i < pActor->getNbShapes(); i++)
	{
		ppShapes[i]->setFlag( NX_SF_DISABLE_COLLISION, true);
		ppShapes[i]->setFlag( NX_SF_DISABLE_RAYCASTING, true);
		ppShapes[i]->setFlag( NX_SF_DISABLE_RESPONSE, true);
		switch( ppShapes[i]->getType())
		{
			case NX_SHAPE_CONVEX :
				{
					NxConvexShape * pMesh = (NxConvexShape *) ppShapes[i];
					m_ConvexList.push_back( & pMesh->getConvexMesh());
				}
				break;

			case NX_SHAPE_MESH :
				{
					NxTriangleMeshShape * pMesh = (NxTriangleMeshShape *) ppShapes[i];
					m_MeshList.push_back( & pMesh->getTriangleMesh());
				}
				break;
		}
	}

	m_pScene->releaseActor( *pActor ); 

	for(size_t i = 0; i < m_ConvexList.size(); i++)
	{
		NxConvexMesh * pMesh = m_ConvexList[i];

		if( pMesh->getReferenceCount() == 0)
		{
			m_pSDK->releaseConvexMesh( *pMesh);
		}
	}
	m_ConvexList.clear();

	for( size_t i = 0; i < m_MeshList.size(); i++)
	{
		NxTriangleMesh * pMesh = m_MeshList[i];

		if( pMesh->getReferenceCount() == 0)
		{
			m_pSDK->releaseTriangleMesh( *pMesh);
		}
	}
	m_MeshList.clear();
}

void i3PhysixContext::_OnReleaseJoint( I3_PHYSIX_CMD_INFO * pInfo)
{
	NxJoint * pJoint = (NxJoint *) pInfo->m_pObject;

	m_pScene->releaseJoint( *pJoint);
}

void i3PhysixContext::_OnReleaseController( I3_PHYSIX_CMD_INFO * pInfo)
{
	NxController * pCtrl = (NxController *) pInfo->m_pObject; 

	s_pCtrlManager->releaseController( *pCtrl );
}

void i3PhysixContext::_OnUpdateGlobalPose( I3_PHYSIX_CMD_INFO * pInfo)
{
	NxVec3 v3Pos;

#if defined ( I3_CHECK_FLOAT_VALUE_FOR_PHYSICX ) && defined (I3_DEBUG)
	{
		for(INT32 i = 0; i< 12; i++) {
			CheckInvalidRealValue(pInfo, pInfo->m_Param.m_r[i]);
		}
	}
#endif
	
#if 1
	if( pInfo->m_pShapeSet->getNxActor()->isDynamic() )
	{
		if( pInfo->m_pShapeSet->getNxActor()->readBodyFlag( NX_BF_KINEMATIC ) )
		{
			if( pInfo->m_pShapeSet->getShapeType() == I3_PHYSIX_SHAPE_TRIMESH )
				pInfo->m_pShapeSet->getNxActor()->setGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r);
			else
			{
				
				// v3Pos = pInfo->m_pShapeSet->getNxActor()->getGlobalPosition();
				// I3TRACE("Before v3Pos %f %f %f \n", v3Pos.x, v3Pos.y, v3Pos.z);
				if (pInfo->m_pShapeSet->getPhysxStyle() == I3PHYSIX_USAGE_TYPE_KM_FORCE_DISABLE)
					pInfo->m_pShapeSet->getNxActor()->setGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r ); 
				else
					pInfo->m_pShapeSet->getNxActor()->moveGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r ); 
				// pInfo->m_pShapeSet->getNxActor()->setGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r ); 
				// v3Pos = pInfo->m_pShapeSet->getNxActor()->getGlobalPosition();
				// I3TRACE("After v3Pos %f %f %f \n", v3Pos.x, v3Pos.y, v3Pos.z);
			}
		}
		else
			pInfo->m_pShapeSet->getNxActor()->setGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r);
	}
#else
	pInfo->m_pShapeSet->getNxActor()->moveGlobalPose( (NxMat34 &) *pInfo->m_Param.m_r ); 
#endif
}

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX

void i3PhysixContext::CheckInvalidRealValue(I3_PHYSIX_CMD_INFO * pInfo, REAL32& rValue)
{

	bool bResult = true;
	
	
	bResult = i3Math::isValid(rValue);
	// bResult = NxMath::isFinite(rValue);

	if(!bResult) {
		/*char szTemp[MAX_PATH] = {};
		i3::snprintf( szTemp, MAX_PATH, "CheckInvalidRealValue rValue %f Name %s ", rValue, 
			pInfo->m_pShapeSet->getEventReceiver()->GetNameString() );
		I3PRINTLOG(I3LOG_WARN, szTemp);*/

#ifdef FILL_ZERO_HEAP_AND_PHYSIX_DATA		
		rValue = 0.f;
#else
		int* iii = nullptr;
		*iii = 10;
#endif
	}
}

#endif // I3_CHECK_FLOAT_VALUE_FOR_PHYSICX

void i3PhysixContext::_OnSetLinearVelocity( I3_PHYSIX_CMD_INFO * pInfo)
{
	// Linear Velocity
	if( pInfo->m_pShapeSet->getNxActor() == nullptr)
		return;

	NxActor * pActor = pInfo->m_pShapeSet->getNxActor();
	if( !pActor->isDynamic() || pActor->readBodyFlag( NX_BF_KINEMATIC ))
		return;

	NxVec3 vel;
	VEC3D * pFBVel = pInfo->m_pShapeSet->getFBLinearVelocity();

	vel.x  = pInfo->m_Param.m_r[0] + pFBVel->x;
	vel.y  = pInfo->m_Param.m_r[1] + pFBVel->y;
	vel.z  = pInfo->m_Param.m_r[2] + pFBVel->z;

	
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
	{
		CheckInvalidRealValue(pInfo, vel.x);
		CheckInvalidRealValue(pInfo, vel.y);
		CheckInvalidRealValue(pInfo, vel.z);
		
	}
#endif


	pActor->setLinearVelocity( vel);
}

void i3PhysixContext::_OnSetAngularVelocity( I3_PHYSIX_CMD_INFO * pInfo)
{
	if( pInfo->m_pShapeSet->getNxActor() == nullptr)
		return;

	NxActor * pActor = pInfo->m_pShapeSet->getNxActor();
	if( !pActor->isDynamic() || pActor->readBodyFlag( NX_BF_KINEMATIC ) )
		return;

	NxVec3 vel;
	VEC3D * pFBVel = pInfo->m_pShapeSet->getFBAngularVelocity();

	// Angular Velocity
	vel.x  = pInfo->m_Param.m_r[0] + pFBVel->x;
	vel.y  = pInfo->m_Param.m_r[1] + pFBVel->y;
	vel.z  = pInfo->m_Param.m_r[2] + pFBVel->z;

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
	CheckInvalidRealValue(pInfo, vel.x);
	CheckInvalidRealValue(pInfo, vel.y);
	CheckInvalidRealValue(pInfo, vel.z);
#endif 

	pActor->setAngularVelocity( vel);
}

void i3PhysixContext::_OnSetDimension( I3_PHYSIX_CMD_INFO * pInfo)
{
	NxShape * pShape = (NxShape *) pInfo->m_pObject;

	switch( pShape->getType())
	{
		case NX_SHAPE_BOX :
			{
				NxBoxShape * pBox = (NxBoxShape *) pShape;
				NxVec3 sz;

				sz.set( pInfo->m_Param.m_r[0], pInfo->m_Param.m_r[1], pInfo->m_Param.m_r[2]);

				pBox->setDimensions( sz);
			}
			break;
	}
}

I3_PHYSIX_HIT_RESULT *	i3PhysixContext::LinearSweepOBB( i3BoundBox * pBox, MATRIX * pWorldMtx, VEC3D * pMotion, UINT32 grp)
{
	NxBox box;
	VEC3D c;
	VEC3D sz;
	NxSweepQueryHit result;

	pBox->GetCenter( &c);
	box.center.x = c.x;
	box.center.y = c.y;
	box.center.z = c.z;

	i3Vector::Sub( &sz, pBox->GetMax(), pBox->GetMin());
	box.extents.x = sz.x;
	box.extents.y = sz.y;
	box.extents.z = sz.z;

	box.rot.setRowMajorStride4( (const NxF32 *) pWorldMtx);

	m_pScene->linearOBBSweep( box, (NxVec3 &) pMotion, NX_SF_STATICS|NX_SF_DYNAMICS, nullptr, 1, &result, nullptr, grp);

	I3_PHYSIX_HIT_RESULT * pResult = getResult();

	pResult->m_FaceIndex	= result.faceID;
	pResult->m_T			= result.t;
	pResult->m_Intersect.x	= result.point.x;
	pResult->m_Intersect.y	= result.point.y;
	pResult->m_Intersect.z	= result.point.z;
	pResult->m_Normal.x		= result.normal.x;
	pResult->m_Normal.y		= result.normal.y;
	pResult->m_Normal.z		= result.normal.z;

	return pResult;
}


I3_PHYSIX_CONTACT_INFO * i3PhysixContext::AllocContactPoint( void)
{
	return (I3_PHYSIX_CONTACT_INFO*) m_pContactPointPool->Alloc( sizeof(I3_PHYSIX_CONTACT_INFO), __FILE__, __LINE__);
}

void i3PhysixContext::FreeContactPoint( I3_PHYSIX_CONTACT_INFO * pInfo)
{
	m_pContactPointPool->Free( pInfo);
}


