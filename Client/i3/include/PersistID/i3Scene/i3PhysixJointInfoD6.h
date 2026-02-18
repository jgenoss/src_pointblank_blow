#if !defined( __I3_PHYSIX_D6_JOINT_INFO_H)
#define __I3_PHYSIX_D6_JOINT_INFO_H

#if defined ( I3_PHYSX)

#include "i3PhysixJointInfo.h"

class I3_EXPORT_SCENE i3PhysixJointInfoD6 : public i3PhysixJointInfo
{
	I3_CLASS_DEFINE( i3PhysixJointInfoD6);
public:
	/**
	\brief Define the linear degrees of freedom

	<b>Default:</b> NX_D6JOINT_MOTION_FREE
	*/
	NxD6JointMotion			xMotion, yMotion, zMotion; 

	/**
	\brief Define the angular degrees of freedom

	<b>Default:</b> NX_D6JOINT_MOTION_FREE
	*/
	NxD6JointMotion			swing1Motion, swing2Motion, twistMotion;

	/**
	\brief If some linear DOF are limited, linearLimit defines the characteristics of these limits
	*/
	NxJointLimitSoftDesc		linearLimit;

	/**
	\brief If swing1Motion is NX_D6JOINT_MOTION_LIMITED, swing1Limit defines the characteristics of the limit
	*/
	NxJointLimitSoftDesc		swing1Limit; 
	
	/**
	\brief If swing2Motion is NX_D6JOINT_MOTION_LIMITED, swing2Limit defines the characteristics of the limit
	*/
	NxJointLimitSoftDesc		swing2Limit;
	
	/**
	\brief If twistMotion is NX_D6JOINT_MOTION_LIMITED, twistLimit defines the characteristics of the limit
	*/
	NxJointLimitSoftPairDesc	twistLimit;

	/* drive */
	
	/**
	\brief Drive the three linear DOF
	*/
	NxJointDriveDesc		xDrive, yDrive, zDrive;

	/**
	\brief These drives are used if the flag NX_D6JOINT_SLERP_DRIVE is not set
	*/
	NxJointDriveDesc		swingDrive, twistDrive;

	/**
	\brief This drive is used if the flag NX_D6JOINT_SLERP_DRIVE is set
	*/
	NxJointDriveDesc		slerpDrive;

	
	/**
	\brief If the type of xDrive (yDrive,zDrive) is NX_D6JOINT_DRIVE_POSITION, drivePosition defines the goal position

	<b>Range:</b> position vector<br>
	<b>Default:</b> Zero
	*/
	VEC3D					drivePosition;

	/**
	\brief If the type of swingDrive or twistDrive is NX_D6JOINT_DRIVE_POSITION, driveOrientation defines the goal orientation

	<b>Range:</b> unit quaternion<br>
	<b>Default:</b> Identity Quaternion
	*/
	QUATERNION				driveOrientation;

	/**
	\brief If the type of xDrive (yDrive,zDrive) is NX_D6JOINT_DRIVE_VELOCITY, driveLinearVelocity defines the goal linear velocity

	<b>Range:</b> velocity vector<br>
	<b>Default:</b> Zero
	*/
	VEC3D					driveLinearVelocity;
	
	/**
	\brief If the type of swingDrive or twistDrive is NX_D6JOINT_DRIVE_VELOCITY, driveAngularVelocity defines the goal angular velocity
	\li driveAngularVelocity.x - goal angular velocity about the twist axis
	\li driveAngularVelocity.y - goal angular velocity about the swing1 axis
	\li driveAngularVelocity.z - goal angular velocity about the swing2 axis

	<b>Range:</b> angular velocity vector<br>
	<b>Default:</b> Zero
	*/
	VEC3D					driveAngularVelocity;

	/**
	\brief If projectionMode is NX_JPM_NONE, projection is disabled. If NX_JPM_POINT_MINDIST, bodies are projected to limits leaving an linear error of projectionDistance and an angular error of projectionAngle

	<b>Default:</b> NX_JPM_NONE
	*/
	NxJointProjectionMode	projectionMode;
	
	/**
	\brief The distance above which to project the joint.

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> 0.1
	*/
	REAL32					projectionDistance;	
	
	/**
	\brief The angle above which to project the joint.

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> 0.0872f (about 5 degrees in radians)
	*/
	REAL32					projectionAngle;	

	/**
	\brief when the flag NX_D6JOINT_GEAR_ENABLED is set, the angular velocity of the second actor is driven towards the angular velocity of the first actor times gearRatio (both w.r.t. their primary axis)

	<b>Range:</b> (0,inf)<br>
	<b>Default:</b> 1.0
	*/
	REAL32					gearRatio;

	/**
	\brief This is a combination of the bits defined by ::NxD6JointFlag

	<b>Default:</b> 0
	*/
	UINT32 flags;


public:
	i3PhysixJointInfoD6(void);
	virtual ~i3PhysixJointInfoD6(void);

	virtual void	SetFrom( NxJointDesc * pDesc);
	virtual void	SetTo( NxJointDesc * pDesc);

	virtual void	CopyTo( i3ElementBase * pDesc, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif

#endif
