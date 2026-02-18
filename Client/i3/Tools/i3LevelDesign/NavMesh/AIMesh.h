#pragma once

#include <d3dx9.h>

struct DS_LINE;

class CAIMesh
{
public:
	D3DXVECTOR3		m_dxv3Pt[3];		// 꼭지점
	D3DXVECTOR3		m_dxv3Byun[3];		// 변의 중점
	CAIMesh*		m_pNeighbor[3];		// 이웃 메시
	UINT16			m_ui16NeighborIdx[3];
	INT8			m_i8NeighborByun[3];	// 이웃 메시의 몇 번 변과 이웃하는가?
	D3DXVECTOR3		m_dxv3Center;		// 중간점

	UINT8			m_ui8Angle;
	REAL32			m_r32Distance[3];
	
	VOID			CheckNeighbor(INT32 i32p_idx, CAIMesh* pMesh);
	INT32			GetNeighborCount();
	VOID			Create(D3DXVECTOR3* pdxv3Pt, UINT8 ui8Angle);
	VOID			Get2DLine(INT32 i32Byun, DS_LINE* plnData);
};
