#include "i3SceneDef.h"
#include "i3BoneMatrixListAttr.h"
#include "i3AnimationContext.h"
#include "i3SceneTracer.h"

I3_CLASS_INSTANCE( i3BoneMatrixListAttr);

i3BoneMatrixListAttr::i3BoneMatrixListAttr(void)
{
	SetID( I3_ATTRID_BONEMATRIXLIST);
}

i3BoneMatrixListAttr::~i3BoneMatrixListAttr(void)
{
	I3_SAFE_DELETE_ARRAY( m_pBone);
	I3_SAFE_RELEASE( m_pTxBoneArray);
	I3_SAFE_RELEASE( m_pInvBoneArray);
}

void i3BoneMatrixListAttr::SetBoneCount( INT32 count, INT32 ver)
{
	I3_SAFE_DELETE_ARRAY( m_pBone);
	I3_SAFE_RELEASE( m_pTxBoneArray);

	m_Version = ver;
	
	if( m_Count != count)
	{
		m_Count = count;

		m_pBone = new i3Bone[count];		// 배열이다..//(i3Bone *) i3MemAlloc( sizeof(i3Bone) * count);
		I3ASSERT( m_pBone != nullptr);
	
		NetworkDump_Malloc( m_pBone, sizeof( i3Bone) * count, __FILE__, __LINE__);

		m_pTxBoneArray = i3MatrixArray::new_object();
		m_pTxBoneArray->Create( count);

	}
}

void i3BoneMatrixListAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetVertexBlendMatrices( m_pTxBoneArray->GetMatrix(0), m_Count);
}

void i3BoneMatrixListAttr::BuildTransformedMatrix( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
	INT32 i, idx;

	I3ASSERT( pCtx != nullptr);

	if( m_pInvBoneArray != nullptr)
	{
		for( i = 0; i < m_Count; i++)
		{
			if( pBoneMap != nullptr)
				idx = pBoneMap[ i];
			else
				idx = i;

			i3Matrix::Mul( m_pTxBoneArray->GetMatrix(i), getInverseMatrix(i), pCtx->getCacheMatrix(idx));
		}
	}
	else
	{
		for( i = 0; i < m_Count; i++)
		{
			if( pBoneMap != nullptr)
				idx = pBoneMap[ i];
			else
				idx = i;

			i3Matrix::Copy( m_pTxBoneArray->GetMatrix(i), pCtx->getCacheMatrix(idx));
		}
	}
}

void i3BoneMatrixListAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	if( !i3::same_of<i3BoneMatrixListAttr*>(pDest))
		return;

	i3BoneMatrixListAttr * pAttr = (i3BoneMatrixListAttr *) pDest;

	if( getBoneCount() > 0)
	{
		pAttr->SetBoneCount( getBoneCount());

	//	memcpy( pAttr->m_pBone, m_pBone, sizeof(i3Bone) * getBoneCount());
		i3::copy(m_pBone, m_pBone + getBoneCount(), pAttr->m_pBone );	// STL copy알고리즘이라면 무난하게 루프대입됨..
	}

	if( getInverseBoneArray() != nullptr)
	{
		pAttr->setInverseBoneArray( getInverseBoneArray());
	}
}

void i3BoneMatrixListAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( getInverseBoneArray() != nullptr)
	{
		getInverseBoneArray()->OnBuildObjectList( List);
	}

	i3RenderAttr::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 BONE_MATRIX_ATTR
	{
		UINT8		m_ID[4] = { 'B', 'M', 'A', '2' };
		INT32		m_Count = 0;
		OBJREF64	m_pInvBoneArray = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3BoneMatrixListAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::BONE_MATRIX_ATTR data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Count = getBoneCount();
	data.m_pInvBoneArray = (OBJREF64) pResFile->GetObjectPersistID( getInverseBoneArray());

	// Count
	{
		Rc = pStream->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	// Bone References
	i3::pack::BONE2 bone;
	for( INT32 i = 0; i < getBoneCount(); i++)
	{
		i3::safe_string_copy( bone.m_szName, m_pBone[i].getNameString(), sizeof( bone.m_szName) );
		bone.m_ParentIndex = m_pBone[i].getParentIndex();
		i3Matrix::Copy( &bone.m_Matrix, m_pBone[i].getMatrix());
		bone.m_Depth = m_pBone[i].getDepth();

		Rc = pStream->Write( &bone, sizeof(bone) );
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3BoneMatrixListAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::BONE_MATRIX_ATTR data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RenderAttr::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	bool bNewVersion = false;

	if( ::memcmp( data.m_ID, "BMA2", 4) == 0)
	{	// new version
		bNewVersion = true;
	}

	if( data.m_Count > 0)
	{
		SetBoneCount( data.m_Count);

		if( bNewVersion)
		{
			i3::pack::BONE2 bone;
			for( INT32 i = 0; i < getBoneCount(); i++)
			{
				Rc = pStream->Read( &bone, sizeof(bone));
				I3_CHKIO( Rc);
				Result += Rc;

				m_pBone[i].setName( bone.m_szName);
				m_pBone[i].setParentIndex( bone.m_ParentIndex);
				m_pBone[i].setMatrix( &bone.m_Matrix);
				m_pBone[i].setDepth( bone.m_Depth);
			}
		}
		else
		{
			i3::pack::BONE bone;
			for( INT32 i = 0; i < getBoneCount(); i++)
			{
				Rc = pStream->Read( &bone, sizeof(bone));
				I3_CHKIO( Rc);
				Result += Rc;

				m_pBone[i].setName( bone.m_szName);
				m_pBone[i].setParentIndex( bone.m_ParentIndex);
				m_pBone[i].setMatrix( &bone.m_Matrix);
				m_pBone[i].setDepth( bone.m_Depth);
			}
		}
	}

	if( data.m_pInvBoneArray != 0)
	{
		i3MatrixArray * pArray = (i3MatrixArray *) pResFile->FindObjectByID( (OBJREF) data.m_pInvBoneArray);
		I3ASSERT( pArray != nullptr);

		setInverseBoneArray( pArray);
	}

	return Result;
}
