#ifndef __I3_ALL_NODES_H__
#define __I3_ALL_NODES_H__

#include "i3Node.h"
#include "i3Transform.h"
#include "i3AttrSet.h"
#include "i3Geometry.h"
#include "i3Camera.h"
#include "i3SceneObject.h"
#include "i3NodeSelector.h"
#include "i3LightSet.h"
#include "i3ColliderSet.h"
#include "i3ProjectiveTextureShader.h"
#include "i3ProjectiveShadowShader.h"
#include "i3Quad.h"
#include "i3Sprite.h"
#include "i3Sprite2D.h"
#include "i3Billboard.h"
#include "i3TimeSequence.h"
#include "i3TimeStamp.h"
#include "i3Particle.h"
#include "i3UserNode.h"
#include "i3TextNode.h"
#include "i3TextNodeDX.h"
#include "i3TextNodeDX2.h"
#include "i3Skin.h"
#include "i3BoneRef.h"

#if defined (I3G_DX)
	#include "i3ShadowMap.h"
	#include "i3LightSpacePerspectiveShadowMap.h"
	#include "i3Glow.h"
#endif

#include "i3Body.h"
#include "i3SoundNode.h"
#include "i3soundObject.h"

#if defined ( I3_PHYSX)
	#include "i3PhysixShapeSet.h"
	#include "i3Transform2.h"
	#include "i3ShapeSetContainer.h"
#endif

#include "i3OcTreeNode.h"
#include "i3Decal.h"
#include "i3BinaryPartitionNode.h"

#include "i3ParticleRenderNode.h"

#endif //__I3_ALL_NODES_H__
