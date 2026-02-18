#include "i3OptPCH.h"
#include "i3OptBuildLODBoneMap.h"

I3_CLASS_INSTANCE( i3OptBuildLODBoneMap);

i3OptBuildLODBoneMap::i3OptBuildLODBoneMap(void)
{
	m_Class = CLASS_SCENEGRAPH;
}

bool i3OptBuildLODBoneMap::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( !i3::kind_of<i3Body* >(pNode))
		return true;

	// LOD가 1개 이하의 경우는 Bone Map이 필요하지 않다.
	// 괜히 Memory만 낭비할 뿐.
	i3Body * pBody = (i3Body *) pNode;
	if( pBody->getLODCount() <= 1)
	{
		return true;
	}

	INT32 i, j, maxbone, k;
	i3LOD * pLOD, * pLOD_0 = nullptr;
	i3Skeleton * pSkel, * pSkel_0 = nullptr;
	INT32 * pBoneMap;

	// 우선 LOD 중에 Bone 구조가 같은 것이 있다면
	// 공유하도록 조치.
	{
		i3LOD * pLOD1, * pLOD2;

		for( i = 0; i < (INT32) pBody->getLODCount() - 1; i++)
		{
			for( j = i + 1; j < (INT32) pBody->getLODCount(); j++)
			{
				pLOD1 = pBody->getLOD( i);
				pLOD2 = pBody->getLOD( j);

				if( pLOD1->getSkeleton() == pLOD2->getSkeleton())
					continue;

				if( (pLOD1->getSkeleton() != nullptr) &&
					(pLOD2->getSkeleton() != nullptr))
				{
					if( pLOD1->getSkeleton()->IsSame( pLOD2->getSkeleton()))
					{
						// 앞선 LOD의 Skeleton을 공유해 사용하도록 조정.
						i3Skeleton * pSkel1 = pLOD1->getSkeleton();
						i3Skeleton * pSkel2 = pLOD2->getSkeleton();

						for( k = 0; k < pSkel1->getBoneCount(); k++)
						{
							if((pSkel1->isShapeSet( k) == false) && (pSkel2->isShapeSet( k) == true))
							{
								pSkel1->setShapeSet( k, pSkel2->getShapeSet( k));
							}
						}

						pLOD2->setSkeleton( pLOD1->getSkeleton());
					}
				}
			}
		}
	}

	// 가장 Bone이 복잡한 Level을 찾는다.
	// NOTE :
	//		항상 LOD-0가 가장 Bone이 많은 것은 아니다.
	for( i = 0, maxbone = 0; i < (INT32) pBody->getLODCount(); i++)
	{
		pLOD = pBody->getLOD( i);
		pSkel = pLOD->getSkeleton();

		if( pSkel != nullptr)
		{
			if( maxbone < (INT32) pSkel->getBoneCount())
			{
				maxbone = pSkel->getBoneCount();
				pLOD_0 = pLOD;
				pSkel_0 = pSkel;
			}
			else if( (INT32) pLOD->getBoneCount() == maxbone)
			{
				INT32 shapeSetCnt = pLOD->hasShapeSet();			

				if( shapeSetCnt > 0)
				{
					pLOD_0 = pLOD;
					pSkel_0 = pSkel;
					maxbone = pLOD->getBoneCount();
				}
			}
		}
	}

	for( i = 0; i < (INT32) pBody->getLODCount(); i++)
	{
		pLOD = pBody->getLOD( i);

		if( pLOD == pLOD_0)
			continue;

		pSkel = pLOD->getSkeleton();
		if( pSkel == nullptr)
			continue;

		pBoneMap = (INT32 *) i3MemAlloc( pSkel->getBoneCount() * sizeof(INT32));
		I3ASSERT( pBoneMap);

		for( j = 0; j < pSkel->getBoneCount(); j++)
		{
			i3Bone * pBone = pSkel->getBone( j);

			pBoneMap[j] = pSkel_0->FindBoneByName( pBone->getName());
			
			if( pBoneMap[j] == -1)
			{
				I3PRINTLOG(I3LOG_WARN,  "%s Bone에 대한 Map을 생성할 수 없습니다.\r\n이것은 Bone 구조가 같지 않은 두 LOD 때문일 수가 있습니다.",
					pBone->getName());
			}
		}

		pLOD->SetBoneMapTable( pBoneMap, pSkel->getBoneCount());

		i3MemFree( pBoneMap);
	}

	I3TRACE( "------------------------------\n");

	return true;
}
