#include "stdafx.h"
#include "AIPathFinder.h"
#include "AIPathData.h"
#include "AIStage.h"

#define GET_G(a, b) (D3DXVec3Length(&((a) - (b))))		// 시작노드 -> 현재노드 까지의 거리
#define GET_H(a, b) (D3DXVec3LengthSq(&((a) - (b)))*10)	// 현재노드 -> 목표노드 까지의 추정치 거리(휴리스틱)

CAIPathFinder	g_AIPathFinder;
 
BOOL CAIPathFinder::BuildNode(INT32 i32Idx)
{
	CAIPathFinderNode* pNodeParent;
	CAIPathFinderNode* pNodeChild;

	D3DXVECTOR3 PtGoal = m_pAIStage->m_vtMesh[m_i32BeginIdx].m_dxv3Center;

	while(1)
	{
		pNodeParent = &m_arNode[i32Idx];
		pNodeParent->SetStateClose();

		for(INT32 i = 0; i < 3; i++)
		{
			CAIMesh* pMesh = &m_pAIStage->m_vtMesh[i32Idx];
			CAIMesh* pMeshNb = pMesh->m_pNeighbor[i];
			if (pMeshNb != NULL)
			{
				pNodeChild = &m_arNode[pMesh->m_ui16NeighborIdx[i]];
				// 목적지 도달
				if (pMesh->m_ui16NeighborIdx[i] == m_i32BeginIdx)
				{
					pNodeChild->m_ui16ParentMesh = i32Idx;
					pNodeChild->m_i32Byun = i;
					pNodeChild->m_pParentMesh = &m_pAIStage->m_vtMesh[i32Idx];
					pNodeChild->m_pParentNode = pNodeParent;
					return TRUE;
				}
				// 닫힌 상태일때
				else if (pNodeChild->IsStateClose())
				{
				}
				// 열린 상태일때
				else if (pNodeChild->IsStateOpen())
				{
					REAL32 r32NewG = pNodeParent->GetG() + GET_G(m_pAIStage->m_vtMesh[i32Idx].m_dxv3Center, pMeshNb->m_dxv3Center);

					// 새 G값이 기존의 G값보다 작다면
					if (pNodeChild->GetG() > r32NewG)
					{
						// 새 G값을 셋팅하고 이에따른 H와 F값도 새로 계산합니다.
						pNodeChild->SetG(r32NewG);
						pNodeChild->SetH(GET_H(pMeshNb->m_dxv3Center, PtGoal));
						pNodeChild->AddGHToF();
					}
				}
				// 열림, 닫힘 둘다 아닐 경우, 열림목록에 추가합니다.
				else
				{
					pNodeChild->m_ui16ParentMesh = i32Idx;
					pNodeChild->m_i32Byun = i;
					pNodeChild->m_pParentMesh = &m_pAIStage->m_vtMesh[i32Idx];
					pNodeChild->m_pParentNode = pNodeParent;

					pNodeChild->SetStateOpen();
					pNodeChild->SetG(pNodeParent->m_r32G + GET_G(pMesh->m_dxv3Center, pMeshNb->m_dxv3Center));
					pNodeChild->SetH(GET_H(pMeshNb->m_dxv3Center, PtGoal));
					pNodeChild->AddGHToF();
				}
			}
		}

		i32Idx = FindMinFInOpenState();
		
		if (i32Idx == -1) return FALSE;
	}
	return FALSE;
}

INT32 CAIPathFinder::FindMinFInOpenState()
{
	BOOL	bFirst	= FALSE;
	REAL32	r32Min	= 0.f;
	INT32	i32Idx	= -1;
	INT32	i32NM	= m_pAIStage->m_vtMesh.size();

	for(INT32 i = 0; i < i32NM; i++)
	{
		if (m_arNode[i].m_i32State == STATE_ASTAR_ALGORITHM_OPEN)
		{
			if (bFirst == FALSE)
			{
				r32Min = m_arNode[i].m_r32F;
				i32Idx = i;				
				bFirst = TRUE;
			}
			else
			{
				if (m_arNode[i].m_r32F < r32Min || r32Min == 0.f)
				{
					r32Min = m_arNode[i].m_r32F;
					i32Idx = i;
				}
			}
		}
	}

	return i32Idx;
}

void CAIPathFinder::ClearState()
{
	for(UINT32 i = 0; i < m_pAIStage->m_vtMesh.size(); i++)
	{
		CAIPathFinderNode* pNode = &m_arNode[i];
		pNode->m_i32State = 0;
		pNode->m_ui16ParentMesh	= -1;
		pNode->m_r32G = 0.f;
		pNode->m_r32F = 0.f;
		pNode->m_r32H = 0.f;
		pNode->m_pMyMesh = &m_pAIStage->m_vtMesh[i];
		pNode->m_ui16MyMesh = i;
	}
}

BOOL CAIPathFinder::FindPath(CAIPathData* pPathData, CAIStage* pStage, INT32 i32BeginIdx, INT32 i32GoalIdx, D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos)
{
	m_pAIStage = pStage;
	m_i32BeginIdx = i32BeginIdx;

	pPathData->Clear();
	if (m_i32BeginIdx == i32GoalIdx) return TRUE;

	ClearState();

	// 길찾은후 찾은 패스를 저장
	BOOL bWayFound = BuildNode(i32GoalIdx);

	if (bWayFound)
	{
		CAIPathFinderNode* pNode = &m_arNode[m_i32BeginIdx];
		BOOL bResult;
		DS_LINE lnData;

		// 시작 노드 삽입(1개)
		
		pNode->m_pParentMesh->Get2DLine(pNode->m_i32Byun, &lnData);
		pPathData->InsertFirstNode(dxv3BeginPos, pNode->m_pParentMesh->m_dxv3Byun[pNode->m_i32Byun], pNode->m_ui16MyMesh, &lnData);

		// 중간 노드 삽입(n개)
		while(pNode != &m_arNode[i32GoalIdx])
		{
			pNode->m_pParentMesh->Get2DLine(pNode->m_i32Byun, &lnData);
			bResult = pPathData->InsertNode(pNode->m_pParentMesh->m_dxv3Byun[pNode->m_i32Byun], pNode->m_ui16MyMesh, &lnData);

			if (!bResult) break;			// MAX_WAYPATH_COUNT가 초과되어 더이상 Insert가 불가능하면 Insert작업을 그만합니다.
			pNode = pNode->m_pParentNode;
		}

		// 끝 노드 삽입(1개)
		pPathData->InsertNode(dxv3GoalPos, pNode->m_ui16MyMesh, NULL);
		
		//pPathData->Pull1st(dxv3BeginPos, dxv3GoalPos);
		pPathData->PullOld(dxv3BeginPos, dxv3GoalPos);
	}

	return bWayFound;
}