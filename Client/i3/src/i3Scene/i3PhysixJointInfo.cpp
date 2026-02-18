#include "i3SceneDef.h"
#include "i3PhysixJointInfo.h"

I3_CLASS_INSTANCE( i3PhysixJointInfo);

void	i3PhysixJointInfo::SetFrom( NxJointDesc * pDesc)
{
	INT32 i;

	m_Type = pDesc->getType();

	for( i = 0; i < 2; i++)
	{
		i3Vector::Set( &m_LocalAxis[i], pDesc->localAxis[i].x, pDesc->localAxis[i].y, pDesc->localAxis[i].z);
		i3Vector::Set( &m_LocalNormal[i], pDesc->localNormal[i].x, pDesc->localNormal[i].y, pDesc->localNormal[i].z);
		i3Vector::Set( &m_LocalAnchor[i], pDesc->localAnchor[i].x, pDesc->localAnchor[i].y, pDesc->localAnchor[i].z);
	}

	m_MaxForce = pDesc->maxForce;
	m_MaxTorque = pDesc->maxTorque;
	m_JointFlags = pDesc->jointFlags;
}

void	i3PhysixJointInfo::SetTo( NxJointDesc * pDesc)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		pDesc->localAxis[i].set( m_LocalAxis[i].x, m_LocalAxis[i].y, m_LocalAxis[i].z);
		pDesc->localNormal[i].set( m_LocalNormal[i].x, m_LocalNormal[i].y, m_LocalNormal[i].z);
		pDesc->localAnchor[i].set( m_LocalAnchor[i].x, m_LocalAnchor[i].y, m_LocalAnchor[i].z);
	}

	pDesc->maxForce = m_MaxForce;
	pDesc->maxTorque = m_MaxTorque;
	pDesc->jointFlags = m_JointFlags;
}

void i3PhysixJointInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PhysixJointInfo * pInfo = (i3PhysixJointInfo *) pDest;
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		pInfo->setLocalAxis( i, getLocalAxis( i));
		pInfo->setLocalNormal( i, getLocalNormal( i));
		pInfo->setLocalAnchor( i, getLocalAnchor( i));
	}

	pInfo->setMaxForce( getMaxForce());
	pInfo->setMaxTorque( getMaxTorque());
	pInfo->setJointFlags( getJointFlags());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PHYSIX_JOINT_INFO
	{
		UINT8			m_ID[4] = { 'P', 'J', 'I', '1' };
		i3::pack::VEC3D	m_LocalAxis[2];
		i3::pack::VEC3D	m_LocalNormal[2];
		i3::pack::VEC3D	m_LocalAnchor[2];
		REAL32			m_MaxForce = 0.0f;
		REAL32			m_MaxTorque = 0.0f;
		UINT32			m_JointFlags = 0;
		UINT32			pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3PhysixJointInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	pack::PHYSIX_JOINT_INFO data;
	
	for( i = 0; i < 2; i++)
	{
		data.m_LocalAxis[i].x = m_LocalAxis[i].x;
		data.m_LocalAxis[i].y = m_LocalAxis[i].y;
		data.m_LocalAxis[i].z = m_LocalAxis[i].z;

		data.m_LocalNormal[i].x = m_LocalNormal[i].x;
		data.m_LocalNormal[i].y = m_LocalNormal[i].y;
		data.m_LocalNormal[i].z = m_LocalNormal[i].z;

		data.m_LocalAnchor[i].x = m_LocalAnchor[i].x;
		data.m_LocalAnchor[i].y = m_LocalAnchor[i].y;
		data.m_LocalAnchor[i].z = m_LocalAnchor[i].z;
	}

	data.m_MaxForce = getMaxForce();
	data.m_MaxTorque = getMaxTorque();
	data.m_JointFlags = getJointFlags();

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3PhysixJointInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	INT32 i;
	pack::PHYSIX_JOINT_INFO data;

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);

	for( i = 0; i < 2; i++)
	{
		m_LocalAxis[i].x = data.m_LocalAxis[i].x;
		m_LocalAxis[i].y = data.m_LocalAxis[i].y;
		m_LocalAxis[i].z = data.m_LocalAxis[i].z;

		m_LocalNormal[i].x = data.m_LocalNormal[i].x;
		m_LocalNormal[i].y = data.m_LocalNormal[i].y;
		m_LocalNormal[i].z = data.m_LocalNormal[i].z;

		m_LocalAnchor[i].x = data.m_LocalAnchor[i].x;
		m_LocalAnchor[i].y = data.m_LocalAnchor[i].y;
		m_LocalAnchor[i].z = data.m_LocalAnchor[i].z;
	}

	setMaxForce( data.m_MaxForce);
	setMaxTorque( data.m_MaxTorque);
	setJointFlags( data.m_JointFlags);

	return Rc;
}
