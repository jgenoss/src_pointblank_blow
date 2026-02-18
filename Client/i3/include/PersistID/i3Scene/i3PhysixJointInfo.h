#if !defined( __I3_PHYSIX_JOINT_INFO_H)
#define __I3_PHYSIX_JOINT_INFO_H

#if defined ( I3_PHYSX)

#include "i3PhysixDefine.h"
#include "i3PhysixUtil.h"

class I3_EXPORT_SCENE i3PhysixJointInfo : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3PhysixJointInfo);
protected:
	NxJointType	m_Type;
	VEC3D		m_LocalAxis[2];
	VEC3D		m_LocalNormal[2];
	VEC3D		m_LocalAnchor[2];
	REAL32		m_MaxForce;
	REAL32		m_MaxTorque;
	UINT32		m_JointFlags;

public:
	i3PhysixJointInfo(void);
	virtual ~i3PhysixJointInfo(void);

	virtual void	SetFrom( NxJointDesc * pDesc);
	virtual void	SetTo( NxJointDesc * pDesc);

	NxJointType	getType(void)					{ return m_Type; }

	VEC3D *		getLocalAxis( INT32 idx)		{ return &m_LocalAxis[idx]; }
	void		setLocalAxis( INT32 idx, VEC3D * pVec)
	{
		i3Vector::Copy( &m_LocalAxis[idx], pVec);
	}
	void		setLocalAxis( INT32 idx, REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_LocalAxis[idx], x, y, z);
	}

	VEC3D *		getLocalNormal( INT32 idx)		{ return &m_LocalNormal[idx]; }
	void		setLocalNormal( INT32 idx, VEC3D * pVec)
	{
		i3Vector::Copy( &m_LocalNormal[ idx], pVec);
	}
	void		setLocalNormal( INT32 idx, REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_LocalNormal[idx], x, y, z);
	}

	VEC3D *		getLocalAnchor( INT32 idx)		{ return &m_LocalAnchor[idx]; }
	void		setLocalAnchor( INT32 idx, VEC3D * pVec)
	{
		i3Vector::Copy( &m_LocalAnchor[idx], pVec);
	}

	REAL32		getMaxForce(void)				{ return m_MaxForce; }
	void		setMaxForce( REAL32 force)		{ m_MaxForce = force; }

	REAL32		getMaxTorque(void)				{ return m_MaxTorque; }
	void		setMaxTorque( REAL32 val)		{ m_MaxTorque = val; }

	UINT32		getJointFlags(void)				{ return m_JointFlags; }
	void		setJointFlags( UINT32 val)		{ m_JointFlags = val; }
	void		addJointFlags( UINT32 mask)		{ m_JointFlags |= mask; }
	void		removeJointFlags( UINT32 mask)	{ m_JointFlags &= ~mask; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pRes);
	virtual UINT32	OnLoad( i3ResourceFile * pRes);
};

#endif

#endif
