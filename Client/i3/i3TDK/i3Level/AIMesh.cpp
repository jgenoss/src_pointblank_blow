#include "stdafx.h"
#include "AIMesh.h"
#include "Common.h"

void CAIMesh::CheckNeighbor(INT32 i32p_idx, CAIMesh* pMesh)
{
	INT32 i32a_idx[3];
	INT32 i32b_idx[3];
	INT32 i32Idx = 0;

	if (m_dxv3Pt[0] == pMesh->m_dxv3Pt[0])
	{
		i32a_idx[0] = 0;
		i32b_idx[0] = 0;
		i32Idx = 1;
	}
	else if (m_dxv3Pt[0] == pMesh->m_dxv3Pt[1])
	{
		i32a_idx[0] = 0;
		i32b_idx[0] = 1;
		i32Idx = 1;			
	}
	else if (m_dxv3Pt[0] == pMesh->m_dxv3Pt[2])
	{
		i32a_idx[0] = 0;
		i32b_idx[0] = 2;
		i32Idx = 1;
	}

	if (m_dxv3Pt[1] == pMesh->m_dxv3Pt[0])
	{
		i32a_idx[i32Idx] = 1;
		i32b_idx[i32Idx] = 0;
		i32Idx++;
	}
	else if (m_dxv3Pt[1] == pMesh->m_dxv3Pt[1])
	{
		i32a_idx[i32Idx] = 1;
		i32b_idx[i32Idx] = 1;
		i32Idx++;			
	}
	else if (m_dxv3Pt[1] == pMesh->m_dxv3Pt[2])
	{
		i32a_idx[i32Idx] = 1;
		i32b_idx[i32Idx] = 2;
		i32Idx++;
	}

	if (m_dxv3Pt[2] == pMesh->m_dxv3Pt[0])
	{
		i32a_idx[i32Idx] = 2;
		i32b_idx[i32Idx] = 0;
		i32Idx++;
	}
	else if (m_dxv3Pt[2] == pMesh->m_dxv3Pt[1])
	{
		i32a_idx[i32Idx] = 2;
		i32b_idx[i32Idx] = 1;
		i32Idx++;			
	}
	else if (m_dxv3Pt[2] == pMesh->m_dxv3Pt[2])
	{
		i32a_idx[i32Idx] = 2;
		i32b_idx[i32Idx] = 2;
		i32Idx++;
	}

	// idx값 0, 1 - 서로 관련없는 폴리곤
	//       2 - 한변이 서로 일치하므로 이웃 폴리곤 정보에 삽입합니다.
	//       3 - 세변이 일치한다. 잘못된 자료구조임. 같은 폴리곤이 2개이상 있는 네비게이션 메시 구조는 존재할수 없습니다.

	// 더 빠르게 할수 있는 알고리즘이 충분히 나올수 있습니다.
	// 구현이 선이라 최적화는 차후 여유있을때 진행하도록 하겠습니다.
	if (i32Idx == 2)
	{
		INT32 tmp = 0;
		if (i32a_idx[0] == 0 && i32a_idx[1] == 1) tmp = 0;
		else if (i32a_idx[0] == 1 && i32a_idx[1] == 0) tmp = 0;
		else if (i32a_idx[0] == 1 && i32a_idx[1] == 2) tmp = 1;
		else if (i32a_idx[0] == 2 && i32a_idx[1] == 1) tmp = 1;
		else if (i32a_idx[0] == 0 && i32a_idx[1] == 2) tmp = 2;
		else if (i32a_idx[0] == 2 && i32a_idx[1] == 0) tmp = 2;

		INT32 tmp2 = 0;
		if (i32b_idx[0] == 0 && i32b_idx[1] == 1) tmp2 = 0;
		else if (i32b_idx[0] == 1 && i32b_idx[1] == 0) tmp2 = 0;
		else if (i32b_idx[0] == 1 && i32b_idx[1] == 2) tmp2 = 1;
		else if (i32b_idx[0] == 2 && i32b_idx[1] == 1) tmp2 = 1;
		else if (i32b_idx[0] == 0 && i32b_idx[1] == 2) tmp2 = 2;
		else if (i32b_idx[0] == 2 && i32b_idx[1] == 0) tmp2 = 2;

		m_pNeighbor[tmp] = pMesh;
		I3ASSERT( tmp2 > -128);
		I3ASSERT( tmp2 < 128);
		m_i8NeighborByun[tmp] = (INT8)tmp2;
		I3ASSERT( i32p_idx > -1);
		I3ASSERT( i32p_idx < 65536);
		m_ui16NeighborIdx[tmp] = (UINT16)i32p_idx; 
	}
}

INT32 CAIMesh::GetNeighborCount()
{
	if (m_pNeighbor[0] == nullptr)
	{
		if (m_pNeighbor[1] == nullptr)
		{
			if (m_pNeighbor[2] == nullptr)	return 0;
			else						return 1;
		}
		else
		{
			if (m_pNeighbor[2] == nullptr)	return 1;
			else						return 2;
		}
	}
	else
	{
		if (m_pNeighbor[1] == nullptr)
		{
			if (m_pNeighbor[2] == nullptr)	return 1;
			else						return 2;
		}
		else
		{
			if (m_pNeighbor[2] == nullptr)	return 2;
			else						return 3;
		}
	}
}

void CAIMesh::Create(D3DXVECTOR3* pdxv3Pt, UINT8 ui8Angle)
{
	m_ui8Angle = ui8Angle;
	memcpy(m_dxv3Pt, pdxv3Pt, sizeof(D3DXVECTOR3)*3);
	ZeroMemory(m_pNeighbor, sizeof(CAIMesh*)*3);
	ZeroMemory(m_i8NeighborByun, sizeof(INT8)*3);		// 명백하게 버그였었음.. INT32 대신 작은 타입으로 변경..

	m_dxv3Center = (m_dxv3Pt[0] + m_dxv3Pt[1] + m_dxv3Pt[2])/3;

	m_dxv3Byun[0] = (m_dxv3Pt[0] + m_dxv3Pt[1])/2;
	m_dxv3Byun[1] = (m_dxv3Pt[1] + m_dxv3Pt[2])/2;
	m_dxv3Byun[2] = (m_dxv3Pt[2] + m_dxv3Pt[0])/2;
}

void CAIMesh::Get2DLine(INT32 i32Byun, DS_LINE* plnData)
{
	switch(i32Byun)
	{
	case 0:
		plnData->m_p1.x = m_dxv3Pt[0].x;
		plnData->m_p1.y = m_dxv3Pt[0].z;
		plnData->m_p2.x = m_dxv3Pt[1].x;
		plnData->m_p2.y = m_dxv3Pt[1].z;
		return;
	case 1:
		plnData->m_p1.x = m_dxv3Pt[1].x;
		plnData->m_p1.y = m_dxv3Pt[1].z;
		plnData->m_p2.x = m_dxv3Pt[2].x;
		plnData->m_p2.y = m_dxv3Pt[2].z;
		return;
	case 2:
		plnData->m_p1.x = m_dxv3Pt[2].x;
		plnData->m_p1.y = m_dxv3Pt[2].z;
		plnData->m_p2.x = m_dxv3Pt[0].x;
		plnData->m_p2.y = m_dxv3Pt[0].z;
		return;
	}
	assert(TRUE);
}
