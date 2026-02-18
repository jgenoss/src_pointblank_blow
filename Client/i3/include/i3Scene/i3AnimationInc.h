#if !defined( __I3_ANIMATION_INC_H__)
#define __I3_ANIMATION_INC_H__

#include "Animation/i3AnimationThreadManager.h"
#include "Animation/i3AnimMemoryPool.h"
#include "Animation/i3AnimThread.h"
#include "Animation/i3AnimTreeNode.h"
#include "Animation/i3AnimTreeOutputNode.h"
#include "Animation/i3AnimTreeControlNode.h"
#include "Animation/i3AnimBlendListNode.h"
#include "Animation/i3AnimBlendNode.h"
#include "Animation/i3AnimBlendNodeByAim.h"
#include "Animation/i3AnimBlendNodeByDir.h"
#include "Animation/i3AnimBlendNodeByOffset.h"
#include "Animation/i3AnimBlendNodeByRandom.h"
#include "Animation/i3AnimBlendNodeBySpeed.h"
#include "Animation/i3AnimBlendNodeBySwitch.h"
#include "Animation/i3AnimBlendNodeByTwoWay.h"
#include "Animation/i3AnimBlendNodeByDividing.h"
#include "Animation/i3AnimPhysicsNode.h"
#include "Animation/i3AnimFilterBoneNode.h"
#include "Animation/i3AnimScaleNode.h"
#include "Animation/i3AnimScaleNodeBySpeed.h"
#include "Animation/i3AnimSequenceCutNode.h"
#include "Animation/i3AnimSequenceNode.h"
#include "Animation/i3AnimSequenceNodeByRaycast.h"
#include "Animation/i3AnimSequenceSetNode.h"
#include "Animation/i3AnimSkelControl.h"
#include "Animation/i3AnimSkelControlLookAt.h"
#include "Animation/i3AnimTimeSequenceNode.h"
#include "Animation/i3AnimTimeEventNode.h"

#include "i3IKConstraint.h"
#include "i3IKContext.h"
#include "Animation/i3AnimIKNode.h"
#include "Animation/i3AnimHumanIK.h"

#include "Animation/i3AnimationContext2.h"
#include "Animation/i3AnimSequenceGroup.h"

I3_EXPORT_SCENE void i3AnimationRegisterMetas( void);

#endif
