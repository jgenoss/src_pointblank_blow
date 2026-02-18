#include "i3SceneDef.h"

#include "i3AllNodes.h"
#include "i3AllAttrs.h"

#include "i3AttrStack.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "i3SceneFile.h"

#include "i3LayerRootNode.h"

#include "i3Collidee.h"
#include "i3Collider.h"
#include "i3CollideeCylinder.h"
#include "i3CollideeGroup.h"
#include "i3CollideeLine.h"
#include "i3CollideeMesh.h"
#include "i3CollideeSphere.h"
#include "i3CollideeCapsule.h"
#include "i3CollideeCylinder.h"
#include "i3CollisionContext.h"
#include "i3CollisionSolver.h"

#include "i3Navigation.h"
#include "i3NavigationMesh.h"
#include "i3NavigationMeshNode.h"
#include "i3NavigationMeshPathFinder.h"

#include "i3NavigationPoint.h"
#include "i3NavigationPointPathFinder.h"

#include "i3LOD.h"
#include "i3SceneGraphInfo.h"
#include "i3SceneGraphContext.h"

#include "i3SceneGraphInfoResManager.h"

#include "i3RegData.h"
#include "i3RegKey.h"
#include "i3RegINT32.h"
#include "i3RegINT32Ex.h"
#include "i3RegREAL32.h"
#include "i3RegString.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"
#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"
#include "i3RegArray.h"
#include "i3RegistrySet.h"

#if defined ( I3_PHYSX)
	#include "i3PhysixContext.h"
	#include "i3PhysixShapeSet.h"
	#include "i3PhysixJointInfo.h"
	#include "i3PhysixJointInfoD6.h"
	#include "i3PhysixController.h"
#endif

#include "i3ParticleRender.h"
#include "i3ParticleGroup.h"

static bool s_bSceneMetaRegistered = false;

I3_EXPORT_SCENE
void i3SceneRegisterMetas(void)
{
	// Dependency
	i3BaseRegisterMetas();
	i3MathRegisterMetas();
	i3GfxRegisterMetas();
	i3InputRegisterMetas();
	i3SoundRegisterMetas();

	if( s_bSceneMetaRegistered == false)
	{
		i3NodeRegisterMetas();
		i3AttrRegisterMetas();

		{
			i3SceneGraphInfoResManager * pManager = i3SceneGraphInfoResManager::new_object_ref();

			i3ResourceFile::AddResourceManager( i3SceneGraphInfo::static_meta(), pManager);
		}

		s_bSceneMetaRegistered = true;
	}
}
#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3SceneRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return TRUE;
}

#endif
