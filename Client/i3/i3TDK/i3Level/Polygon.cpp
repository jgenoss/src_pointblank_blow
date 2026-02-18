#include "stdafx.h"
#include "Polygon.h"
#include "Cube.h"

inline REAL32 g_CalDistance(D3DXVECTOR3 dxv3A, D3DXVECTOR3 dxv3B)
{
	return sqrt((dxv3A.x - dxv3B.x)*(dxv3A.x - dxv3B.x) + (dxv3A.y - dxv3B.y)*(dxv3A.y - dxv3B.y) + (dxv3A.z - dxv3B.z)*(dxv3A.z - dxv3B.z));
	//return (abs(dxv3A.x - dxv3B.x) + abs(dxv3A.y - dxv3B.y) + abs(dxv3A.z - dxv3B.z))*10;
}

REAL32 CPolygon::DistanceFromChild(INT32 i32Idx)
{
	return g_CalDistance(m_dxv3Center, m_pNear[i32Idx]->m_dxv3Center);
}

INT32 CPolygon::GetNNear()
{
	INT32 i32Count;
	if (m_pNear[0] != nullptr) i32Count = 1;
	else i32Count = 0;
	if (m_pNear[1] != nullptr) i32Count++;
	if (m_pNear[2] != nullptr) i32Count++;

	return i32Count;
}

void CPolygon::MakePlane(D3DXPLANE* pdxplData)
{
	D3DXPlaneFromPoints(pdxplData, &m_dxv3Point[0], &m_dxv3Point[1], &m_dxv3Point[2]);
}

BOOL CPolygon::CheckInPoint(D3DXVECTOR3* pdxv3Pt)
{
	D3DXPLANE dxplData;
	MakePlane(&dxplData);

	if (_finite(D3DXPlaneDotCoord(&dxplData, pdxv3Pt))) return TRUE;
	return FALSE;
}

BOOL CPolygon::IsIn(D3DXVECTOR3* pdxv3Pos)
{
	CCube cube;
	D3DXPLANE dxplData;
	cube.Set(this);
	
	// 폴리곤을 확장한 큐브의 안에 pos가 들어가지 않는다면 더 검사해 볼 필요도 없이 미포함 결정.
	if (!cube.IsIn(pdxv3Pos)) return FALSE;
	
	D3DXPlaneFromPoints(&dxplData, &m_dxv3Point[0], &m_dxv3Point[1], &m_dxv3Point[2]);

	/*
	D3DXVECTOR3 a, b, c;
	a.x = pos->x;
	a.y = cube.m_pos_high.y;	
	a.z = pos->z;
	b.x = pos->x;
	b.y = cube.m_pos_low.y;
	b.z = pos->z;*/

	D3DXVECTOR3 dxv3VT;
	D3DXVECTOR3 dxv3BB;
	dxv3BB.x = pdxv3Pos->x + 0.001f;
	dxv3BB.y = pdxv3Pos->y + 0.001f;
	dxv3BB.z = pdxv3Pos->z + 0.001f;
	//D3DXPlaneIntersectLine(&c, &plane, &a, &b);
	D3DXVECTOR3* pdxv3Tmp = D3DXPlaneIntersectLine(&dxv3VT, &dxplData, pdxv3Pos, &dxv3BB);
	if (pdxv3Tmp == nullptr) return FALSE;

	return TRUE;
}

// 자기의 변 3개와 비교대상(a)의 변 3개를 비교해 같은것이 있는지 검사
// 같은 것이 있다면 링크정보에 삽입
void CPolygon::FindNear(CPolygon* pPoly)
{
	for(INT32 i = 0; i < 3; i++)
	{
		INT32 j = i + 1;
		if (j == 3)
		{
			j = 0;
		}

		if (m_dxv3Point[i] == pPoly->m_dxv3Point[0] && m_dxv3Point[j] == pPoly->m_dxv3Point[1])
		{
			//InsertNearInfo( poly );
			m_pNear[0] = pPoly;
			return;
		}
		if (m_dxv3Point[i] == pPoly->m_dxv3Point[1] && m_dxv3Point[j] == pPoly->m_dxv3Point[0])
		{
			//InsertNearInfo( poly );
			m_pNear[0] = pPoly;
			return;
		}

		if (m_dxv3Point[i] == pPoly->m_dxv3Point[1] && m_dxv3Point[j] == pPoly->m_dxv3Point[2])
		{
			//InsertNearInfo( poly );
			m_pNear[1] = pPoly;
			return;
		}
		if (m_dxv3Point[i] == pPoly->m_dxv3Point[2] && m_dxv3Point[j] == pPoly->m_dxv3Point[1])
		{
			//InsertNearInfo( poly );
			m_pNear[1] = pPoly;
			return;
		}

		if (m_dxv3Point[i] == pPoly->m_dxv3Point[0] && m_dxv3Point[j] == pPoly->m_dxv3Point[2])
		{
			//InsertNearInfo( poly );
			m_pNear[2] = pPoly;
			return;
		}
		if (m_dxv3Point[i] == pPoly->m_dxv3Point[2] && m_dxv3Point[j] == pPoly->m_dxv3Point[0])
		{
			//InsertNearInfo( poly );
			m_pNear[2] = pPoly;
			return;
		}
	}
}

void CPolygon::InsertNearInfo(CPolygon* pPoly)
{
	if (m_pNear[0] == nullptr)
	{
		m_pNear[0] = pPoly;
		return;
	}
	if (m_pNear[1] == nullptr)
	{
		m_pNear[1] = pPoly;
		return;
	}
	m_pNear[2] = pPoly;
}

void CPolygon::Create(INT32 i32Idx, D3DXVECTOR3 dxv3A, D3DXVECTOR3 dxv3B, D3DXVECTOR3 dxv3C)
{
	m_i32Index = i32Idx;

	m_pNear[0] = nullptr;
	m_pNear[1] = nullptr;
	m_pNear[2] = nullptr;

	m_dxv3Point[0] = dxv3A;
	m_dxv3Point[1] = dxv3B;
	m_dxv3Point[2] = dxv3C;

	m_dxv3Center = (dxv3A + dxv3B + dxv3C)/3;
	
	m_dxv3ByunCenter[0]	= (dxv3A + dxv3B)/2;
	m_dxv3ByunCenter[1]	= (dxv3B + dxv3C)/2;
	m_dxv3ByunCenter[2]	= (dxv3C + dxv3A)/2;
}

void CPolygon::ReCal()
{
	m_dxv3Center = (m_dxv3Point[0] + m_dxv3Point[1] + m_dxv3Point[2])/3;
	
	m_dxv3ByunCenter[0]	= (m_dxv3Point[0] + m_dxv3Point[1])/2;
	m_dxv3ByunCenter[1]	= (m_dxv3Point[1] + m_dxv3Point[2])/2;
	m_dxv3ByunCenter[2]	= (m_dxv3Point[2] + m_dxv3Point[0])/2;
}