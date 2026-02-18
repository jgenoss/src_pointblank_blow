#pragma once

#define STATE_ASTAR_ALGORITHM_NOTHING	0
#define STATE_ASTAR_ALGORITHM_OPEN		1
#define STATE_ASTAR_ALGORITHM_CLOSE		2

#include "AIMesh.h"
#include "Polygon.h"

#define MAX_WAYPATH_COUNT	256
#define MAX_NAVIMESH_COUNT	8192

extern INT8					g_i8Mode;
extern D3DPRESENT_PARAMETERS g_d3dpp;


enum {
	MODE_EDIT,
	MODE_RAY,
	MODE_PATHFIND,

	MODE_MAX_COUNT
};

class PATHNODE;
class PATHNODE_PULLED;

struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;
    D3DCOLOR	color;
};

struct DS_LINE
{
	D3DXVECTOR2 m_p1;
	D3DXVECTOR2 m_p2;

	VOID Clear()
	{
		m_p1.x = 0.f;
		m_p1.y = 0.f;
		m_p2.x = 0.f;
		m_p2.y = 0.f;
	}

	VOID SetBegin(D3DXVECTOR2* pdxv2Pos)
	{
		memcpy(&m_p1, pdxv2Pos, sizeof(D3DXVECTOR2));
	}

	VOID SetBegin(D3DXVECTOR3* pdxv3Pos)
	{
		m_p1.x = pdxv3Pos->x;
		m_p1.y = pdxv3Pos->z;
	}

	VOID SetEnd(D3DXVECTOR3* pdxv3Pos)
	{
		m_p2.x = pdxv3Pos->x;
		m_p2.y = pdxv3Pos->z;
	}

	void GetNormal(D3DXVECTOR2* pdxv2Out)
	{
		D3DXVECTOR2 dxv2Tmp;
		dxv2Tmp = m_p2 - m_p1;
		D3DXVec2Normalize(pdxv2Out, &dxv2Tmp);
	}
	REAL32 GetLength()
	{
		D3DXVECTOR2 dxv2Tmp;
		dxv2Tmp = m_p2 - m_p1;
		return D3DXVec2Length(&dxv2Tmp);
	}
	void Draw(D3DCOLOR color);
	void Draw();
	void Set(REAL32 p1x, REAL32 p1y, REAL32 p2x, REAL32 p2y)
	{
		m_p1.x = p1x;
		m_p1.y = p1y;
		m_p2.x = p2x;
		m_p2.y = p2y;
	}
	void Set(D3DXVECTOR2 dxv2p1, D3DXVECTOR2 dxv2p2)
	{
		memcpy(&m_p1, &dxv2p1, sizeof(D3DXVECTOR2));
		memcpy(&m_p2, &dxv2p2, sizeof(D3DXVECTOR2));
	}
	void Set(D3DXVECTOR2 dxv2p1, D3DXVECTOR3 dxv3p2)
	{
		memcpy(&m_p1, &dxv2p1, sizeof(D3DXVECTOR2));
		m_p2.x = dxv3p2.x;
		m_p2.y = dxv3p2.z;
	}
	void Set(D3DXVECTOR3* pdxv3p1, D3DXVECTOR3* pdxv3p2)
	{
		m_p1.x = pdxv3p1->x;
		m_p1.y = pdxv3p1->z;
		m_p2.x = pdxv3p2->x;
		m_p2.y = pdxv3p2->z;
	}
	void Set(PATHNODE* pPathNode);
	void Set(PATHNODE_PULLED* pPathNode);
	void SetCross(D3DXVECTOR3 dxv3Pos, BOOL bDir)
	{
		if (bDir)
		{
			m_p1.x = dxv3Pos.x - 0.3f;
			m_p1.y = dxv3Pos.z;
			m_p2.x = dxv3Pos.x + 0.3f;
			m_p2.y = dxv3Pos.z;
		}
		else
		{
			m_p1.x = dxv3Pos.x;
			m_p1.y = dxv3Pos.z - 0.3f;
			m_p2.x = dxv3Pos.x;
			m_p2.y = dxv3Pos.z + 0.3f;
		}
	}
	void SetCross(D3DXVECTOR2 dxv2Pos, BOOL bDir)
	{
		if (bDir)
		{
			m_p1.x = dxv2Pos.x - 0.3f;
			m_p1.y = dxv2Pos.y;
			m_p2.x = dxv2Pos.x + 0.3f;
			m_p2.y = dxv2Pos.y;
		}
		else
		{
			m_p1.x = dxv2Pos.x;
			m_p1.y = dxv2Pos.y - 0.3f;
			m_p2.x = dxv2Pos.x;
			m_p2.y = dxv2Pos.y + 0.3f;
		}
	}
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

extern INT8					g_i8CurrentMode;
//extern LPDIRECT3DDEVICE9	g_pd3dDevice;
extern INT32				g_i32StartIdx;
extern INT32				g_i32EndIdx;
extern LPD3DXLINE			g_Line;

extern REAL32 g_CalDistance(D3DXVECTOR3 dxv3A, D3DXVECTOR3 dxv3B);
extern INT32 g_SplitPolygon(D3DXPLANE* plane, CAIMesh* pData, CAIMesh* pSplit1, CAIMesh* pSplit2, CAIMesh* pSplit3);
extern void g_Draw3DLine(D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DCOLOR color);
extern void g_Draw3DLine(DS_LINE* pLine, D3DCOLOR color);
extern REAL32 g_CalDistance(D3DXVECTOR3 dxv3A, D3DXVECTOR3 dxv3B);
extern INT32 g_CheckFrontOrBack(D3DXPLANE* pplane, CAIMesh* pData);
extern INT32 g_CheckFrontOrBack(CAIMesh* pStandard, CAIMesh* pData);

extern BOOL g_KeyPress(INT32 i32Key);
extern INT32 g_CCW(D3DXVECTOR2 Pt0, D3DXVECTOR2 Pt1, D3DXVECTOR2 Pt2);
extern BOOL g_Intersect(DS_LINE l1, DS_LINE l2);
extern BOOL g_GetIntersectPoint(DS_LINE* pLineA, DS_LINE* pLineB, D3DXVECTOR2* pPtOut);
extern BOOL g_GetIntersectPoint(DS_LINE* pLineA, DS_LINE* pLineB);