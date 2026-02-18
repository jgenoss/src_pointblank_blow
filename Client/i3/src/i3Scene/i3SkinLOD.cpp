#include "i3SceneDef.h"
#include "i3SkinLOD.h"
#include "i3SceneUtil.h"

I3_CLASS_INSTANCE( i3SkinLOD, i3PersistantElement);

i3SkinLOD::i3SkinLOD(void)
{
	m_pSkel = NULL;
	m_pShapeNode = NULL;
	m_pBoneMap = NULL;
}

i3SkinLOD::~i3SkinLOD(void)
{
	I3_SAFE_RELEASE( m_pSkel);
	I3_SAFE_RELEASE( m_pShapeNode);
	I3MEM_SAFE_FREE( m_pBoneMap);
}

void i3SkinLOD::SetBoneMap( INT32 * pTable, UINT32 Count)
{
	I3ASSERT( Count == (UINT32) m_pSkel->getBoneCount());

	I3MEM_SAFE_FREE( m_pBoneMap);

	m_pBoneMap = (INT32 *) i3MemAlloc( Count * sizeof(INT32));
	I3ASSERT( m_pBoneMap != NULL);

	if( pTable == NULL)
		return;

	i3mem::Copy( m_pBoneMap, pTable, Count * sizeof(INT32));
}

void i3SkinLOD::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	I3ASSERT( pDest->IsTypeOf( i3SkinLOD::GetClassMeta()));

	i3SkinLOD * pLOD = (i3SkinLOD *) pDest;

	if( m_pBoneMap != NULL)
	{
		pLOD->SetBoneMap( m_pBoneMap, getSkeleton()->getBoneCount());
	}

	if( method == I3_COPY_REF)
	{
		pLOD->setSkeleton( getSkeleton());
		pLOD->setShapeNode( getShapeNode());
	}
	else
	{
		if( getSkeleton() != NULL)
		{
			i3Skeleton * pSkel = i3Skeleton::NewObjectRef();

			getSkeleton()->CopyTo( pSkel, method);

			pLOD->setSkeleton( pSkel);
		}

		if( getShapeNode() != NULL)
		{
			i3Node * pNode = i3Scene::CreateClone( getShapeNode(), method);

			pLOD->setShapeNode( pNode);
		}
	}
}

void i3SkinLOD::OnBuildObjectList( i3List * pList)
{
	if( getSkeleton() != NULL)
		getSkeleton()->OnBuildObjectList( pList);

	if( getShapeNode() != NULL)
		getShapeNode()->OnBuildObjectList( pList);

	i3PersistantElement::OnBuildObjectList( pList);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct ALIGN8 _tagi3PersistSkinLOD
{
	UINT8		m_ID[4];
	OBJREF		m_pSkel;
	OBJREF		m_pShapeNode;
	UINT32		m_BoneMapCount;
	UINT8		pad[12];
} I3_PERSIST_SKINLOD;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3SkinLOD::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_SKINLOD data;

	memset( &data, 0, sizeof(data));
	memcpy( data.m_ID, "SLD1", 4);
	data.m_pSkel = (OBJREF) getSkeleton();
	data.m_pShapeNode = (OBJREF) getShapeNode();

	if( m_pBoneMap > 0)
	{
		data.m_BoneMapCount = getSkeleton()->getBoneCount();
	}

	Rc = pStream->Write( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_pBoneMap > 0)
	{
		Rc = pStream->Write( m_pBoneMap, sizeof(INT32) * data.m_BoneMapCount);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3SkinLOD::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_SKINLOD data;

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3FATAL( "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_pSkel != 0)
	{
		i3Skeleton * pSkel = (i3Skeleton *) pResFile->FindObjectByID( data.m_pSkel);
		I3ASSERT( pSkel != NULL);

		setSkeleton( pSkel);
	}

	if( data.m_pShapeNode != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( data.m_pShapeNode);
		I3ASSERT( pNode != NULL);

		setShapeNode( pNode);
	}

	if( data.m_BoneMapCount > 0)
	{
		SetBoneMap( NULL, data.m_BoneMapCount);
		Rc = pStream->Read( m_pBoneMap, sizeof(INT32) * data.m_BoneMapCount);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}
