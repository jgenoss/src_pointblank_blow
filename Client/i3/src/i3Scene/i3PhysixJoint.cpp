#include "i3SceneDef.h"
#include "i3PhysixJoint.h"
#include "i3PhysixContext.h"
#include "i3Transform2.h"
#include "i3PhysixJointInfoD6.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_Streaming.h"
#include "NXU_cooking.h"

I3_CLASS_INSTANCE( i3PhysixJoint);

i3PhysixJoint::~i3PhysixJoint(void)
{
	if( m_pJoint != nullptr && g_pPhysixCtx != nullptr )
	{
		g_pPhysixCtx->CMD_releaseJoint( m_pJoint);
		m_pJoint = nullptr;
	}

	m_pParentBone = nullptr;
	m_pChildBone = nullptr;

	I3_SAFE_RELEASE( m_pInfo);
}

void i3PhysixJoint::Dump(void)
{
	INT32 i;
	NxVec3 vec;

	I3TRACE( "----------------\n");
	
	for( i = 0; i < 2; i++)
	{
	}
}

bool i3PhysixJoint::CreateJoint( NxJointDesc * pDesc)
{
	m_pJoint = g_pPhysixCtx->createJoint( pDesc);
	if( m_pJoint == nullptr )
		return false;

	m_pJoint->userData = this;

	// Desc. Data를 생성
	if( m_pInfo == nullptr)
	{
		m_pInfo = i3PhysixJointInfoD6::new_object();
	}
	
	m_pInfo->SetFrom( pDesc);

	return true;
}

bool i3PhysixJoint::Create( i3Transform2 * pParentBone, i3Transform2 * pChildBone)
{
	// 이미 Desc Data가 존재하는 상태에서의 생성
	NxD6JointDesc desc;

	I3ASSERT( m_pInfo != nullptr);

	m_pInfo->SetTo( &desc);

	desc.actor[0]	=	pParentBone->getShapeSet()->getNxActor();
	desc.actor[1]	=	pChildBone->getShapeSet()->getNxActor();

	m_pJoint = g_pPhysixCtx->createJoint( &desc);
	I3ASSERT( m_pJoint != nullptr);

	return true;
}

void i3PhysixJoint::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pInfo != nullptr)
	{
		m_pInfo->OnBuildObjectList( List);
	}

	// 2 Bone들에 대해서는 종속적인 관계에 있기 때문에
	// 추가하지 않는다.

	i3NamedElement::OnBuildObjectList( List);
}

void i3PhysixJoint::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3PhysixJoint * pJoint = (i3PhysixJoint *) pDest;

	// Desc Data는 항상 Ref. 방식으로 복사한다.
	pJoint->setJointInfo( getJointInfo());

	// i3PhysixJoint class는 능동적으로 동작하지 않으며,
	// i3Transform2 Node class에 의해 수동적으로 처리되어 진다.
	// Desc Data에 대한 Ref.만 복사해주고 i3Transform2 Node에서
	// 생성 및 각 Bone들과의 Link를 책임지도록 한다.
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PHYSIX_JOINT
	{
		UINT8		m_ID[4] = { 'P', 'X', 'J', '1' };
		OBJREF		m_pInfo = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3PhysixJoint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::PHYSIX_JOINT Data;

	// Node
	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// PhysX Data를 저장.
	Data.m_pInfo = pResFile->GetObjectPersistID( m_pInfo);

	Rc = pResFile->Write( &Data, sizeof(Data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3PhysixJoint::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::PHYSIX_JOINT Data;

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pResFile->Read( &Data, sizeof(Data));
	I3_CHKIO( Rc);
	Result += Rc;

	// Desc. Data
	if( Data.m_pInfo != 0)
	{
		i3PhysixJointInfo * pInfo = (i3PhysixJointInfo *) pResFile->FindObjectByID( Data.m_pInfo);
		I3ASSERT( pInfo != nullptr);

		setJointInfo( pInfo);
	}

	return Result;
}
