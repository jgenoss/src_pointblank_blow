#ifndef __I3_SCENE_H__
#define __I3_SCENE_H__

#include "i3Scene/i3AllAttrs.h"

#include "i3Scene/i3AllNodes.h"

#include "i3Scene/i3AttrStack.h"
#include "i3Scene/i3AttrStackManager.h"
#include "i3Scene/i3CommonNodeProcs.h"
#include "i3Scene/i3SceneTracer.h"
#include "i3Scene/i3SceneFile.h"

#include "i3Scene/i3LayerRootNode.h"

#include "i3Scene/i3SceneDebug.h"
#include "i3Scene/i3SceneMeta.h"
#include "i3Scene/i3SceneUtil.h"

#include "i3Scene/i3Animation.h"
#include "i3Scene/i3Animation2.h"
#include "i3Scene/i3AnimationPackFile.h"

#include "i3Scene/i3Collidee.h"
#include "i3Scene/i3CollideeSphere.h"
#include "i3Scene/i3CollideeLine.h"
#include "i3Scene/i3CollideeMesh.h"
#include "i3Scene/i3CollideeCapsule.h"
#include "i3Scene/i3Collider.h"
#include "i3Scene/i3CollisionContext.h"
#include "i3Scene/i3CollisionSolver.h"
#include "i3Scene/i3CollideeGroup.h"

#include "i3Scene/i3Navigation.h"
#include "i3Scene/i3NavigationMesh.h"
#include "i3Scene/i3NavigationMeshNode.h"
#include "i3Scene/i3NavigationPointSlerp.h"

#ifndef I3_WINCE
	#include "i3Scene/i3IME.h"	
#endif

#include "i3Scene/i3Bone.h"
#include "i3Scene/i3Shape.h"
#include "i3Scene/i3Skeleton.h"

#include "i3Scene/i3RegData.h"
#include "i3Scene/i3RegKey.h"
#include "i3Scene/i3RegINT32.h"
#include "i3Scene/i3RegREAL32.h"
#include "i3Scene/i3RegString.h"
#include "i3Scene/i3RegVEC2D.h"
#include "i3Scene/i3RegVEC3D.h"
#include "i3Scene/i3RegVEC4D.h"
#include "i3Scene/i3RegCOLOR.h"
#include "i3Scene/i3RegMATRIX.h"
#include "i3Scene/i3Registry.h"
#include "i3Scene/i3RegistrySet.h"
#include "i3Scene/i3RegistryDef.h"

#if defined (I3_PHYSX)
	#include "i3Scene/i3PhysixContext.h"
	#include "i3Scene/i3PhysixJointInfo.h"
	#include "i3Scene/i3PhysixJointInfoD6.h"
	#include "i3Scene/i3PhysixController.h"
#endif

#include "i3Scene/i3ParticleRender.h"
#include "i3Scene/i3ParticleGroup.h"

#include "i3Scene/i3BSPDefine.h"

#endif //__I3_SCENE_H__
