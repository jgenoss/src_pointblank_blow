#include "pch.h"
#include <crtdbg.h>
//#include <NxBodyDesc.h>
//#include "nxShapeSet.h"
//
//#include <float.h>
//#include <vector>
//
//
//// i3ObjList의 class meta를 위한 태그
//#define CLASS_MATRIX_OBJECT		0
//#define CLASS_TRANSFORM			1
//#define CLASS_NODE				2
//#define CLASS_MEMORY_BUFFER		3
//#define CLASS_VECTOR3_ARRAY		4
//#define CLASS_PHYSIX_SHAPE_SET	5
//#define CLASS_TRANSFORM2		6
//#define CLASS_SCENEGRAPH_INFO	7
//#define CLASS_LAST				8
//#define CLASS_UNKNWON			(-1)
//
//// tri-mesh 구분
//#define MEMORY_TYPE_TERRAIN		0
//#define MEMORY_TYPE_FACES		1
//
//
//
//// i3PersisitentElement가 포인트하는 충돌체의 class name
//static char* g_i3ClassNames[] = { 
//								"i3MatrixObject", 
//								"i3Transform",
//								"i3Node",
//								"i3MemoryBuffer",
//								"i3Vector3Array",
//								"i3PhysixShapeSet",
//								"i3Transform2",
//								"i3SceneGraphInfo" 
//};
//
//// string to index (case문에서 클래스를 식별하기 위함)
//static int GetTypeId(const char* className)
//{
//	for(int i = 0; i < CLASS_LAST; i++)
//	{
//		if(strcmp(g_i3ClassNames[i], className) == 0)
//			return i;
//	}
//
//	return -1;
//}
//



//// 리스트 변환 함수 
//NxU32	i3ListToNxList(const i3List* pInput, NxShapeSet** ppOutput, int maxItem)
//{
//	assert(pInput);
//	assert(ppOutput);
//
//	std::vector<NxMat34> stack;
//
//	int nSrcCount = pInput->m_Count;
//	int nDstIndex = 0;
//	int memType  = MEMORY_TYPE_TERRAIN; // terrain->face_list 순서로 toggle
//	if(!nSrcCount) return 0;
//
//	NxShapeSet* shapeSet	= new NxShapeSet; // temporary
//	assert(shapeSet);
//
//	for(int i = 0; i < nSrcCount && i < maxItem; i++)
//	{
//		i3PersistantElement* e = (i3PersistantElement *)pInput->GetItem(i);
//		NxShapeSet*			 d = shapeSet;
//		
//		char* className = e->GetMeta()->GetClassName();
//		switch(GetTypeId(className))
//		{
//		case	CLASS_MATRIX_OBJECT: 
//			{ 
//				
//				i3MatrixObject* obj = (i3MatrixObject*)e;
//				RT_MATRIX* i3m		= obj->GetMatrix();			// 4x4
//				
//				NxMat34	   nxm;
//				nxm.setColumnMajor44((NxF32*)i3m); // row major인지 column major인지 확인할 것.
//				
//				if(stack.size() == 0) // world matrix
//				{
//					d->m_matModelView = nxm;
//				}
//				else // local matrix
//				{
//					d->m_matLocalPose = nxm;
//				}
//
//				stack.push_back(nxm);
//				
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				
//				break; 
//			}
//		case	CLASS_TRANSFORM: 
//			{ 
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}		
//		case	CLASS_NODE: 
//			{ 
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		case	CLASS_MEMORY_BUFFER: 
//			{ 
//				if(memType == MEMORY_TYPE_TERRAIN)
//				{
//					i3MemoryBuffer* obj = (i3MemoryBuffer*)e;
//					d->m_numTerrainVectors = obj->GetSize() / 12;
//					d->m_pvTerrain = new NxVec3[d->m_numTerrainVectors];
//					ZeroMemory(d->m_pvTerrain, sizeof(NxVec3)*d->m_numTerrainVectors);
//					VEC3D* buf = (VEC3D*)obj->getBuffer();
//					
//					for(unsigned k = 0; k < d->m_numTerrainVectors; k++)
//					{
//						VEC3D tmp = buf[k];
//						NxVec3 v  = NxVec3(tmp.x, tmp.y, tmp.z);
//						v = d->m_matModelView * v;
//						
//						//d->m_pvTerrain[k] = NxVec3(buf[k * 3], buf[k * 3 + 1], buf[k * 3 + 2]);
//						d->m_pvTerrain[k] = v;
//
//					}
//					
//					memType = MEMORY_TYPE_FACES; // toggle
//				}
//				else
//				{
//					i3MemoryBuffer* obj = (i3MemoryBuffer*)e;
//					d->m_nbFaces = obj->GetSize() / 16; // 16byte. alignment  // eg 9179개
//					d->m_pfsFaces = new NxU32[d->m_nbFaces * 3];
//					ZeroMemory(d->m_pfsFaces, d->m_nbFaces * sizeof(NxU32) * 3);
//					NxU32* buf = (NxU32*)obj->getBuffer();
//					for (unsigned k = 0; k < d->m_nbFaces; k++)
//					{
//						d->m_pfsFaces[3 * k]	 = buf[4 * k];
//						d->m_pfsFaces[3 * k + 1] = buf[4 * k + 1];
//						d->m_pfsFaces[3 * k + 2] = buf[4 * k + 2];
//					}
//
//					memType = MEMORY_TYPE_TERRAIN; // toggle
//				}
//				
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		case CLASS_VECTOR3_ARRAY: // vertex list
//			{
//				
//				i3Vector3Array* obj = (i3Vector3Array*)e;
//				d->m_numVertices = obj->GetVectorCount();
//				d->m_pvVertList = new float[d->m_numVertices * 3];
//				ZeroMemory(d->m_pvVertList, sizeof(float) * 3 * d->m_numVertices);
//				CopyMemory(d->m_pvVertList, obj->GetVectorArray(), sizeof(float) * 3 * d->m_numVertices);
//				// local transform이 나중에 로드 되기 때문에 지금 포지션을 결정할 수 없다.
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break;
//			}
//		case	CLASS_PHYSIX_SHAPE_SET: 
//			{ 
//				i3PhysixShapeSet* obj = (i3PhysixShapeSet*)e;
//				
//				// set nx type
//				d->m_ShapeType = obj->m_ShapeType;
//
//				// copy collection (simple shape data : mesh인 것 같음)
//				if(obj->m_NxDataSize > 0)
//				{
//					d->m_NxDataSize = obj->m_NxDataSize;
//					d->m_pNxData = new char[d->m_NxDataSize];
//					CopyMemory(d->m_pNxData, obj->m_pNxData->getBuffer(), d->m_NxDataSize);
//				}
//
//				////////////////////////////////////////////////////
//				// descripton setting
//				////////////////////////////////////////////////////
//
//				//		-		obj	0x0277987c {m_ShapeType=I3_PHYSIX_SHAPE_TRIMESH m_dynamicState=0 m_pReceiver=0x00000000 ...}	i3PhysixShapeSet *
//				//+		i3EventReceiver	{s_pClassMetai3EventReceiver=0x0035ee50 }	i3EventReceiver
//				//+		s_pClassMetai3PhysixShapeSet	0x0035f278 s_ClassMetai3PhysixShapeSet	i3ClassMeta *
//				//		m_ShapeType	I3_PHYSIX_SHAPE_TRIMESH	I3_PHYSIX_SHAPE_TYPE
//				//		m_dynamicState	0	unsigned int
//				//+		m_pReceiver	0x00000000 {s_pClassMetai3EventReceiver=0x0035ee50 }	i3EventReceiver *
//				//		m_Usage	0	int
//				//		m_Format	FORMAT_BINARY	i3PhysixShapeSet::FORMAT
//				//+		m_pNxActor	0x00000000 {userData=??? }	NxActor *
//				//+		m_pNxJoint	0x00000000 {userData=??? appData=??? }	NxJoint *
//				//+		m_pTerrainArray	0x027794d4 {m_pPointer=0x02771cb4 m_Size=24 m_bOwnBuffer=true }	i3MemoryBuffer *
//				//		m_TerrainCount	1	int
//				//+		m_pFaceArray	0x02779664 {m_pPointer=0x049fa22c m_Size=146864 m_bOwnBuffer=true }	i3MemoryBuffer *
//				//		m_FaceCount	9179	unsigned int
//				//+		m_pPosArray	0x027797f4 {m_pVector=0x04a1e034 m_VectorCount=6417 }	i3Vector3Array *
//				
//			
//				//+		m_pJointInfo	0x00000000 {m_Type=??? m_LocalAxis=0x0000002c m_LocalNormal=0x00000044 ...}	i3PhysixJointInfo *
//				//		m_ShapeGroup	0	unsigned short
//				//		m_Restitution	0.0030000000	float
//				//		m_StaticFriction	0.30000001	float
//				//		m_DynamicFriction	0.30000001	float
//				
//				//+		m_vFBLinVelocity	{x=0.00000000 y=0.00000000 z=0.00000000 ...}	VEC3D
//				//+		m_vFBAngVelocity	{x=0.00000000 y=0.00000000 z=0.00000000 ...}	VEC3D
//				//		m_rFxForce	1.0000000	float
//				//+		m_vInertia	{x=0.00000000 y=0.00000000 z=0.00000000 ...}	VEC3D
//
//				//+		m_pNxData	0x04a30d5c {m_pPointer=0x04a30eec m_Size=246077 m_bOwnBuffer=true }	i3MemoryBuffer *
//				//		m_NxDataSize	246077	int
//				//		m_nVersion	3	int
//				//+		m_pCollectionData	0x00000000 {m_pPointer=??? m_Size=??? m_bOwnBuffer=??? }	i3MemoryBuffer *
//				//		m_nCollectionSize	0	int
//				//		m_ContactReportFlags	0	unsigned int
//				//+		m_pReports	0x00000000 {m_Events=??? m_pOwnShapeSet=??? m_pDestShapeSet=??? ...}	_tagi3PhysixContactReport *
//				//		m_nContactCount	0	int
//				//+		m_vOriginalSize	{x=1.0000000 y=1.0000000 z=1.0000000 ...}	VEC3D
//				//		m_pUserData	0x00000000	void *
//				//+		m_mtxUpdate	{m00=1.0000000 m01=0.00000000 m02=0.00000000 ...}	_tagi3Matrix
//				//+		m_mtxInitiate	{m00=1.0000000 m01=0.00000000 m02=0.00000000 ...}	_tagi3Matrix
//				//
//				
//				////////////////////////////////////////////////////
//				// set description
//				////////////////////////////////////////////////////
//				//	NX_INLINE void NxBodyDesc::setToDefault()
//				//	{
//				//		massLocalPose			.id();
//				//		massSpaceInertia		.zero();
//				//		linearVelocity			.zero();	//setNotUsed();	//when doing a loadFromDesc, the user expects to set the complete state, so this is not OK.
//				//		angularVelocity			.zero();	//setNotUsed();
//				//		wakeUpCounter			= 20.0f*0.02f;
//				//		mass					= 0.0f;
//				//		linearDamping			= 0.0f;
//				//		angularDamping			= 0.05f;
//				//		maxAngularVelocity		= -1.0f;
//				//		flags					= NX_BF_VISUALIZATION;
//				//		sleepLinearVelocity		= -1.0f;
//				//		sleepAngularVelocity	= -1.0f;
//				//		CCDMotionThreshold		= 0.0f;
//				//		solverIterationCount    = 4;
//				//		flags					|= NX_BF_ENERGY_SLEEP_TEST;
//				//		sleepEnergyThreshold	= -1.0f;
//				//		sleepDamping			= 0.0f;
//				//		contactReportThreshold  = NX_MAX_REAL;
//				//	}
//				// i3MatrixObject가 변환 될 때 디폴트 값으로 설정되었다.
//				NxBodyDesc* desc = &(d->m_bodyDesc);
//				desc->angularVelocity	= NxVec3(obj->m_vFBAngVelocity.x, obj->m_vFBAngVelocity.y, obj->m_vFBAngVelocity.z);
//				desc->linearVelocity	= NxVec3(obj->m_vFBLinVelocity.x, obj->m_vFBLinVelocity.y, obj->m_vFBLinVelocity.z);
//				desc->massSpaceInertia	= NxVec3(obj->m_vInertia.x, obj->m_vInertia.y, obj->m_vInertia.z);
//				
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		
//		case	CLASS_TRANSFORM2:  // object end - tag
//			{ 
//				i3Transform2* obj = (i3Transform2*)e;
//				// obj->m_pShapeSet이 항상 NULL이기 때문에 처리하지 않음
//				
//				// vertex position을 변경한다
//				// assert(stack.size() == 2);
//				
//				// 1. terrain vertex
//				NxMat34 m1  = stack[0]; // world
//				NxMat34 m2 = stack[1];	// local
//				m1 = m1 * m2;
//				for(int k = 0; k < d->m_numTerrainVectors; k++)
//				{
//					NxVec3 v = d->m_pvTerrain[k];
//					//v = m1 * v;
//					d->m_pvTerrain[k] = v;
//				}
//				// 2. mesh data
//				for(int k = 0; k < d->m_numVertices; k++)
//				{
//					NxVec3 v = ((NxVec3*)d->m_pvVertList)[k];
//					//v = m1 * v;
//					((NxVec3*)d->m_pvVertList)[k] = v;
//				}
//				
//				// 오브젝트 하나의 로드 마지막 단계이기 때문에 처리한다.
//				//d->m_bModelView = FALSE;		// 초기화 플래그 셋
//				memType = MEMORY_TYPE_TERRAIN;	// memType 초기화
//				stack.pop_back();
//				
//				ppOutput[nDstIndex] = shapeSet;
//				nDstIndex++;					// index 증가
//
//				shapeSet = new NxShapeSet;
//				assert(shapeSet);
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		case	CLASS_SCENEGRAPH_INFO:  // end of list -tag
//			{ 
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		case	CLASS_UNKNWON: 
//			{
//				fprintf(stderr, "%d, class name = %s\n", nDstIndex, className);
//				break; 
//			}
//		default: 
//			break;
//		}
//
//
//	}
//
//	delete shapeSet; // 마지막
//	
//	return nDstIndex; // 리스트에 보관된 충돌체 개수
//}
// 



