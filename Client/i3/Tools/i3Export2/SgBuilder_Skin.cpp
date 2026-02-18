#include "stdafx.h"
#include "SgBuilder.h"
#include "i3Export.h"

INode * SgBuilder::_ExtractWeight_Skin( INode * pNode, Modifier * pMod)
{
	INT32 i, blendBoneCount, j;
	ISkin * pISkin = NULL;
	ISkinContextData * pSkinCtx = NULL;
	INode * pSkinBone = NULL;					// Skinning된 Bone 중에 아무거나 하나 잡아 Return하면 된다.

	pISkin = (ISkin *) pMod->GetInterface( I_SKIN);
	if( pISkin == NULL)
		return NULL;

	pSkinCtx = pISkin->GetContextInterface( pNode);
	if( pSkinCtx == NULL)
		goto ExitPart;

	// m_pWeights Buffer에 대한 준비
	{
		m_WeightCount = pSkinCtx->GetNumPoints();

		m_pWeights = new i3tool::exporter::SKIN_VERTEX[ m_WeightCount];
		I3ASSERT( m_pWeights != NULL);

		memset( m_pWeights, 0, sizeof(i3tool::exporter::SKIN_VERTEX) * m_WeightCount);
	}

	// 우선 Vertex에 의해 Reference되고 있는 모든 Bone Node들의 List를 작성한다.
	for( i = 0; i < m_WeightCount; i++)
	{
		blendBoneCount = pSkinCtx->GetNumAssignedBones( i);

		m_pWeights[i].m_WeightCount = blendBoneCount;

		for( j = 0; j < blendBoneCount; j++)
		{
			INT32 boneIdx = pSkinCtx->GetAssignedBone( i, j);
			INode * pBone = pISkin->GetBone( boneIdx);

			m_pWeights[i].m_pBone[j] = pBone;
			m_pWeights[i].m_Weight[j] = pSkinCtx->GetBoneWeight( i, j);

			if( pSkinBone == NULL)
				pSkinBone = pBone;
		}
	}

ExitPart:
	if( pISkin != NULL)
	{
		pMod->ReleaseInterface( I_SKIN, pISkin);
		pISkin = NULL;
	}

	return pSkinBone;
}