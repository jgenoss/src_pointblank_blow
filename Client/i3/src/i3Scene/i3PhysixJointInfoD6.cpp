#include "i3SceneDef.h"
#include "i3PhysixJointInfoD6.h"

I3_CLASS_INSTANCE( i3PhysixJointInfoD6);

void	i3PhysixJointInfoD6::SetFrom( NxJointDesc * pDesc)
{
	NxD6JointDesc * pD6 = (NxD6JointDesc *) pDesc;

	i3PhysixJointInfo::SetFrom( pDesc);

	this->xMotion					= pD6->xMotion;
	this->yMotion					= pD6->yMotion;
	this->zMotion					= pD6->zMotion;
	this->swing1Motion				= pD6->swing1Motion;
	this->swing2Motion				= pD6->swing2Motion;
	this->twistMotion				= pD6->twistMotion;
	this->linearLimit				= pD6->linearLimit;
	this->swing1Limit				= pD6->swing1Limit;
	this->swing2Limit				= pD6->swing2Limit;
	this->twistLimit				= pD6->twistLimit;

	this->xDrive					= pD6->xDrive;
	this->yDrive					= pD6->yDrive;
	this->zDrive					= pD6->zDrive;

	this->swingDrive				= pD6->swingDrive;
	this->twistDrive				= pD6->twistDrive;
	this->slerpDrive				= pD6->slerpDrive;

	this->drivePosition.x			= pD6->drivePosition.x;
	this->drivePosition.y			= pD6->drivePosition.y;
	this->drivePosition.z			= pD6->drivePosition.z;

	i3Quat::Set( &this->driveOrientation, pD6->driveOrientation.x, pD6->driveOrientation.y, pD6->driveOrientation.z, pD6->driveOrientation.w);

	this->driveLinearVelocity.x		= pD6->driveLinearVelocity.x;
	this->driveLinearVelocity.y		= pD6->driveLinearVelocity.y;
	this->driveLinearVelocity.z		= pD6->driveLinearVelocity.z;

	this->driveAngularVelocity.x	= pD6->driveAngularVelocity.x;
	this->driveAngularVelocity.y	= pD6->driveAngularVelocity.y;
	this->driveAngularVelocity.z	= pD6->driveAngularVelocity.z;

	this->projectionMode			= pD6->projectionMode;
	this->projectionDistance		= pD6->projectionDistance;
	this->projectionAngle			= pD6->projectionAngle;

	this->gearRatio					= pD6->gearRatio;
	this->flags						= pD6->flags;
}

void	i3PhysixJointInfoD6::SetTo( NxJointDesc * pDesc)
{
	NxD6JointDesc * pD6 = (NxD6JointDesc *) pDesc;

	i3PhysixJointInfo::SetTo( pDesc);

	pD6->xMotion					= this->xMotion;
	pD6->yMotion					= this->yMotion;
	pD6->zMotion					= this->zMotion;
	pD6->swing1Motion				= this->swing1Motion;
	pD6->swing2Motion				= this->swing2Motion;
	pD6->twistMotion				= this->twistMotion;
	pD6->linearLimit				= this->linearLimit;
	// 임시로 각 리미트값을 조정합니다. 캐릭터에만 해당되야 합니다.
	//pD6->linearLimit.damping		*= 1.f;
	//pD6->linearLimit.restitution	*= 0.3f;
	//pD6->linearLimit.spring			*= 0.1f;
	//pD6->linearLimit.value			*= 0.5f;
	pD6->swing1Limit				= this->swing1Limit;
	//pD6->swing1Limit.damping		= this->swing1Limit.damping * 1.f;
	//if( this->swing1Limit.value > 0.f )
	{
	//	pD6->swing1Limit.restitution	= this->swing1Limit.restitution * 0.7f;
	//	pD6->swing1Limit.spring			= this->swing1Limit.spring * 0.8f;
	}
	pD6->swing2Limit				= this->swing2Limit;
	//pD6->swing2Limit.damping		= this->swing2Limit.damping * 1.f;
	//if( this->swing2Limit.value > 0.f )
	{
	//	pD6->swing2Limit.restitution	= this->swing2Limit.restitution * 0.7f;
	//	pD6->swing2Limit.spring			= this->swing2Limit.spring * 0.8f;
	}
	pD6->twistLimit					= this->twistLimit;

	pD6->xDrive						= this->xDrive;
	pD6->yDrive						= this->yDrive;
	pD6->zDrive						= this->zDrive;

	pD6->swingDrive					= this->swingDrive;
	pD6->twistDrive					= this->twistDrive;
	pD6->slerpDrive					= this->slerpDrive;

	pD6->drivePosition.x			= this->drivePosition.x;
	pD6->drivePosition.y			= this->drivePosition.y;
	pD6->drivePosition.z			= this->drivePosition.z;

	pD6->driveOrientation.x			= this->driveOrientation.x;
	pD6->driveOrientation.y			= this->driveOrientation.y;
	pD6->driveOrientation.z			= this->driveOrientation.z;
	pD6->driveOrientation.w			= this->driveOrientation.w;

	pD6->driveLinearVelocity.x		= this->driveLinearVelocity.x;
	pD6->driveLinearVelocity.y		= this->driveLinearVelocity.y;
	pD6->driveLinearVelocity.z		= this->driveLinearVelocity.z;

	pD6->driveAngularVelocity.x		= this->driveAngularVelocity.x;
	pD6->driveAngularVelocity.y		= this->driveAngularVelocity.y;
	pD6->driveAngularVelocity.z		= this->driveAngularVelocity.z;

	pD6->projectionMode				= this->projectionMode;
	pD6->projectionDistance			= this->projectionDistance;
	pD6->projectionAngle			= this->projectionAngle;

	pD6->gearRatio					= this->gearRatio;
	pD6->flags						= this->flags;
}

void	i3PhysixJointInfoD6::CopyTo( i3ElementBase * pDesc, I3_COPY_METHOD method)
{
	i3PhysixJointInfo::CopyTo( pDesc, method);

	i3PhysixJointInfoD6 * pD6 = (i3PhysixJointInfoD6 *) pDesc;

	pD6->xMotion					= this->xMotion;
	pD6->yMotion					= this->yMotion;
	pD6->zMotion					= this->zMotion;
	pD6->swing1Motion				= this->swing1Motion;
	pD6->swing2Motion				= this->swing2Motion;
	pD6->twistMotion				= this->twistMotion;
	pD6->linearLimit				= this->linearLimit;
	pD6->swing1Limit				= this->swing1Limit;
	pD6->swing2Limit				= this->swing2Limit;
	pD6->twistLimit					= this->twistLimit;

	pD6->xDrive						= this->xDrive;
	pD6->yDrive						= this->yDrive;
	pD6->zDrive						= this->zDrive;

	pD6->swingDrive					= this->swingDrive;
	pD6->twistDrive					= this->twistDrive;
	pD6->slerpDrive					= this->slerpDrive;

	pD6->drivePosition.x			= this->drivePosition.x;
	pD6->drivePosition.y			= this->drivePosition.y;
	pD6->drivePosition.z			= this->drivePosition.z;

	pD6->driveOrientation.x			= this->driveOrientation.x;
	pD6->driveOrientation.y			= this->driveOrientation.y;
	pD6->driveOrientation.z			= this->driveOrientation.z;
	pD6->driveOrientation.w			= this->driveOrientation.w;

	pD6->driveLinearVelocity.x		= this->driveLinearVelocity.x;
	pD6->driveLinearVelocity.y		= this->driveLinearVelocity.y;
	pD6->driveLinearVelocity.z		= this->driveLinearVelocity.z;

	pD6->driveAngularVelocity.x		= this->driveAngularVelocity.x;
	pD6->driveAngularVelocity.y		= this->driveAngularVelocity.y;
	pD6->driveAngularVelocity.z		= this->driveAngularVelocity.z;

	pD6->projectionMode				= this->projectionMode;
	pD6->projectionDistance			= this->projectionDistance;
	pD6->projectionAngle			= this->projectionAngle;

	pD6->gearRatio					= this->gearRatio;
	pD6->flags						= this->flags;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct NXJOINTLIMITDESC
	{
		REAL32		value = 0.0f;
		REAL32		spring = 0.0f;
		REAL32		damping = 0.0f;
		REAL32		restitution = 0.0f;
	};

	struct NXJOINTDRIVEDESC
	{
		UINT32		driveType = 0;
		REAL32		spring = 0.0f;
		REAL32		damping = 0.0f;
		REAL32		forceLimit = 0.0f;
	};

	struct PHYSIX_JOINT_INFO_D6
	{
		INT32						xMotion = 0;
		INT32						yMotion = 0;
		INT32						zMotion = 0;
		INT32						swing1Motion = 0;
		INT32						swing2Motion = 0;
		INT32						twistMotion = 0;

		NXJOINTLIMITDESC	linearLimit;
		NXJOINTLIMITDESC	swing1Limit;
		NXJOINTLIMITDESC	swing2Limit;
		NXJOINTLIMITDESC	twistLimit_low;
		NXJOINTLIMITDESC	twistLimit_high;

		NXJOINTDRIVEDESC	xDrive;
		NXJOINTDRIVEDESC	yDrive;
		NXJOINTDRIVEDESC	zDrive;

		NXJOINTDRIVEDESC	swingDrive;
		NXJOINTDRIVEDESC	twistDrive;
		NXJOINTDRIVEDESC	slerpDrive;

		i3::pack::VEC3D				drivePosition;
		i3::pack::VEC4D				driveOrientation;
		i3::pack::VEC3D				driveLinearVelocity;
		i3::pack::VEC3D				driveAngularVelocity;

		INT32						projectionMode = 0;
		REAL32						projectionDistance = 0.0f;
		REAL32						projectionAngle = 0.0f;
		REAL32						gearRatio = 0.0f;

		UINT32						flags = 0;

		UINT32						pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3PhysixJointInfoD6::OnSave( i3ResourceFile * pResFile)
{
	pack::PHYSIX_JOINT_INFO_D6 data;
	UINT32 Rc, Result;

	Result = i3PhysixJointInfo::OnSave( pResFile);
	I3_CHKIO( Result);

	data.xMotion					= (INT32) this->xMotion;
	data.yMotion					= (INT32) this->yMotion;
	data.zMotion					= (INT32) this->zMotion;
	data.swing1Motion				= (INT32) this->swing1Motion;
	data.swing2Motion				= (INT32) this->swing2Motion;
	data.twistMotion				= (INT32) this->twistMotion;

	data.linearLimit.value			= this->linearLimit.value;
	data.linearLimit.spring			= this->linearLimit.spring;
	data.linearLimit.damping		= this->linearLimit.damping;
	data.linearLimit.restitution	= this->linearLimit.restitution;

	data.swing1Limit.value			= this->swing1Limit.value;
	data.swing1Limit.spring			= this->swing1Limit.spring;
	data.swing1Limit.damping		= this->swing1Limit.damping;
	data.swing1Limit.restitution	= this->swing1Limit.restitution;

	data.swing2Limit.value			= this->swing2Limit.value;
	data.swing2Limit.spring			= this->swing2Limit.spring;
	data.swing2Limit.damping		= this->swing2Limit.damping;
	data.swing2Limit.restitution	= this->swing2Limit.restitution;

	data.twistLimit_low.value		= this->twistLimit.low.value;
	data.twistLimit_low.spring		= this->twistLimit.low.spring;
	data.twistLimit_low.damping		= this->twistLimit.low.damping;
	data.twistLimit_low.restitution	= this->twistLimit.low.restitution;

	data.twistLimit_high.value		= this->twistLimit.high.value;
	data.twistLimit_high.spring		= this->twistLimit.high.spring;
	data.twistLimit_high.damping	= this->twistLimit.high.damping;
	data.twistLimit_high.restitution= this->twistLimit.high.restitution;

	data.xDrive.driveType			= this->xDrive.driveType;
	data.xDrive.spring				= this->xDrive.spring;
	data.xDrive.damping				= this->xDrive.damping;
	data.xDrive.forceLimit			= this->xDrive.forceLimit;

	data.yDrive.driveType			= this->yDrive.driveType;
	data.yDrive.spring				= this->yDrive.spring;
	data.yDrive.damping				= this->yDrive.damping;
	data.yDrive.forceLimit			= this->yDrive.forceLimit;

	data.zDrive.driveType			= this->zDrive.driveType;
	data.zDrive.spring				= this->zDrive.spring;
	data.zDrive.damping				= this->zDrive.damping;
	data.zDrive.forceLimit			= this->zDrive.forceLimit;

	data.swingDrive.driveType		= this->swingDrive.driveType;
	data.swingDrive.spring			= this->swingDrive.spring;
	data.swingDrive.damping			= this->swingDrive.damping;
	data.swingDrive.forceLimit		= this->swingDrive.forceLimit;

	data.twistDrive.driveType		= this->twistDrive.driveType;
	data.twistDrive.spring			= this->twistDrive.spring;
	data.twistDrive.damping			= this->twistDrive.damping;
	data.twistDrive.forceLimit		= this->twistDrive.forceLimit;

	data.slerpDrive.driveType		= this->slerpDrive.driveType;
	data.slerpDrive.spring			= this->slerpDrive.spring;
	data.slerpDrive.damping			= this->slerpDrive.damping;
	data.slerpDrive.forceLimit		= this->slerpDrive.forceLimit;

	data.drivePosition.x			= this->drivePosition.x;
	data.drivePosition.y			= this->drivePosition.y;
	data.drivePosition.z			= this->drivePosition.z;

	data.driveOrientation.x			= this->driveOrientation.x;
	data.driveOrientation.y			= this->driveOrientation.y;
	data.driveOrientation.z			= this->driveOrientation.z;
	data.driveOrientation.w			= this->driveOrientation.w;

	data.driveLinearVelocity.x		= this->driveLinearVelocity.x;
	data.driveLinearVelocity.y		= this->driveLinearVelocity.y;
	data.driveLinearVelocity.z		= this->driveLinearVelocity.z;

	data.driveAngularVelocity.x		= this->driveAngularVelocity.x;
	data.driveAngularVelocity.y		= this->driveAngularVelocity.y;
	data.driveAngularVelocity.z		= this->driveAngularVelocity.z;

	data.projectionMode				= this->projectionMode;
	data.projectionDistance			= this->projectionDistance;
	data.projectionAngle			= this->projectionAngle;

	data.gearRatio					= this->gearRatio;
	data.flags						= this->flags;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3PhysixJointInfoD6::OnLoad( i3ResourceFile * pResFile)
{
	pack::PHYSIX_JOINT_INFO_D6 data;
	UINT32 Rc, Result;

	Result = i3PhysixJointInfo::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	xMotion						= (NxD6JointMotion) data.xMotion;
	yMotion						= (NxD6JointMotion) data.yMotion;
	zMotion						= (NxD6JointMotion) data.zMotion;
	swing1Motion				= (NxD6JointMotion) data.swing1Motion;
	swing2Motion				= (NxD6JointMotion) data.swing2Motion;
	twistMotion					= (NxD6JointMotion) data.twistMotion;

	linearLimit.value			= data.linearLimit.value;
	linearLimit.spring			= data.linearLimit.spring;
	linearLimit.damping			= data.linearLimit.damping;
	linearLimit.restitution		= data.linearLimit.restitution;

	swing1Limit.value			= data.swing1Limit.value;
	swing1Limit.spring			= data.swing1Limit.spring;
	swing1Limit.damping			= data.swing1Limit.damping;
	swing1Limit.restitution		= data.swing1Limit.restitution;

	swing2Limit.value			= data.swing2Limit.value;
	swing2Limit.spring			= data.swing2Limit.spring;
	swing2Limit.damping			= data.swing2Limit.damping;
	swing2Limit.restitution		= data.swing2Limit.restitution;

	twistLimit.low.value		= data.twistLimit_low.value;
	twistLimit.low.spring		= data.twistLimit_low.spring;
	twistLimit.low.damping		= data.twistLimit_low.damping;
	twistLimit.low.restitution	= data.twistLimit_low.restitution;

	twistLimit.high.value		= data.twistLimit_high.value;
	twistLimit.high.spring		= data.twistLimit_high.spring;
	twistLimit.high.damping		= data.twistLimit_high.damping;
	twistLimit.high.restitution	= data.twistLimit_high.restitution;

	xDrive.driveType			= data.xDrive.driveType;
	xDrive.spring				= data.xDrive.spring;
	xDrive.damping				= data.xDrive.damping;
	xDrive.forceLimit			= data.xDrive.forceLimit;

	yDrive.driveType			= data.yDrive.driveType;
	yDrive.spring				= data.yDrive.spring;
	yDrive.damping				= data.yDrive.damping;
	yDrive.forceLimit			= data.yDrive.forceLimit;

	zDrive.driveType			= data.zDrive.driveType;
	zDrive.spring				= data.zDrive.spring;
	zDrive.damping				= data.zDrive.damping;
	zDrive.forceLimit			= data.zDrive.forceLimit;

	swingDrive.driveType		= data.swingDrive.driveType;
	swingDrive.spring			= data.swingDrive.spring;
	swingDrive.damping			= data.swingDrive.damping;
	swingDrive.forceLimit		= data.swingDrive.forceLimit;

	twistDrive.driveType		= data.twistDrive.driveType;
	twistDrive.spring			= data.twistDrive.spring;
	twistDrive.damping			= data.twistDrive.damping;
	twistDrive.forceLimit		= data.twistDrive.forceLimit;

	slerpDrive.driveType		= data.slerpDrive.driveType;
	slerpDrive.spring			= data.slerpDrive.spring;
	slerpDrive.damping			= data.slerpDrive.damping;
	slerpDrive.forceLimit		= data.slerpDrive.forceLimit;

	drivePosition.x				= data.drivePosition.x;
	drivePosition.y				= data.drivePosition.y;
	drivePosition.z				= data.drivePosition.z;

	driveOrientation.x			= data.driveOrientation.x;
	driveOrientation.y			= data.driveOrientation.y;
	driveOrientation.z			= data.driveOrientation.z;
	driveOrientation.w			= data.driveOrientation.w;

	driveLinearVelocity.x		= data.driveLinearVelocity.x;
	driveLinearVelocity.y		= data.driveLinearVelocity.y;
	driveLinearVelocity.z		= data.driveLinearVelocity.z;

	driveAngularVelocity.x		= data.driveAngularVelocity.x;
	driveAngularVelocity.y		= data.driveAngularVelocity.y;
	driveAngularVelocity.z		= data.driveAngularVelocity.z;

	projectionMode				= (NxJointProjectionMode) data.projectionMode;
	projectionDistance			= data.projectionDistance;
	projectionAngle				= data.projectionAngle;

	gearRatio					= data.gearRatio;
	flags						= data.flags;

	return Result;
}
