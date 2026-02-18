// physX 용 글로벌 변수 정의
#include "pch.h"
#include "NxGlobal.h"

NxPhysicsSDK*		gPhysicsSDK = NULL;
DebugRenderer		gDebugRenderer;		
ErrorStream			gErrorStream;		
UserAllocator*		gAllocator = NULL;	
NxCookingInterface*	gCooking = NULL;

BOOL IsHardwarePresent()
{
	return ( gPhysicsSDK->getHWVersion() != NX_HW_VERSION_NONE );
}

const char* getNxSDKCreateError(const NxSDKCreateError& errorCode) 
{
	switch(errorCode) 
	{
		case NXCE_NO_ERROR: return "NXCE_NO_ERROR";
		case NXCE_PHYSX_NOT_FOUND: return "NXCE_PHYSX_NOT_FOUND";
		case NXCE_WRONG_VERSION: return "NXCE_WRONG_VERSION";
		case NXCE_DESCRIPTOR_INVALID: return "NXCE_DESCRIPTOR_INVALID";
		case NXCE_CONNECTION_ERROR: return "NXCE_CONNECTION_ERROR";
		case NXCE_RESET_ERROR: return "NXCE_RESET_ERROR";
		case NXCE_IN_USE_ERROR: return "NXCE_IN_USE_ERROR";
		default: return "Unknown error";
	}
};

BOOL InitNx()
{
	gAllocator = new UserAllocator;
	
	// Initialize PhysicsSDK
	gPhysicsSDK = NxCreatePhysicsSDK( NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream);
	if( gPhysicsSDK == NULL) 
	{
		DSERROR("[InitNx] Wrong SDK DLL version?\n");
		return FALSE;
	}

#ifdef USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if ( gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
	{
		 gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
	}
#endif
	
	return TRUE;
}

void ReleaseNx()
{
	NXU::releasePersistentMemory();

	if( gPhysicsSDK != NULL)
	{
		NxReleasePhysicsSDK( gPhysicsSDK );
		gPhysicsSDK = NULL;
	}
	
	NX_DELETE_SINGLE(gAllocator);
}


// test code
//------------------------
// Terrain Test
//------------------------

//
//void InitTerrain(NxScene* pScene)
//{
//	// Initialize terrain vertices
//	gTerrainVerts = new NxVec3[TERRAIN_NB_VERTS];
//	NxU32 y;
//	for(y=0;y<TERRAIN_SIZE;y++)
//	{
//		for(NxU32 x=0;x<TERRAIN_SIZE;x++)
//		{
//			gTerrainVerts[x+y*TERRAIN_SIZE] = NxVec3(NxF32(x)-(NxF32(TERRAIN_SIZE-1)*0.5f), 0.0f, NxF32(y)-(NxF32(TERRAIN_SIZE-1)*0.5f)) * TERRAIN_WIDTH;
//		}
//	}
//
//	// Initialize terrain faces
//	gTerrainFaces = new NxU32[TERRAIN_NB_FACES*3];
//	NxU32 k = 0;
//	for(NxU32 j=0;j<TERRAIN_SIZE-1;j++)
//	{
//		for(NxU32 i=0;i<TERRAIN_SIZE-1;i++)
//		{
//			// Create first triangle
//			gTerrainFaces[k++] = i   + j*TERRAIN_SIZE;
//			gTerrainFaces[k++] = i   + (j+1)*TERRAIN_SIZE;
//			gTerrainFaces[k++] = i+1 + (j+1)*TERRAIN_SIZE;
//
//			// Create second triangle
//			gTerrainFaces[k++] = i   + j*TERRAIN_SIZE;
//			gTerrainFaces[k++] = i+1 + (j+1)*TERRAIN_SIZE;
//			gTerrainFaces[k++] = i+1 + j*TERRAIN_SIZE;
//		}
//	}
//
//	// Initialize terrain colors
//	gTerrainColors = new NxVec3[TERRAIN_NB_VERTS];
//	for(y=0;y<TERRAIN_SIZE;y++)
//	{
//		for(NxU32 x=0;x<TERRAIN_SIZE;x++)
//		{
//			gTerrainColors[x+y*TERRAIN_SIZE].zero();
//		}
//	}
//
//	struct Local
//	{
//		static void _Compute(bool* done, NxVec3* field, NxU32 x0, NxU32 y0, NxU32 size, NxF32 value)
//		{
//			// Compute new size
//			size>>=1;
//			if(!size) return;
//
//			// Compute new heights
//			NxF32 v0 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
//			NxF32 v1 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
//			NxF32 v2 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
//			NxF32 v3 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
//			NxF32 v4 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
//
//			NxU32 x1 = (x0+size)		% TERRAIN_SIZE;
//			NxU32 x2 = (x0+size+size)	% TERRAIN_SIZE;
//			NxU32 y1 = (y0+size)		% TERRAIN_SIZE;
//			NxU32 y2 = (y0+size+size)	% TERRAIN_SIZE;
//
//			if(!done[x1 + y0*TERRAIN_SIZE])	field[x1 + y0*TERRAIN_SIZE].y = v0 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x2 + y0*TERRAIN_SIZE].y);
//			if(!done[x0 + y1*TERRAIN_SIZE])	field[x0 + y1*TERRAIN_SIZE].y = v1 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x0 + y2*TERRAIN_SIZE].y);
//			if(!done[x2 + y1*TERRAIN_SIZE])	field[x2 + y1*TERRAIN_SIZE].y = v2 + 0.5f * (field[x2 + y0*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
//			if(!done[x1 + y2*TERRAIN_SIZE])	field[x1 + y2*TERRAIN_SIZE].y = v3 + 0.5f * (field[x0 + y2*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
//			if(!done[x1 + y1*TERRAIN_SIZE])	field[x1 + y1*TERRAIN_SIZE].y = v4 + 0.5f * (field[x0 + y1*TERRAIN_SIZE].y + field[x2 + y1*TERRAIN_SIZE].y);
//
//			done[x1 + y0*TERRAIN_SIZE] = true;
//			done[x0 + y1*TERRAIN_SIZE] = true;
//			done[x2 + y1*TERRAIN_SIZE] = true;
//			done[x1 + y2*TERRAIN_SIZE] = true;
//			done[x1 + y1*TERRAIN_SIZE] = true;
//
//			// Recurse through 4 corners
//			value *= 0.5f;
//			_Compute(done, field, x0,	y0,	size, value);
//			_Compute(done, field, x0,	y1,	size, value);
//			_Compute(done, field, x1,	y0,	size, value);
//			_Compute(done, field, x1,	y1,	size, value);
//		}
//	};
//
//	// Fractalize
//	bool* done = new bool[TERRAIN_NB_VERTS];
//
//	memset(done, 0, TERRAIN_NB_VERTS);
//	gTerrainVerts[0].y = 10.0f;
//	gTerrainVerts[TERRAIN_SIZE-1].y = 10.0f;
//	gTerrainVerts[TERRAIN_SIZE*(TERRAIN_SIZE-1)].y = 10.0f;
//	gTerrainVerts[TERRAIN_NB_VERTS-1].y = 10.0f;
//	Local::_Compute(done, gTerrainVerts, 0, 0, TERRAIN_SIZE, TERRAIN_CHAOS);
//	for(NxU32 i=0;i<TERRAIN_NB_VERTS;i++)	gTerrainVerts[i].y += TERRAIN_OFFSET;
//
//	delete[] done;
//
//	// Build vertex normals
//	gTerrainNormals = new NxVec3[TERRAIN_NB_VERTS];
//	NxBuildSmoothNormals(TERRAIN_NB_FACES, TERRAIN_NB_VERTS, gTerrainVerts, gTerrainFaces, NULL, gTerrainNormals, true);
//
//	// Build physical model
//	NxTriangleMeshDesc terrainDesc;
//	terrainDesc.numVertices					= TERRAIN_NB_VERTS;
//	terrainDesc.numTriangles				= TERRAIN_NB_FACES;
//	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
//	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
//	terrainDesc.points						= gTerrainVerts;
//	terrainDesc.triangles					= gTerrainFaces;							
//	terrainDesc.flags						= 0;
//
//	terrainDesc.heightFieldVerticalAxis		= NX_Y;
//	terrainDesc.heightFieldVerticalExtent	= -1000.0f;
//
//	NxTriangleMeshShapeDesc terrainShapeDesc;
//
//	bool status = NxInitCooking();
//	if (!status) {
//		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
//		exit(1);
//	}
//
//	MemoryWriteBuffer buf;
//	status = NxCookTriangleMesh(terrainDesc, buf);
//	if (!status) {
//		printf("Unable to cook a triangle mesh.");
//		exit(1);
//	}
//
//	MemoryReadBuffer readBuffer(buf.data);
//	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(readBuffer);
//	//
//	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
//	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
//	//
//
//	NxActorDesc ActorDesc;
//	ActorDesc.shapes.pushBack(&terrainShapeDesc);
//	pScene->createActor(ActorDesc)->userData = (void*)0;
//
//	NxCloseCooking();
//}


//static void RenderTerrain()
//{
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	float* pVertList = new float[TERRAIN_NB_FACES*3*3];
//	float* pNormList = new float[TERRAIN_NB_FACES*3*3];
//	float* pColorList = new float[TERRAIN_NB_FACES*4*3];
//
//	int vertIndex = 0;
//	int normIndex = 0;
//	int colorIndex = 0;
//	for(int i=0;i<TERRAIN_NB_FACES;i++)
//	{
//		for(int j=0;j<3;j++)
//		{
//		pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].x;
//        pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].y;
//        pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].z;
//
//        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].x;
//        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].y;
//        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].z;
//
//		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].x;
//		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].y;
//		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].z;
//		pColorList[colorIndex++] = 1.0f;
//		}
//	}
//
//    glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3,GL_FLOAT, 0, pVertList);
//    glEnableClientState(GL_NORMAL_ARRAY);
//	glNormalPointer(GL_FLOAT, 0, pNormList);
//    glColorPointer(4,GL_FLOAT, 0, pColorList);
//    glEnableClientState(GL_COLOR_ARRAY);
//    glDrawArrays(GL_TRIANGLES, 0, TERRAIN_NB_FACES*3);
//	glDisableClientState(GL_COLOR_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);
//    glDisableClientState(GL_VERTEX_ARRAY);
//
//	delete[] pVertList;
//	delete[] pNormList;
//	delete[] pColorList;
//}

//test

//static bool				gPause = false;
//static bool				gUpdateColors = true;
//static NxBounds3		gSceneBounds;
//static NxF32			gElapsedTime = 0.0f;
//
//// Render code
//static NxVec3 Eye(130.0f, 100.0f, 130.0f);
//static NxVec3 Dir(-0.65,-0.4,-0.6);
//static NxVec3 N;
//static int mx = 0;
//static int my = 0;
//
//#define TERRAIN_SIZE		65
//#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
//#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
//#define TERRAIN_OFFSET		-20.0f
//#define TERRAIN_WIDTH		3.0f
//#define TERRAIN_CHAOS		30.0f
//#define ONE_OVER_RAND_MAX	(1.0f / float(RAND_MAX))
//#define HALF_RAND_MAX       (RAND_MAX >> 1)
//static NxVec3* gTerrainVerts = NULL;
//static NxVec3* gTerrainNormals = NULL;
//static NxU32* gTerrainFaces = NULL;
//static NxVec3* gTerrainColors = NULL;
//
//void InitTerrain();

//test end
