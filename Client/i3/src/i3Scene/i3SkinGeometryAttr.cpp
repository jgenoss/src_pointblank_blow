#include "i3SceneDef.h"
#include "i3SkinGeometryAttr.h"

#include "i3SkinMeshAABBProcessor.h"

I3_CLASS_INSTANCE( i3SkinGeometryAttr);

i3SkinGeometryAttr::~i3SkinGeometryAttr(void)
{
	I3_SAFE_RELEASE( m_pSkinGroup);
}

I3_DRAW_GROUP *	i3SkinGeometryAttr::getSkinGroup( INT32 idx)
{
	I3_DRAW_GROUP * pGroup = (I3_DRAW_GROUP *)m_pSkinGroup->getBuffer();

	I3_BOUNDCHK( idx, m_SkinGroupCount);

	return &pGroup[ idx];
}

void i3SkinGeometryAttr::SetSkinGroupCount( INT32 cnt)
{
	I3_SAFE_RELEASE( m_pSkinGroup);

	m_SkinGroupCount = cnt;

	m_pSkinGroup = i3MemoryBuffer::new_object();
	m_pSkinGroup->Create( sizeof(I3_DRAW_GROUP) * cnt);
}


void i3SkinGeometryAttr::OptimizeForSkin(void)
{
	// 직접 Skinning Mesh를 Build하기 때문에 처리할 필요없다.
}

void i3SkinGeometryAttr::Setup( i3GeometryAttr * pSrcAttr)
{
	I3_DRAW_GROUP * pSrcGroup;
	I3_DRAW_GROUP * pDestGroup;

	if( pSrcAttr->getPrimitiveGroupList() == nullptr)
	{
		pSrcAttr->BuildSkin( 4, 20);
	}

	{
		SetVertexArray( pSrcAttr->getPrimitiveVertexArray(), pSrcAttr->GetPrimitiveType(), pSrcAttr->GetPrimitiveCount(), 0, false); //pSrcAttr->GetStartIndex(), false);
		SetIndexArray( pSrcAttr->getPrimitiveIndexArray());
	}

	I3_DRAW_GROUP_LIST_PTR& pGroupList = pSrcAttr->getPrimitiveGroupList();

	SetSkinGroupCount( (INT32)pGroupList->size());

	for(size_t i = 0; i < pGroupList->size(); i++ )
	{
		pSrcGroup = (*pGroupList)[i];
		pDestGroup = getSkinGroup( i);

		i3mem::Copy( pDestGroup, pSrcGroup, sizeof( I3_DRAW_GROUP));
	}	

	m_AABBProcessorPtr = i3::make_shared<i3SkinMeshAABBProcessor>(this);

}

void i3SkinGeometryAttr::Apply( i3RenderContext * pCtx)
{
	INT32 i, j, idx;
	i3IndexArray * pIA = GetIndexArray();
	MATRIX *	pBoneTable[256];
	I3_DRAW_GROUP * pGroup;

	if( GetVertexArray() == nullptr)
		return;

	if( pCtx->GetLightingEnable())
	{
		_UpdateLights( pCtx);
	}

	pCtx->SetVertexArray( GetVertexArray());

	if( pIA != nullptr)
		pCtx->SetIndexArray( pIA);

	I3ASSERT( m_pBlendMatrixList != nullptr);

	for( i = 0; i < getSkinGroupCount(); i++)
	{
		pGroup = getSkinGroup( i);

		{
			for( j = 0; j < pGroup->m_BoneCount; j++ )
			{
				idx = pGroup->m_Bone[j];
				I3ASSERT( idx != 0xFF);
				I3ASSERT( idx < m_pBlendMatrixList->getBoneCount() );

				pBoneTable[j] = m_pBlendMatrixList->getTransformedMatrix( idx);
			}

			pCtx->SetVertexBlendMatricesByRef( pBoneTable, pGroup->m_BoneCount);
		}

		if( pIA != nullptr)
			pCtx->DrawIndexedPrim( m_pDrawShader, m_PrimType, pGroup->m_idxStart, pGroup->m_PrimCount);
		else
			pCtx->DrawPrim( m_pDrawShader, m_PrimType, pGroup->m_idxStart, pGroup->m_PrimCount);
	}
}

void i3SkinGeometryAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	// 항상 Reference 형식으로 복사한다.
	i3GeometryAttr::CopyTo( pObj, method);

	i3SkinGeometryAttr * pDest = (i3SkinGeometryAttr *) pObj;

	pDest->SetVertexArray( GetVertexArray(), GetPrimitiveType(), GetPrimitiveCount(), GetStartIndex(), false);
	pDest->SetIndexArray( GetIndexArray());

	pDest->m_SkinGroupCount = m_SkinGroupCount;
	I3_REF_CHANGE( pDest->m_pSkinGroup, m_pSkinGroup);

}

void	i3SkinGeometryAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pSkinGroup != nullptr)
	{
		m_pSkinGroup->OnBuildObjectList( List);
	}

	i3GeometryAttr::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SKIN_GEOMETRY
	{
		INT8	m_ID[4] = { 'S', 'G', 'A', '1' };
		UINT32	m_SkinGroupCount = 0;
		OBJREF	m_pSkinGroup = 0;
		UINT32	pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SkinGeometryAttr::OnSave( i3ResourceFile * pFile)
{
	pack::SKIN_GEOMETRY data;
	UINT32 rc, result;

	result = i3GeometryAttr::OnSave( pFile);
	I3_CHKIO( result);

	data.m_SkinGroupCount = getSkinGroupCount();
	data.m_pSkinGroup = pFile->GetObjectPersistID( m_pSkinGroup);

	rc = pFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3SkinGeometryAttr::OnLoad( i3ResourceFile * pFile)
{
	pack::SKIN_GEOMETRY data;
	UINT32 rc, result;

	result = i3GeometryAttr::OnLoad( pFile);
	I3_CHKIO( result);

	rc = pFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pSkinGroup != 0)
	{
		i3MemoryBuffer * pBuffer = (i3MemoryBuffer *) pFile->FindObjectByID( data.m_pSkinGroup);

		I3_REF_CHANGE( m_pSkinGroup, pBuffer);
	}

	m_SkinGroupCount = data.m_SkinGroupCount;

	m_AABBProcessorPtr = i3::make_shared<i3SkinMeshAABBProcessor>(this);

	return result;
}

#if defined( I3_DEBUG)
void i3SkinGeometryAttr::Dump(void)
{
	i3GeometryAttr::Dump();
}

#endif