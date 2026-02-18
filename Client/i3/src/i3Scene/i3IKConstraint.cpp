#include "i3SceneDef.h"
#include "i3IKConstraint.h"

//I3_CLASS_INSTANCE( i3IKConstraint, i3PersistantElement);
I3_CLASS_INSTANCE( i3IKConstraint);

i3IKConstraint::i3IKConstraint()
{
	m_Type				= I3_IKCONSTRAINT_TYPE_NONE;
	m_rDistanceThresh	= 0.f;
	i3Matrix::Identity( &m_matTarget);
	m_pTargetMat		= NULL;

	m_iIKBoneStarterIdx	= 0;
	m_iIKBoneEffectorIdx = 0;
}

i3IKConstraint::~i3IKConstraint()
{
}


bool i3IKConstraint::CheckReachToTarget( VEC3D * pVec)
{
	VEC3D vDist;

	I3ASSERT( pVec);

	i3Vector::Sub( &vDist, i3Matrix::GetPos( &m_matTarget), pVec);

	if( i3Vector::Length( &vDist) < m_rDistanceThresh)
		return true;

	return false;
}

/*virtual*/ void i3IKConstraint::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::rtti_is_same<i3IKConstraint*>(pDest)); //->IsExactTypeOf( i3IKConstraint::GetClassMeta()) );

	i3IKConstraint * pIKConstraint = (i3IKConstraint*) pDest;

	pIKConstraint->setIKBoneStarterIdx( getIKBoneStarterIdx());
	pIKConstraint->setIKBoneEffectorIdx( getIKBoneEffectorIdx());
	pIKConstraint->setDistanceThresh( getDistanceThresh());
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistIKConstraint
{
	UINT8			m_ID[4];
	INT32			m_iIKStarterIdx;
	INT32			m_iIKEffectorIdx;
	REAL32			m_rDistanceThresh;
	UINT32			pad[4];
} I3_PERSIST_IKCONSTRAINT;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

/*virtual*/ UINT32 i3IKConstraint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_IKCONSTRAINT data;
	i3Stream * pStream = pResFile->GetStream();
	
	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "IKT1", 4);

	data.m_iIKStarterIdx = getIKBoneStarterIdx();
	data.m_iIKEffectorIdx = getIKBoneEffectorIdx();
	data.m_rDistanceThresh = getDistanceThresh();
	
	{
		Rc = pStream->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

/*virtual*/ UINT32 i3IKConstraint::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_IKCONSTRAINT data;

	Rc = pResFile->Read( &data, sizeof( I3_PERSIST_IKCONSTRAINT));
	I3_CHKIO( Rc);
	Result += Rc;

	I3ASSERT( memcmp( data.m_ID, "IKT1", 4) == 0 );
	
	setIKBoneStarterIdx( data.m_iIKStarterIdx);
	setIKBoneEffectorIdx( data.m_iIKEffectorIdx);
	setDistanceThresh( data.m_rDistanceThresh);

	return Result;
}

bool i3IKConstraint::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "Starter", m_iIKBoneStarterIdx);
	pXML->addAttr( "Effector", m_iIKBoneEffectorIdx);
	pXML->addAttr( "Thresh", m_rDistanceThresh);

	return true;
}

bool i3IKConstraint::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->getAttr( "Starter", &m_iIKBoneStarterIdx);
	pXML->getAttr( "Effector", &m_iIKBoneEffectorIdx);
	pXML->getAttr( "Thresh", &m_rDistanceThresh);

	return true;
}


