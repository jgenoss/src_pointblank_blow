#include "i3SceneDef.h"
#include "i3Bone2MatrixListAttr.h"
#include "i3AnimationContext.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3Bone2MatrixListAttr, i3BoneMatrixListAttr);

i3Bone2MatrixListAttr::i3Bone2MatrixListAttr(void)
{
	m_pBone2 = NULL;
}

i3Bone2MatrixListAttr::~i3Bone2MatrixListAttr(void)
{
	I3MEM_SAFE_FREE( m_pBone2);
}

void i3Bone2MatrixListAttr::SetBoneCount( INT32 count)
{
	if( m_Count != count || m_pBone2 == NULL)
	{
		m_Count = count;

		I3MEM_SAFE_FREE( m_pBone2);
		m_pBone2 = (i3Bone2 *) i3MemAlloc( sizeof(i3Bone2) * count);
		I3ASSERT( m_pBone2 != NULL);
		i3mem::FillZero( m_pBone2, sizeof( i3Bone2) * count);

		I3_SAFE_RELEASE( m_pTxBoneArray);
		m_pTxBoneArray = i3MatrixArray::NewObject();
		m_pTxBoneArray->Create( count);
	}
}

void i3Bone2MatrixListAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	if( pDest->IsTypeOf( i3Bone2MatrixListAttr::GetClassMeta()) == FALSE)
		return;

	i3PersistantElement::CopyTo( pDest, method);

	i3Bone2MatrixListAttr * pAttr = (i3Bone2MatrixListAttr *) pDest;

	if( getBoneCount() > 0)
	{
		pAttr->SetBoneCount( getBoneCount());

		memcpy( pAttr->m_pBone2, m_pBone2, sizeof(i3Bone2) * getBoneCount());
	}

	if( m_pInvBoneArray != NULL)
	{
		m_pInvBoneArray->CopyTo( pAttr->m_pInvBoneArray, method);
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistBone2MatrixListAttr
{
	UINT8		m_ID[4];
	INT32		m_Count;
	OBJREF		m_pInvBoneArray;
	UINT32		pad[8];
} I3_PERSIST_BONE2_MATRIX_ATTR;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3Bone2MatrixListAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_BONE2_MATRIX_ATTR data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "BMA2", 4);
	data.m_Count = getBoneCount();
	data.m_pInvBoneArray = (OBJREF) pResFile->GetObjectPersistID( getInverseBoneArray());

	// Count
	{
		Rc = pStream->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	// Bone References
	if( getBoneCount() > 0)
	{
		Rc = pStream->Write( m_pBone2, sizeof(i3Bone2) * getBoneCount());
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3Bone2MatrixListAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3_PERSIST_BONE2_MATRIX_ATTR data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_Count > 0)
	{
		SetBoneCount( data.m_Count);

		Rc = pStream->Read( m_pBone2, sizeof(i3Bone2) * getBoneCount());
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( (OBJREF) data.m_pInvBoneArray != 0)
	{
		i3MatrixArray * pArray = (i3MatrixArray *) pResFile->FindObjectByID( (OBJREF) data.m_pInvBoneArray);
		I3ASSERT( pArray != NULL);

		I3_REF_CHANGE( m_pInvBoneArray, pArray);
	}

	return Result;
}

void i3Bone2MatrixListAttr::CopyFrom( i3BoneMatrixListAttr * pSrc, I3_COPY_METHOD method)
{
	INT32 i;

	I3ASSERT( pSrc->IsExactTypeOf( i3BoneMatrixListAttr::GetClassMeta()));

	((i3PersistantElement*)pSrc)->CopyTo( this, method);

	if( pSrc->getBoneCount() > 0)
	{
		SetBoneCount( pSrc->getBoneCount());
		
		for( i = 0; i < pSrc->getBoneCount(); i++)
		{
			i3Bone * pBone = pSrc->getBone( i);

			m_pBone2[ i].setDepth( pBone->getDepth());
			m_pBone2[ i].setFlag( 0);
			m_pBone2[ i].setIKBoneInfoIndex( -1);
			m_pBone2[ i].setIKContextIndex( -1);
			m_pBone2[ i].setMatrix( pBone->getMatrix());
			m_pBone2[ i].setName( pBone->getName());
			m_pBone2[ i].setParentIndex( pBone->getParentIndex());
		}
	}

	if( pSrc->getInverseBoneArray() != NULL)
	{
		setInverseBoneArray( pSrc->getInverseBoneArray());
	}
}

