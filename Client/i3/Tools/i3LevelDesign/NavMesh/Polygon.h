#pragma once

#include <d3dx9.h>

class CPolygon
{
public:
	D3DXVECTOR3		m_dxv3Center;
	D3DXVECTOR3		m_dxv3Point[3];			// 꼭지점 3개
	D3DXVECTOR3		m_dxv3ByunCenter[3];	// 두 점의 중점( = 한 변의 중점 )
	// 변 0,1의 중점 => m_vCenter[0]
	// 변 1,2의 중점 => m_vCenter[1]
	// 변 2,0의 중점 => m_vCenter[2]

	CPolygon*		m_pNear[3];				// 붙어있는 삼각형	
	// 변0,1에 붙은것 => m_pNear[0]
	// 변1,2에 붙은것 => m_pNear[1]
	// 변2,0에 붙은것 => m_pNear[2]

	INT32			m_i32Index;

	void			Create(INT32 i32Idx, D3DXVECTOR3 dxv3A, D3DXVECTOR3 dxv3B, D3DXVECTOR3 dxv3C);
	void			FindNear(CPolygon* pPoly);				// 자기의 변 3개와 비교대상(a)의 변 3개를 비교해 같은것이 있는지 검사. 같은 것이 있다면 링크정보에 삽입
	void			ReCal();
	INT32			GetNNear();
	REAL32			DistanceFromChild(INT32 i32Idx);
	void			InsertNearInfo(CPolygon* pPoly);

	void			MakePlane(D3DXPLANE* pdxplData);
	BOOL			CheckInPoint(D3DXVECTOR3* pdxv3Pt);

	BOOL			IsIn(D3DXVECTOR3* pdxv3Pos);
};