#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "i3EXPTriObject.h"

i3GeometryAttr * i3Export::_BuildGeometry( INode * pNode, Mesh * pMesh, i3tool::exporter::MESH_GROUP_INFO * pGrpInfo, Matrix3 * pPosTX, bool bVertexColor, INT32 TexCoordCount, INT32 blendCount)
{
	INT32 vx[3], k, i;
	i3VertexFormat fmt;
	TimeValue time = m_timeStart;
	bool bNegScale;
	i3VertexArray * pVA;
	i3GeometryAttr * pGeoAttr;
	INT32 vidx[3];
	Mtl * pFaceMtl;
	Face * pFace;
	REAL32 fTransparency;
	Matrix3 PivotTrans;

	// Pivot Transform
	{
		#if 1
			GetPivotMatrix( pNode, &PivotTrans, time);

			//_dumpMatrix2( "Pivot TM", &PivotTrans);

			if( pPosTX != NULL)
				PivotTrans *= *pPosTX;
		#else
			if( pPosTX != NULL)
				PivotTrans = *pPosTX;
			else
				PivotTrans.IdentityMatrix();
		#endif
	}

	{
		// we take the lowest level object of the 3ds max pipeline
		Object* object = pNode->GetObjOrWSMRef();

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

	fmt.SetHasPosition( true);
	fmt.SetHasNormal( true);
	fmt.SetHasColor( bVertexColor);
	
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

				sprintf( conv, "Material에는 %d개의 Texture를 사용하도록 지정되어 있지만, Vertex에는 동일 개수의 UV 좌표가 설정되어 있지 않습니다.", TexCoordCount);
				Log( pNode, conv);

				TexCoordCount = i - 1;
				break;
			}
		}

		fmt.SetTextureCoordSetCount( TexCoordCount);
		fmt.SetTextureCoordCount( 2);
	}

	fmt.SetBlendIndexCount( blendCount);

	if( blendCount > 0)
	{
		fmt.SetBlendWeightCount( blendCount - 1);
	}


	pVA = i3VertexArray::new_object_ref();
	pVA->Create( &fmt, pGrpInfo->m_FaceList.size() * 3, 0);

	pGeoAttr = i3GeometryAttr::new_object_ref();
	pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pGrpInfo->m_FaceList.size());

	pVA->Lock();

	for(size_t j = 0; j < pGrpInfo->m_FaceList.size(); j++)
	{
		TVFace * pTFace;
		INT32 idxFace = pGrpInfo->m_FaceList[j];

		pFaceMtl = (Mtl *) pGrpInfo->m_pMtl;
		pFace = &(pMesh->faces[ idxFace]);

		if( pFaceMtl != NULL)
			fTransparency = 1.0f - pFaceMtl->GetXParency();
		else
			fTransparency = 1.0f;

		vidx[0] = pFace->v[ vx[0]];
		vidx[1] = pFace->v[ vx[1]];
		vidx[2] = pFace->v[ vx[2]];

		// Position
		{
			VEC3D vec;

			for( k = 0; k < 3; k++)
			{
				Point3 pos;

				pos = pMesh->getVert( vidx[k]);
				pos = PivotTrans.PointTransform( pos);
				Convert( &vec, pos);

				pVA->SetPosition( (j * 3) + k, &vec);
			}
		}

		// Normal
		{
			VEC3D vec;

			for( k = 0; k < 3; k++)
			{
				Point3 norm;

				norm = GetVertexNormal( pMesh, idxFace, pMesh->getRVertPtr( vidx[k]));
				norm = PivotTrans.VectorTransform( norm);
				norm.Normalize();

				Convert( &vec, norm);
				pVA->SetNormal( (j * 3) + k, &vec);
			}
		}

		// Vertex Color
		if( bVertexColor)
		{
			VertColor col;
			COLORREAL c;

			pTFace = pMesh->vcFace + idxFace;

			for( k = 0; k < 3; k++)
			{
				col = pMesh->vertCol[ pTFace->t[ vx[k]]];
				i3Color::Set( &c, col.x, col.y, col.z, fTransparency);
				pVA->SetColor( (j * 3) + k, &c);
			}
		}

		// Texture Coord.
		if( pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			INT32 set;
			VEC2D uv;
			UVVert pt, * pUVVerts;
			Matrix3 * pUVTX;
			//REAL32 minX, minY;

			// 3DSMAX에서는 Vertex Color가 하나의 Map Channel(0)을 사용하기
			// 때문에 제외시켜야 한다.
			for( set = 1; set < pVA->GetFormat()->GetTextureCoordSetCount() + 1; set++)
			{
				pTFace = pMesh->mapFaces( set) + idxFace;
				pUVVerts = pMesh->mapVerts( set);
				pUVTX = & m_TexSetInfo[set - 1].m_Matrix;

				for( k = 0; k < 3; k++)
				{
					pt = pUVVerts[ pTFace->t[vx[k]]];

					pt = pUVTX->PointTransform( pt);
					Convert( &uv, pt);

					// Normalize
					//NormalizeTexCoord( &uv);

					pVA->SetTextureCoord( set - 1, (j * 3) + k, &uv);
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
						pVA->SetBlendIndex( bi, (j * 3) + k, 0);

						if( bi < (blendCount - 1))
							pVA->SetBlendWeight( bi, (j * 3) + k, 0.0f);
					}
					else
					{
						if( bi < (blendCount - 1))
							pVA->SetBlendWeight( bi, (j * 3) + k, pWVtx->m_Weight[bi]);

						boneIdx = _FindBoneByRef( pWVtx->m_pBone[bi]);

						if( boneIdx == -1)
						{
							char conv[256];

							sprintf( conv, "잘못된 Bone에 대한 Weight가 설정되어 있습니다. [%s]", pWVtx->m_pBone[bi]->GetName());
							Log( pNode, conv);
							boneIdx = 0;
						}

						pVA->SetBlendIndex( bi, (j * 3) + k, boneIdx);
					}
				}
			}
		}
	}

	pVA->Unlock();

	{
		i3NormalMapEnableAttr * pAttr1 = (i3NormalMapEnableAttr *) m_pCurProcessInfo->findLastAttr( i3NormalMapEnableAttr::static_meta());

		if((pAttr1 != NULL) && (pAttr1->Get() == TRUE))
		{
			i3OptBuildTangentBasis opt;

			opt.Build( pGeoAttr);
		}
	}

	return pGeoAttr;
}

Mtl * i3Export::_ResolveMeshGroup( INode * pNode, i3AttrSet * pAttrSet, Mtl * pMtl)
{
	INT32 i;
	Mtl * pNodeMtl = NULL, * pTempMtl;

	// Recursive하게 돌면서 관련된 모든 Material들을 풀어낸다.
	ulong partA = pMtl->ClassID().PartA();

	switch( partA)
	{
		case  MIXMAT_CLASS_ID :
			{
				// Blend Material이다.
				// 이 자체는 실제적인 Material 값을 가지고 있지 않기 때문에
				// Group에 추가하지 않는다.

				_OnLuxMapMaterial( pNode, pAttrSet, pMtl);
			}
			break;

		case MULTI_CLASS_ID :
			if( pMtl->NumSubMtls() > 0)
				pNodeMtl = pMtl;
			break;

		default :
			_AddMeshGroup( pMtl);
			break;
	}

	for( i = 0; i < pMtl->NumSubMtls(); i++)
	{
		if( pMtl->GetSubMtl( i) != NULL)
		{
			pTempMtl = _ResolveMeshGroup( pNode, pAttrSet, pMtl->GetSubMtl( i));

			if( pTempMtl != NULL)
				pNodeMtl = pTempMtl;
		}
	}

	return pNodeMtl;
}

void i3Export::_OnExportGeometry( INode * pNode, ObjectState * pState)
{
	Mesh * pMesh;
	Face * pFace;
	i3EXPTriObject triobj( pNode, m_timeStart);

	bool bVertexColor = false;
	INT32	TexCoordCount = 0;
	Mtl * pNodeMtl, * pFaceMtl;
	TimeValue time = getStartTime();
	char szName[MAX_PATH];
	Matrix3 uvtx;
	BONETYPE boneType;
	::Modifier * pModifier = NULL;

	if( triobj.GetTriObject() == NULL)
	{
		return;
	}

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

		pNewMtl = _ResolveMeshGroup( pNode, NULL, pNodeMtl);

		if( pNewMtl != NULL)
			pNodeMtl = pNewMtl;
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

		_AddTriToMeshGroup( pFaceMtl, i);
	}

	pMesh->buildNormals();

	m_pWeights = NULL;

	// Physique 및 Skin을 검색한다.
	{
		pModifier = FindBonesModifier( pNode, &boneType);
		switch( boneType)
		{
			case BONES_3DS :
				break;

			case PHYSIQUE :
				break;

			default :
				pModifier = NULL;
				break;
		}
	}

	// 분류되어진 Material별로 Scene Graph를 생선한다.
	for(size_t i = 0; i < m_MeshGroupList.size(); i++)
	{
		i3tool::exporter::MESH_GROUP_INFO * pGrpInfo = m_MeshGroupList[i];
		i3AttrSet * pAttrSet;
		i3Geometry * pGeo;

		pFaceMtl = (Mtl *) pGrpInfo->m_pMtl;

		if( pGrpInfo->m_FaceList.size() <= 0)
			continue;

		// Material 정보를 Scene Graph에 Export한다.
		pAttrSet = i3AttrSet::new_object_ref();

		sprintf( szName, "%s_Material", pNode->GetName());
		pAttrSet->SetName( szName);
		m_pCurProcessInfo->pushi3Node( pAttrSet);

		i3ExpProcessInfo * pOldInfo = m_pCurProcessInfo;

		if( _OnExportMaterial( pNode, pGrpInfo->m_pMtl, pAttrSet) == false)
			TexCoordCount = 0;

		// Texture Mapping을 위한 필요 정보
		if( pFaceMtl != NULL)
		{
			_SetTextureSet( 0, pFaceMtl, ID_DI);
		}

		switch( boneType)
		{
			case BONES_3DS :
			case PHYSIQUE :
				_OnExportSkin( pNode, pModifier, pMesh, pGrpInfo, bVertexColor, TexCoordCount);
				break;

				/*			
				_OnExportPhysiqueSkin( pNode, pModifier, pMesh, pGrpInfo, bVertexColor, TexCoordCount);
				break;
				*/

			default :
				m_pWeights = NULL;

				{
					pGeo = i3Geometry::new_object_ref();

					pGeo->SetName( pNode->GetName());

					{
						i3GeometryAttr * pGeoAttr;

						pGeoAttr = _BuildGeometry( pNode, pMesh, pGrpInfo, NULL, bVertexColor, TexCoordCount, 0);
						pGeo->AppendGeometryAttr( pGeoAttr);
					}

					m_pCurProcessInfo->pushi3Node( pGeo);
					m_pCurProcessInfo->popi3Node();		// Geometry;
				}
				break;
		}

		if( pOldInfo != m_pCurProcessInfo)
		{
			I3TRACE( "sdfsdf\n");
		}

		m_pCurProcessInfo->popi3Node();		// AttrSet;

	}

	_RemoveAllMeshGroup();

	if( m_pWeights != NULL)
	{
		delete m_pWeights;
		m_pWeights = NULL;
	}

	_RemoveAllJointInfo();
}
