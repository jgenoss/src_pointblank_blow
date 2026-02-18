#include "i3SceneDef.h"
#include "i3IKContext.h"
#include "i3Math.h"
#include "i3PhysixJointInfoD6.h"
#include "i3Skeleton.h"
#include "i3SceneObject.h"
#include "i3BoneRef.h"

#include "i3IKConstraint.h"

//I3_CLASS_INSTANCE( i3IKContext, i3PersistantElement);
I3_CLASS_INSTANCE( i3IKContext);

i3IKContext::i3IKContext()
{	
	m_iIKBoneCount		= 0;
	m_pIKBoneInfoArray	= NULL;

	m_pConstraint	= NULL;

	m_nIKStyle			= 0;

	
	m_szName[0] = 0;
}

i3IKContext::~i3IKContext()
{
	I3MEM_SAFE_FREE( m_pIKBoneInfoArray);
	
	I3_SAFE_RELEASE( m_pConstraint);
	

}

void i3IKContext::SetIKConstraint( i3IKConstraint * pConstraint)
{
	I3_REF_CHANGE( m_pConstraint, pConstraint);
}

void i3IKContext::_CreateIKBoneInfo( INT32 boneCount)
{
	I3ASSERT( m_pIKBoneInfoArray == NULL);

	m_pIKBoneInfoArray = (i3IKBoneInfo*) i3MemAlloc( sizeof(i3IKBoneInfo) * boneCount);
	i3mem::FillZero( m_pIKBoneInfoArray, sizeof( i3IKBoneInfo) * boneCount);

	m_iIKBoneCount = boneCount;
}

void i3IKContext::CreateIKConstraint( INT32 iStarterIdx, INT32 iEffectorIdx, REAL32 rDistanceThresh)
{
	INT32 prevStart = -1;
	INT32 prevCount = 0;
	if( m_pConstraint != NULL)
	{
		prevStart = m_pConstraint->getIKBoneStarterIdx();
		prevCount = m_iIKBoneCount;
	}

	if( m_pConstraint == NULL)
	{
		m_pConstraint = i3IKConstraint::new_object();
		I3ASSERT( m_pConstraint != NULL);
	}

	m_pConstraint->setIKBoneStarterIdx( iStarterIdx);
	m_pConstraint->setIKBoneEffectorIdx( iEffectorIdx);
	m_pConstraint->setDistanceThresh( rDistanceThresh);

	INT32 boneCount = (iEffectorIdx - iStarterIdx) + 1;

	if( boneCount > 0)
	{
		if( m_pIKBoneInfoArray == NULL)
		{
			_CreateIKBoneInfo( boneCount);
		}
		else if( m_iIKBoneCount < boneCount)
		{
			i3IKBoneInfo * pNewInfo = (i3IKBoneInfo*) i3MemAlloc( sizeof( i3IKBoneInfo) * boneCount);
			i3mem::FillZero( pNewInfo, sizeof(i3IKBoneInfo) * boneCount );
			I3ASSERT( prevStart != -1);

			INT32 start = prevStart - iStarterIdx;
			I3ASSERT( start >= 0);
			
			i3mem::Copy( &pNewInfo[ start], m_pIKBoneInfoArray, sizeof( i3IKBoneInfo) * prevCount);
			I3MEM_SAFE_FREE( m_pIKBoneInfoArray);

			m_pIKBoneInfoArray = pNewInfo;
		}
		else
		{
		}

		m_pIKBoneInfoArray[ 0].addFlag( I3_IKBONE_FLAG_STARTER);
		m_pIKBoneInfoArray[ boneCount-1].addFlag( I3_IKBONE_FLAG_EFFECTOR);
	}

	m_iIKBoneCount = boneCount;
}

INT32 i3IKContext::FindIKBone( INT32 iBoneIndex)
{
	INT32 i;
	
	for( i = 0; i < m_iIKBoneCount; i++)
	{	
		if( m_pIKBoneInfoArray[ i].getBoneIndex() == iBoneIndex)
			return i;
	}

	return -1;
}

bool i3IKContext::CheckDistance( MATRIX * pMat)
{	
	VEC3D vDir;

	I3ASSERT( m_pConstraint != NULL);

	i3Vector::Sub( &vDir, i3Matrix::GetPos( m_pConstraint->getTargetMatrix()), i3Matrix::GetPos( pMat));
	REAL32 rLen = i3Vector::Length( &vDir);

	if( rLen < m_pConstraint->getDistanceThresh() )
	{
		return true;
	}

	return false;
}


/*virtual*/ void i3IKContext::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	INT32 i;

	i3PersistantElement::CopyTo( pDest, method);

	I3ASSERT( i3::rtti_is_kind<i3IKContext*>(pDest)); //->IsTypeOf( i3IKContext::GetClassMeta()));

	i3IKContext * pCtx = (i3IKContext *) pDest;

	pCtx->setName( getName());
	pCtx->setIKBoneCount( getIKBoneCount());

	for( i = 0; i < getIKBoneCount(); i++)
	{
		pCtx->setIKBoneInfo( i, getIKBoneInfo( i));
	}

	{
		i3IKConstraint * pDest = i3IKConstraint::new_object();
		m_pConstraint->CopyTo( pDest, method);
		pCtx->SetIKConstraint( pDest);
	}
}

void i3IKContext::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3PersistantElement::OnBuildObjectList( List);

	if( m_pConstraint != NULL)
		m_pConstraint->OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

struct ALIGN4 I3_PERSIST_IKCONTEXT
{
	UINT8			m_ID[4];
	UINT8			m_szName[ 64];
	
	UINT32			m_iIKBoneCount;
	
	UINT32			pad[4];
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

/*virtual*/ UINT32 i3IKContext::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_IKCONTEXT data;
	i3Stream * pStream = pResFile->GetStream();

	
	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "IKC1", 4);

	i3mem::Copy( data.m_szName, m_szName, sizeof( m_szName));
	data.m_iIKBoneCount = getIKBoneCount();
	
	{
		Rc = pStream->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	// IKBoneInfo References
	if( getIKBoneCount() > 0)
	{
		Rc = pStream->Write( m_pIKBoneInfoArray, sizeof( i3IKBoneInfo) * getIKBoneCount());
		I3_CHKIO( Rc);
		Result += Rc;
	}

	OBJREF ref = 0;

	if( m_pConstraint != NULL)
		ref = pResFile->GetObjectPersistID( m_pConstraint);

	Rc = pStream->Write( &ref, sizeof( ref) );
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3IKContext::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_IKCONTEXT data;

	Rc = pResFile->Read( data.m_ID, 4);
	I3_CHKIO( Rc);
	Result += Rc;

	I3ASSERT( memcmp( data.m_ID, "IKC1", 4) == 0 );
	
	i3mem::Copy( m_szName, data.m_szName, sizeof( m_szName));
	setIKBoneCount( data.m_iIKBoneCount);

	// IKBoneInfo References
	if( getIKBoneCount() > 0)
	{
		Rc = pResFile->Read( m_pIKBoneInfoArray, sizeof( i3IKBoneInfo) * getIKBoneCount());
		I3_CHKIO( Rc);
		Result += Rc;
	}

	{
		OBJREF ref;

		Rc = pResFile->Read( &ref, sizeof( ref));
		I3_CHKIO( Rc);
		Result += Rc;

		if( ref != 0)
		{
			i3IKConstraint * pConstraint = (i3IKConstraint*) pResFile->FindObjectByID( ref);
			SetIKConstraint( pConstraint);
		}
	}

	return Result;
}

bool i3IKContext::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "Name", m_szName);
	pXML->addAttr( "IKBoneCount", m_iIKBoneCount);

	// IKBoneInfo References
	INT32 i;
	char szConv[ 32];
	for( i = 0; i < m_iIKBoneCount; i++)
	{
		i3String::Format( szConv, sizeof( szConv), "IKBone_%d", i);
		m_pIKBoneInfoArray[ i].SaveToXML( szConv, pXML);
	}

	OBJREF ref = 0;
	if( m_pConstraint != NULL)
		ref = pFile->GetObjectPersistID( m_pConstraint);

	pXML->addAttr( "Constraint", (INT32) ref);
	return true;
}

bool i3IKContext::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	const char * pszName = pXML->getAttr( "Name");
	if( pszName != NULL)
		i3String::Copy( m_szName, pszName, sizeof( m_szName));

	INT32 count;
	pXML->getAttr( "IKBoneCount", &count);
	setIKBoneCount( count);

	INT32 i;
	char szConv[32];
	for( i = 0; i < m_iIKBoneCount; i++)
	{
		i3String::Format( szConv, sizeof( szConv), "IKBone_%d", i);
		m_pIKBoneInfoArray[i].LoadFromXML( szConv, pXML);
	}

	OBJREF ref;
	pXML->getAttr( "Constraint", (INT32*) &ref);
	if( ref != 0)
	{
		i3IKConstraint * pConstraint = (i3IKConstraint*) pFile->FindObjectByID( ref);
		SetIKConstraint( pConstraint);
	}

	return true;
}

void i3IKContext::CreateRenderableRotateAxis( i3SceneObject * pSceneObject)
{
	/*
	INT32 i, j;
	i3VertexFormat format;

	if( m_pTestIKRotateAxisList == NULL)
		m_pTestIKRotateAxisList = i3::make_pscoped< i3::vector<i3BoneRef*> >();

	i3Node * pFindNode = i3Scene::FindNodeByName( pSceneObject, "IK_RotateAxis");
	if( pFindNode == NULL)
	{
		i3Node * pNode = i3Node::new_object_ref();
		pNode->SetName( "IK_RotateAxis");

		I3ASSERT( pSceneObject != NULL);
		pSceneObject->GetBody()->AddChild( pNode);

		format.SetHasPosition( true);
		format.SetHasColor( true);

		for( i = 0; i < pSceneObject->GetBody()->getFrameLOD()->getBoneCount(); i++)
		{
			i3BoneRef * pBoneRef = NULL;
			
			i3Skeleton2 * pSkel2 = pSceneObject->GetBody()->getFrameLOD()->getSkeleton2();

			if( pSkel2 == NULL)
			{
				pNode->RemoveFromParent();
				return;
			}

			i3Bone * pBone = pSkel2->getBone( i);

			if( !(pBone->getFlag() & I3_BONEFLAG_IS_IK) || (pBone->getFlag() & I3_BONEFLAG_IK_EFFECTOR) )
			{
				continue;
			}

			pBoneRef= i3BoneRef::new_object_ref();

			pBoneRef->setBoneIndex( i);
			pBoneRef->SetName( pBone->getName());

			
			for( j = 0; j < 7; j++)
			{
				i3Transform * pTrans = i3Transform::new_object_ref();
			
				pTrans->SetUpdateFrameID( 0xFFFFFFFF);
				pTrans->SetFlag( I3_NODEFLAG_TRANSFORM );
				pTrans->SetName( "renderable_Axis");
				i3Matrix::Identity( pTrans->GetMatrix());
				i3Matrix::Identity( pTrans->GetCacheMatrix());

				i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();

				{
					i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
					pAttr->Set( FALSE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
					pAttr->Set( FALSE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

					pAttr->Set( FALSE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

					pAttr->Set( I3G_FACE_CULL_FRONT);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();
					pAttr->Set( TRUE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();
					
					pAttr->SetSource( I3G_BLEND_SRCALPHA);
					pAttr->SetDestination( I3G_BLEND_DESTALPHA);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();

					pAttr->setBias( -0.002f);
					pAttrSet->AddAttr( pAttr);
				}

				// Geometry
				{
					i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

					i3VertexArray * pVA = i3VertexArray::new_object_ref();
					pVA->Create( &format, 2, 0);

					{
						VEC3D vPos;
						COLORREAL color;

						pVA->Lock();

						i3Vector::Set( &vPos, 0.f, 0.f, 0.f);
						pVA->SetPosition( 0, &vPos);
						
						if( j < 3)
							i3Vector::Set( &vPos, 0.8f, 0.f, 0.f);
						else if( j < 5)
							i3Vector::Set( &vPos, 0.8f, 0.f, 0.f);
						else
							i3Vector::Set( &vPos, 0.0f, 0.f, -0.8f);
						pVA->SetPosition( 1, &vPos);

						i3Color::Set( &color, 1.f, 1.f, 1.f, 0.5f);
						pVA->SetColor( 0, &color);

						if( j < 3)
							i3Color::Set( &color, (REAL32)(j%3 == 0)*0.5f + 0.5f, (REAL32)(j%3 == 0)*0.5f + 0.5f, 1.f, 0.6f);
						else if( j < 5)
							i3Color::Set( &color, 0.f, 1.f, 0.f, 0.6f);
						else
							i3Color::Set( &color, 1.f, 0.f, 0.f, 0.6f);
						
						pVA->SetColor( 1, &color);

						pVA->Unlock();
					}

					pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0, false );

					i3Geometry * pGeo = i3Geometry::new_object_ref();
					pGeo->AppendGeometryAttr( pGeoAttr);

					pTrans->AddChild( pGeo );

					pAttrSet->AddChild( pTrans);
				}

				pBoneRef->AddChild( pAttrSet);
			}

			pNode->AddChild( pBoneRef);

			m_pTestIKRotateAxisList->push_back( pBoneRef);
		}
	}
	else
	{
		for( i = 0; i < pFindNode->GetChildCount(); i++)
		{
			i3BoneRef * pBoneRef = (i3BoneRef*) pFindNode->GetChild( i);

			m_pTestIKRotateAxisList->push_back( pBoneRef);
		}
	}
	*/
}

