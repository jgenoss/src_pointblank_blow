#include "stdafx.h"
#include "SgBuilder.h"
#include "i3Export.h"

INode *  SgBuilder::_ExtractWeight_Physique( INode * pNode, Modifier * pMod)
{
	INT32 i, blendBoneCount;
	IPhyVertexExport * pVertexExport = NULL;
	IPhysiqueExport * phyExport = NULL;
	IPhyContextExport * mcExport = NULL;
	INode * pSkinBone = NULL;
		
	phyExport = (IPhysiqueExport *)pMod->GetInterface( I_PHYINTERFACE);
	if( phyExport == NULL)
		return NULL;

	mcExport = (IPhyContextExport *)phyExport->GetContextInterface( pNode);
	if( mcExport == NULL)
		goto ExitPart;

	mcExport->ConvertToRigid( TRUE);
	mcExport->AllowBlending( TRUE);

	if( mcExport->GetNumberVertices() <= 0)
		goto ExitPart;

	{
		m_WeightCount = mcExport->GetNumberVertices();

		m_pWeights = new i3tool::exporter::SKIN_VERTEX[ m_WeightCount];
		I3ASSERT( m_pWeights != NULL);

		memset( m_pWeights, 0, sizeof(i3tool::exporter::SKIN_VERTEX) * mcExport->GetNumberVertices());
	}

	// 우선 Vertex에 의해 Reference되고 있는 모든 Bone Node들의 List를 작성한다.
	for( i = 0; i < mcExport->GetNumberVertices(); i++)
	{
		pVertexExport = (IPhyVertexExport *) mcExport->GetVertexInterface( i);

		switch( pVertexExport->GetVertexType())
		{
			case RIGID_BLENDED_TYPE :
				//type-cast the node to the proper class		
				{
					IPhyBlendedRigidVertex *vtx = (IPhyBlendedRigidVertex *) pVertexExport;

					blendBoneCount = MIN( vtx->GetNumberNodes(), MAX_BLEND_MATRIX);

					m_pWeights[i].m_WeightCount = blendBoneCount;
					
					for( int x = 0; x < blendBoneCount; x++)
					{
						m_pWeights[i].m_pBone[x] = vtx->GetNode( x);
						m_pWeights[i].m_Weight[x] = vtx->GetWeight( x);
						m_pWeights[i].m_Offset[x] = vtx->GetOffsetVector( x);

						if( pSkinBone == NULL)
							pSkinBone = vtx->GetNode( x);
					}
				}
				break;

			case RIGID_NON_BLENDED_TYPE :
				{
					IPhyRigidVertex * vtx = (IPhyRigidVertex *) pVertexExport;

					m_pWeights[i].m_WeightCount = 1;
					m_pWeights[i].m_pBone[0] = vtx->GetNode();
					m_pWeights[i].m_Weight[0] = 1.0f;
					m_pWeights[i].m_Offset[0] = vtx->GetOffsetVector();

					if( pSkinBone == NULL)
						pSkinBone = vtx->GetNode();
				}
				break;

			default :
				break;
		}

		mcExport->ReleaseVertexInterface( pVertexExport);
		pVertexExport = NULL;
	}

ExitPart:
	if( phyExport != NULL)
	{
		pMod->ReleaseInterface( I_PHYINTERFACE, phyExport);
		phyExport = NULL;
	}

	return pSkinBone;
}