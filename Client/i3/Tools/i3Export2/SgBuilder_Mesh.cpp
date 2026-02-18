#include "stdafx.h"
#include "SgBuilder.h"
#include "i3Export.h"
#include "Util.h"
#include "i3EXPTriObject.h"


void SgBuilder::_RemoveAllMesh(void)
{

	CMeshInfo * pMesh;

	for(size_t i = 0; i < m_MeshList.size(); i++)
	{
		pMesh = m_MeshList[i];

		delete pMesh;
	}

	m_MeshList.clear();
}

INT32 SgBuilder::_FindMesh( INode * pNode, Mesh * pMesh)
{
	CMeshInfo * pTemp;
	Object * pObj = pNode->EvalWorldState( g_pExport->getStartTime()).obj;

	for( size_t i = 0; i < m_MeshList.size(); i++)
	{
		pTemp = m_MeshList[i];

		//if( (pTemp->getNode() == pNode) && (pTemp->getMesh() == pMesh))
		//if( pTemp->getMesh() == pMesh)
		if( pTemp->getSrcObject() == pObj)
		{
			return i;
		}
	}

	return -1;
}

CMeshInfo * SgBuilder::_RegisterMesh( INode * pNode, Mesh * pMesh, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount, bool bTangent)
{
	TimeValue time = m_pExport->getStartTime();
	INT32 vx[3], k, i;
	bool bNegScale;
	INT32 vidx[3];
	Face * pFace;
	Matrix3 PivotTrans;
	CVertInfo v[3];
	CMeshInfo * pMeshInfo;

	// Pivot Transform
	{
		GetPivotMatrix( pNode, &PivotTrans, time);

		if( pPosTX != NULL)
			PivotTrans *= *pPosTX;
	}

	{
		// we check for the winding orders of the triangle
		Matrix3 tm = pNode->GetObjTMAfterWSM( time);
		bNegScale = TMNegParity( tm);
	}

	if( bNegScale)
	{
		// Minus Scale에 의해 뒤집혀 있는 경우...
		vx[0] = 2;	vx[1] = 1;	vx[2] = 0;
	}
	else
	{
		vx[0] = 0;	vx[1] = 1;	vx[2] = 2;
	}

	if( TexCoordCount > 0)
	{
		// 관련 UV 좌표가 Material의 Texture 설정 개수에 맞게
		// 존재하는지 검사해서, 실제로 존재하지 않는다면
		// UV 좌표가 존재하는 개수만큼만 Material Texture도 Export하도록
		// 강제한다.
		for( i = 1; i <= TexCoordCount; i++)
		{
			if( (pMesh->mapFaces(i) == NULL) || (pMesh->mapVerts(i) == NULL))
			{
				char conv[256];

				i3::snprintf( conv, sizeof(conv), "Material에는 %d개의 Texture를 사용하도록 지정되어 있지만, Vertex에는 동일 개수의 UV 좌표가 설정되어 있지 않습니다.", TexCoordCount);
				g_pExport->Log( pNode, conv);

				TexCoordCount = i - 1;
				break;
			}
		}
	}

	{
		INT32 idx = _FindMesh( pNode, pMesh);
		if( idx != -1)
		{
			return m_MeshList[ idx];
		}

		pMeshInfo = new CMeshInfo;

		pMeshInfo->Create( pNode, pMesh, pPosTX, bVertexColor, TexCoordCount, blendCount, bTangent);

		m_MeshList.push_back( pMeshInfo);
	}

	for( i = 0; i < pMesh->getNumFaces(); i++)
	{
		TVFace * pTFace;

		pFace = &(pMesh->faces[ i]);

		vidx[0] = pFace->v[ vx[0]];
		vidx[1] = pFace->v[ vx[1]];
		vidx[2] = pFace->v[ vx[2]];

		v[0].reset();
		v[1].reset();
		v[2].reset();
		
		// Position & Normal
		{
			v[0].m_idxPos = vidx[0];
			v[1].m_idxPos = vidx[1];
			v[2].m_idxPos = vidx[2];

			VEC3D vec;

			for( k = 0; k < 3; k++)
			{
				Point3 pos;
				Point3 norm;

				// Position
				pos = pMesh->getVert( vidx[k]);
				pos = PivotTrans.PointTransform( pos);
				Convert( &vec, pos);

				i3Vector::Copy( & v[k].m_Pos, &vec);

				// Normal
				norm = GetVertexNormal( pMesh, i, pMesh->getRVertPtr( vidx[k]));
				norm = PivotTrans.VectorTransform( norm);
				norm = norm.Normalize();
				Convert( &vec, norm);

				i3Vector::Copy( & v[k].m_Normal, &vec);
			}
		}

		// Vertex Color
		if( bVertexColor)
		{
			VertColor col;

			pTFace = pMesh->vcFace + i;

			for( k = 0; k < 3; k++)
			{
				col = pMesh->vertCol[ pTFace->t[ vx[k]]];
				
				i3Color::Set( & v[k].m_Color, col.x, col.y, col.z, 1.0f);
			}
		}

		// Texture Coord.
		if( TexCoordCount > 0)
		{
			INT32 set;
			VEC2D uv;
			UVVert pt, * pUVVerts;
			Matrix3 * pUVTX;

			// 3DSMAX에서는 Vertex Color가 하나의 Map Channel(0)을 사용하기
			// 때문에 제외시켜야 한다.
			for( set = 1; set < TexCoordCount + 1; set++)
			{	
				pTFace = pMesh->mapFaces( set) + i;
				pUVVerts = pMesh->mapVerts( set);
				pUVTX = & m_pExport->m_TexSetInfo[set - 1].m_Matrix;

				for( k = 0; k < 3; k++)
				{
					pt = pUVVerts[ pTFace->t[vx[k]]];

					pt = pUVTX->PointTransform( pt);
					Convert( &uv, pt);

					i3Vector::Copy( &v[k].m_TexCoord[ set - 1], &uv);
				}
			}
		}
		
		if( blendCount > 0)
		{
			INT32 bi, boneIdx;
			i3tool::exporter::SKIN_VERTEX * pWVtx;

			for( k = 0; k < 3; k++)
			{
				I3ASSERT( vidx[k] < m_WeightCount);
				pWVtx = & m_pWeights[ vidx[k]];

				for( bi = 0; bi < blendCount; bi++)
				{
					if( pWVtx->m_WeightCount <= bi)
					{
						boneIdx = 0;
						v[k].m_Weight[ bi] = 0.0f;
					}
					else
					{
						v[k].m_Weight[ bi] = pWVtx->m_Weight[bi];

						{
							// Weighting된 Bone을 검색한다.
							INT32 idxTemp = findBoneByINode( pWVtx->m_pBone[bi]);
							I3ASSERT( idxTemp != -1);
							BONE_INFO * pBone = getBone( idxTemp);

							I3ASSERT( pBone->m_bSkinned);
							I3ASSERT( i3::same_of<i3Body*>(pBone->m_pNode)); //->IsExactTypeOf( i3Body::static_meta()));

							i3Body * pBody = (i3Body *) pBone->m_pNode;
							i3LOD * pLOD = pBody->getLOD( 0);
							i3Skeleton * pSkel = pLOD->getSkeleton();

							boneIdx = pSkel->FindBoneByName( pWVtx->m_pBone[bi]->GetName());
						}

						if( boneIdx == -1)
						{
							char conv[256];

							i3::snprintf( conv, sizeof(conv), "잘못된 Bone에 대한 Weight가 설정되어 있습니다. [%s]", pWVtx->m_pBone[bi]->GetName());
							g_pExport->Log( pNode, conv);
							boneIdx = 0;
						}
					}

					v[k].m_Index[ bi] = boneIdx;
				}
			}
		}

		// Tangent & Binormal
		if( TexCoordCount > 0)
		{
			VEC3D vT, vB;

			i3Math::CalcTangentBasis(	& v[0].m_Pos,			& v[1].m_Pos,			& v[2].m_Pos, 
										& v[0].m_TexCoord[0],	& v[1].m_TexCoord[0],	& v[2].m_TexCoord[0],
										&vT, &vB);

			i3Vector::Add( & v[0].m_Tangent, & v[0].m_Tangent, & vT);
			i3Vector::Add( & v[0].m_Binormal, & v[0].m_Binormal, & vB);

			i3Vector::Add( & v[1].m_Tangent, & v[1].m_Tangent, & vT);
			i3Vector::Add( & v[1].m_Binormal, & v[1].m_Binormal, & vB);

			i3Vector::Add( & v[2].m_Tangent, & v[2].m_Tangent, & vT);
			i3Vector::Add( & v[2].m_Binormal, & v[2].m_Binormal, & vB);
		}

		INT32 idxFace0 = pMeshInfo->AddVert( &v[0]);
		INT32 idxFace1 = pMeshInfo->AddVert( &v[1]);
		INT32 idxFace2 = pMeshInfo->AddVert( &v[2]);

		pMeshInfo->AddFace( idxFace0, idxFace1, idxFace2);
	}

	if( pMeshInfo->getVertCount() > 0xFFFF)
	{
		//g_pExport->Log( pNode, "Vertex의 개수가 65536개 제한을 초과합니다. Export를 진행하지만, 올바르게 출력되지 않을 수 있습니다. 적절히 나눠 Export해 주시기 바랍니다.");
	}

	pMeshInfo->BuildVertexArray( );

	return pMeshInfo;
}

i3GeometryAttr * SgBuilder::_ExtractGeometry( INode * pNode, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount, bool bTangent)
{
	
	i3VertexFormat fmt;
	TimeValue time = m_pExport->getStartTime();
	i3GeometryAttr * pGeoAttr = NULL;

	CMeshInfo * pMeshInfo = _RegisterMesh( pNode, pMesh, pPosTX, bVertexColor, TexCoordCount, blendCount, bTangent);

	if( pGeoAttr == NULL)
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();
	}

	pGeoAttr->SetVertexArray( pMeshInfo->getVertexArray(), I3G_PRIM_TRILIST, pGrpInfo->m_FaceList.size());
	pGeoAttr->SetIndexArray( pMeshInfo->getIndexArray());

	pMeshInfo->BeginSetIndexArray();

	pGeoAttr->SetStartIndex( pMeshInfo->getDrawStartIndex());

	for(size_t i = 0; i < pGrpInfo->m_FaceList.size(); i++)
	{
		INT32 idxFace = pGrpInfo->m_FaceList[i];

		pMeshInfo->SetIndex( idxFace);				
	}

	pMeshInfo->EndSetIndexArray();

	return pGeoAttr;
}

void SgBuilder::_OnJobMesh( JOB_INFO * pJob)
{
	INode * pNode = pJob->m_pINode;
	Mesh * pMesh;
	Face * pFace;
	INT32 j;
	bool bVertexColor = false;
	INT32	TexCoordCount = 0;
	Mtl * pNodeMtl, * pFaceMtl;
	TimeValue time = m_pExport->getStartTime();
	char szName[MAX_PATH];
	Matrix3 uvtx;
	BONETYPE boneType;
	INT32 maxBlendMatrix;
	INode *			pSkinBone = NULL;
	i3AttrSet *		pHighAttrSet = NULL;
	i3Node *		pAttachNode = NULL;

	i3EXPTriObject triobj( pNode, m_pExport->getStartTime());

	pJob->m_bProcessed = true;			// 해당 Job을 처리했음을 표시.

	if( triobj.GetTriObject() == NULL)
		return;

	pMesh = triobj.GetMesh();

	// Vertex Color
	if( pMesh->mapSupport( 0) && (pMesh->getNumVertCol() > 0))
		bVertexColor = true;
	
	// Texture Mapping
	TexCoordCount = pMesh->getNumMaps() - 1;

	pNodeMtl = pNode->GetMtl();

	// 우선 Single Texture 또는 Multi Texture의 상황을 파악하고,
	// 사용하는 각 Material들에 대한 Mesh Group을 생성한다.
	if( pNodeMtl != NULL)
	{
		Mtl * pNewMtl;

		pHighAttrSet = i3AttrSet::new_object();

		// _ResolveMeshGroup의 처리 중에 상위 Material로 추가적인 Attr이
		// 검색될 수 있다.
		pNewMtl = m_pExport->_ResolveMeshGroup( pNode, pHighAttrSet, pNodeMtl);

		if( pNewMtl != NULL)
			pNodeMtl = pNewMtl;

		if( pHighAttrSet->GetAttrCount() == 0)
		{
			// 만약 _ResolveMeshGroup에서 검색된 상위 Material이 딱히 없다면
			// AttrSet을 제거한다.
			// 불필요하게 i3AttrSet에 마구 추가되는 것을 방지하기 위함.
			I3_SAFE_RELEASE( pHighAttrSet);
		}
	}

	// 각 Face를 참조하는 Material의 종류에 따라 분류한다.
	// 각 Material마다 별개의 i3AttrSet Node로 구분되어 Scene Graph로 
	// 만들어진다.
	for(INT32 i = 0; i < pMesh->getNumFaces(); i++)
	{
		pFace = & pMesh->faces[i];

		if((pNodeMtl != NULL) && (pNodeMtl->NumSubMtls() > 0))
		{
			// Multi Material
			// 각 Face가 다른 Material을 가지고 있는 경우.
			// getMatID() 함수가 반환하는 값은 모호한 부분이 있다.
			// It is definitely not 1 to 1 with sub materials
			// According to the ascii exporter included with max,
			// use modulus with the number of sub materials at the _node
			// to get a valid index of a submaterial 
			INT32 idxMtl = pFace->getMatID();
			
			if( idxMtl >= pNodeMtl->NumSubMtls())
				idxMtl %= pNodeMtl->NumSubMtls();

			pFaceMtl = pNodeMtl->GetSubMtl( idxMtl);
		}
		else
		{
			pFaceMtl = pNodeMtl;
		}

		m_pExport->_AddTriToMeshGroup( pFaceMtl, i);
	}

	pMesh->buildNormals();

	m_pWeights = NULL;

	// Physique 및 Skin을 검색한다.
	{
		::Modifier * pModifier = FindBonesModifier( pNode, &boneType);

		switch( boneType)
		{
			case BONES_3DS :
				pSkinBone = _ExtractWeight_Skin( pJob->m_pINode, pModifier);
				break;

			case PHYSIQUE :	
				pSkinBone = _ExtractWeight_Physique( pJob->m_pINode, pModifier);	
				break;

			default :
				m_pWeights = NULL;
				m_WeightCount = 0;
				maxBlendMatrix = 0;
				pModifier = NULL;
				break;
		}
	}

	// Attach Node를 검색한다.
	// i3Body에 붙는 경우와 그렇지 않은 경우에 따라 다르다.
	{
		pAttachNode = GetAttachPoint( pJob->m_pINode, pHighAttrSet, pSkinBone);
	}

	// 분류되어진 Material별로 Scene Graph를 생선한다.
	for(size_t i = 0; i < m_pExport->m_MeshGroupList.size(); i++)
	{
		i3tool::exporter::MESH_GROUP_INFO * pGrpInfo = m_pExport->m_MeshGroupList[i];
		i3AttrSet * pAttrSet;
		bool bTangent;

		pFaceMtl = (Mtl *) pGrpInfo->m_pMtl;

		if( pGrpInfo->m_FaceList.size() <= 0)
			continue;

		// Material 정보를 Scene Graph에 Export한다.
		pAttrSet = i3AttrSet::new_object_ref();

		if( pFaceMtl != NULL)
		{
			if( g_pExport->getOption()->m_bUseMaterialName)
				sprintf( szName, "%s_%s", pNode->GetName(), (const char*)pFaceMtl->GetName());
			else
				sprintf( szName, "%s_Material", pNode->GetName());
		}
		else
			sprintf( szName, "%s", pNode->GetName());

		pAttrSet->SetName( szName);

		if( m_pExport->_OnExportMaterial( pNode, pGrpInfo->m_pMtl, pAttrSet) == false)
			TexCoordCount = 0;

		// Texture Mapping을 위한 필요 정보
		if( pFaceMtl != NULL)
		{
			m_pExport->_SetTextureSet( 0, pFaceMtl, ID_DI);
		}

		i3GeometryAttr * pGeoAttr;
		Matrix3			posTx;

		// 사용된 Blending Matrix의 최대 개수를 구한다.
		for( j = 0, maxBlendMatrix = 0; j < m_WeightCount; j++)
		{
			if( m_pWeights[j].m_WeightCount > maxBlendMatrix)
			{
				maxBlendMatrix = m_pWeights[j].m_WeightCount;
			}
		}

		// Tangent와 Binormal이 필요한지 검사.
		{
			i3NormalMapEnableAttr * pAttr = (i3NormalMapEnableAttr *) pAttrSet->FindAttr( i3NormalMapEnableAttr::static_meta());

			bTangent = ( (pAttr != NULL) && pAttr->Get());
		}

		// 현재 Node에 대한 Bone이 존재하지 않는다면 Transform을 구해 직접 Vertex를 Transform 시킨다.
		{
			INT32 idx = findBoneByINode( pNode);

			if( idx == -1)
				ExtractTransform( pNode, &posTx, m_pExport->getStartTime());
			else
				posTx.IdentityMatrix();
		}

		pGeoAttr = _ExtractGeometry( pNode, pMesh, pGrpInfo, &posTx, bVertexColor, TexCoordCount, maxBlendMatrix, bTangent);

		{
			i3Geometry * pGeo;

			pGeo = i3Geometry::new_object_ref();

			pGeo->SetName( pNode->GetName());		
			pGeo->AppendGeometryAttr( pGeoAttr);

			pAttrSet->AddChild( pGeo);
		}
		
		pAttachNode->AddChild( pAttrSet);

		// Transparent Flag이 있다면 상위에까지 알려야 한다.
		if( pAttrSet->IsFlag( I3_NODEFLAG_TRANSPARENT))
		{
			i3Node * pTemp = pAttachNode;

			while( pTemp != NULL)
			{
				pTemp->AddFlag( I3_NODEFLAG_TRANSPARENT);

				pTemp = pTemp->GetParent();
			}
		}
	}

	if( m_pWeights != NULL)
	{
		delete m_pWeights;
		m_pWeights = NULL;
	}

	I3_SAFE_RELEASE( pHighAttrSet);

	m_pExport->_RemoveAllMeshGroup();
}

