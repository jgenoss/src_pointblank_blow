#include "pch.h"
#include "ConvertBlendIndex.h"

#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/contain_string.h"

ConvertBlendIndex::ConvertBlendIndex() : m_pFrameSkel(NULL)
{
}

void ConvertBlendIndex::_ConvertFullPath( char * pDest, const char * pszPath)
{
	if( i3System::isAbsolutePath( pszPath))
	{
		i3::safe_string_copy( pDest, pszPath, MAX_PATH);
	}
	else
	{
		if (i3::generic_empty(i3ResourceFile::GetWorkingFolder()) )
			i3::snprintf( pDest, MAX_PATH, "%s", pszPath);
		else
			i3::snprintf( pDest, MAX_PATH, "%s/%s", i3ResourceFile::GetWorkingFolder(), pszPath);
	}
}

void ConvertBlendIndex::_BuildBoneMap( i3LOD * pLOD)
{
	INT32 i;
	INT32 boneMap[512];

	i3Skeleton * pSkel = pLOD->getSkeleton();

	// Frame Skeleton에 비춘 Bone Map을 생성한다.
	I3ASSERT( pSkel->getBoneCount() < 512);

	for( i = 0; i < pSkel->getBoneCount(); i++)
	{
		i3Bone * pBone = pSkel->getBone( i);

		INT32 idx = m_pFrameSkel->FindBoneByName( pBone->getName());

		if( idx == -1)
		{
			i3::string str;
			i3::sprintf( str, "LOD의 %s Bone이 Frame Skeleton에서 찾을 수 없습니다. Bone 구조를 확인해 주세요.", (const char*)pBone->getName() );
			I3PRINTLOG(I3LOG_FATAL, str.c_str() );
		}

		boneMap[i] = idx;
	}

	pLOD->SetBoneMapTable( boneMap, pSkel->getBoneCount());
}

void ConvertBlendIndex::_ConvertBlend( i3VertexArray * pVA, INT32 * boneMap, UINT32 boneMapCnt)
{
	I3ASSERT( boneMap != NULL);

	pVA->Lock();

	for( UINT32 i = 0; i < pVA->GetCount(); i++)
	{
		// Blend Index
		for( INT32 j = 0; j < pVA->GetFormat()->GetBlendIndexCount(); j++)
		{
			//로드될 본인덱스
			INT32 idxOldBone = (INT32) pVA->GetBlendIndex( j, i);
			// 현재 Part의 Bone 구조에 대한 Index를
			// 현재 LOD Upper Bone Hierarchy에서의 Index로 변환
			I3ASSERT( (UINT32)idxOldBone < boneMapCnt);
			I3ASSERT( boneMap[ idxOldBone] != -1);
			I3ASSERT( boneMap[ idxOldBone] < 255);
			
			pVA->SetBlendIndex( j, i, boneMap[ idxOldBone]);
		}
	}

	pVA->Unlock();
}

void ConvertBlendIndex::LoadFrameSkel( const char * pszFileName, INT32 lod)
{
	// Upper는 항상 0번 LOD를 사용
	char szPath[ MAX_PATH] = {0,};
	char szFullPath[ MAX_PATH] = {0,};

	i3::snprintf( szPath, MAX_PATH, "Avatar/Upper/%s/%s_LOD%d.i3s", pszFileName, pszFileName, lod);

	_ConvertFullPath( szFullPath, szPath);

	i3SceneFile file;
	UINT32 rv = file.Load( szFullPath);
	if( rv == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL, "file open fail!!! %s", szFullPath);
		return;
	}

	i3Node * pNode = file.getSceneGraphInfo()->getInstanceSg();
	i3Body * pBody = static_cast<i3Body*>(i3Scene::FindNodeByExactType( pNode, i3Body::static_meta()));
	I3ASSERT( pBody != NULL);

	I3_REF_CHANGE( m_pFrameSkel, pBody->getLOD( 0)->getSkeleton());
}

void ConvertBlendIndex::ConvertSG( const char * pszFrame, const char * pszFileName)
{
	char szPath[ MAX_PATH];
	char szFullPath[ MAX_PATH] = {0,};

	bool bFrame = true;

	for( UINT32 i = 0; i < AVT_LOD_COUNT; i++)
	{
		i3::snprintf( szPath, MAX_PATH, "%s_LOD%d.i3s", pszFileName, i);

		if( i3System::IsFile( szPath) == false)
			continue;

		LoadFrameSkel( pszFrame, i);

		i3SceneFile file;
		UINT32 rv = file.Load( szPath);
		if( rv == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL, "file open fail!!! %s", szPath);
			return;
		}

		i3Node * pNode = file.getSceneGraphInfo()->getInstanceSg();
		i3Body * pBody = static_cast<i3Body*>(i3Scene::FindNodeByExactType( pNode, i3Body::static_meta()));
		I3ASSERT( pBody != NULL);

		{
			_BuildBoneMap( pBody->getLOD( 0));

			i3::vector<i3Node*> list;
			i3Scene::FindNodeByExactType( pBody->getLOD( 0)->getShapeNode(), i3Geometry::static_meta(), list);
			i3::vector<i3VertexArray*> BakedVAList;

			for( UINT32 a = 0; a < list.size(); a++)
			{
				i3Geometry * pGeo = static_cast<i3Geometry*>(list.at(a));

				for( INT32 k = 0; k < pGeo->GetGeometryAttrCount(); k++)
				{
					i3RenderAttr * pAttr = pGeo->GetGeometryAttr( k);
					if( i3::same_of<i3GeometryAttr*>(pAttr))
					{
						i3GeometryAttr * pGeoAttr = static_cast<i3GeometryAttr*>( pAttr);

						if( (pGeoAttr->getGeoStyle() & I3_GEOMETRY_STYLE_PREBAKED) == 0)
						{
							bool bSameVA = false;
							for( UINT32 f = 0; f < BakedVAList.size(); f++)
							{
								if( BakedVAList.at( f) == pGeoAttr->GetVertexArray() )
								{
									bSameVA = true;
									break;
								}
							}
							if( bSameVA == false)
							{
								_ConvertBlend( pGeoAttr->GetVertexArray(), pBody->getLOD(0)->getBoneMap(), pBody->getLOD(0)->getBoneCount() );
								BakedVAList.push_back( pGeoAttr->GetVertexArray());
							}

							pGeoAttr->addGeoStyle( I3_GEOMETRY_STYLE_PREBAKED);
						}
					}
				}
			}

			pBody->getLOD( 0)->setBoneMap( NULL);

			//if( bFrame)
			{// FrameSkeleton으로 잡은 Sg로 바꾼다.
				pBody->getLOD( 0)->setSkeleton( m_pFrameSkel);
			//	bFrame = false;
			}
		}

		file.Save( szPath);
	}
}