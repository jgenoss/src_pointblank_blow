#include "stdafx.h"
#include "Common.h"
#include "AIPathData.h"

INT32	g_i32StartIdx	= -1;
INT32	g_i32EndIdx		= -1;

// pplane평면이 pData폴리곤 앞에 있는지 뒤에 있는지 판별
INT32 g_CheckFrontOrBack(D3DXPLANE *pdxplData, CPolygon* pData)
{
	REAL32 r32f1 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Point[0]);
	REAL32 r32f2 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Point[1]);
	REAL32 r32f3 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Point[2]);

	if (r32f1 >= 0.f && r32f2 >= 0.f && r32f3 >= 0.f)			return 0;	// 앞에 있음
	if (!_finite(r32f1) && !_finite(r32f2) && !_finite(r32f3))	return 0;	// 평면과 일치
	if (r32f1 < 0.f && r32f2 < 0.f && r32f3 < 0.f)				return 1;	// 뒤에 있음
	
	return 2;	// 평면에 끼임 ( 분할이 요구됨 )
}

INT32 g_CheckFrontOrBack(D3DXPLANE *pdxplData, CAIMesh *pData)
{
	REAL32 r32f1 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[0]);
	REAL32 r32f2 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[1]);
	REAL32 r32f3 = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[2]);

	if (r32f1 >= 0.f && r32f2 >= 0.f && r32f3 >= 0.f)			return 0;	// 앞에 있음
	if (!_finite(r32f1) && !_finite(r32f2) && !_finite(r32f3))	return 0;	// 평면과 일치
	if (r32f1 < 0.f && r32f2 < 0.f && r32f3 < 0.f)				return 1;	// 뒤에 있음
	
	return 2;	// 평면에 끼임 ( 분할이 요구됨 )
}

// pStandard폴리곤을 평면화 했을때 pData폴리곤이 앞에 있는지 뒤에 있는지 판별
INT32 g_CheckFrontOrBack(CPolygon* pStandard, CPolygon* pData)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane, &pStandard->m_dxv3Point[0], &pStandard->m_dxv3Point[1], &pStandard->m_dxv3Point[2]);

	return g_CheckFrontOrBack(&plane, pData);
}

INT32 g_CheckFrontOrBack(CAIMesh* pStandard, CAIMesh* pData)
{
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane, &pStandard->m_dxv3Pt[0], &pStandard->m_dxv3Pt[1], &pStandard->m_dxv3Pt[2]);

	return g_CheckFrontOrBack(&plane, pData);
}

// a폴리곤을 평면화 했을때 b폴리곤을 1개에서 3개로 분할.
// 앞폴리곤이 1개, 뒤폴리곤이 2개로 분할되면 0리턴
// 앞폴리곤이 2개, 뒤폴리곤이 1개로 분할되면 1리턴
//int SplitPolygon( D3DXPLANE* plane, CPolygon* pData, CPolygon* pSplit1, CPolygon* pSplit2, CPolygon* pSplit3 )
INT32 g_SplitPolygon(D3DXPLANE* pdxplData, CAIMesh* pData, CAIMesh* pSplit1, CAIMesh* pSplit2, CAIMesh* pSplit3)
{
	REAL32 r32f[3];
	r32f[0] = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[0]);
	r32f[1] = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[1]);
	r32f[2] = D3DXPlaneDotCoord(pdxplData, &pData->m_dxv3Pt[2]);
	// pStandard와 pData의 인풋이 이상없다면 f[3]중에 양수:음수의 비중이 1:2 거나 2:1 임.

	INT32 i32Front = 0;
	INT32 i32Back = 0;
	D3DXVECTOR3 dxv3Front[2];
	D3DXVECTOR3 dxv3Back[2];

	for(INT32 i = 0; i < 3; i++)
	{
		if (r32f[i] < 0.f)
		{
			dxv3Back[i32Back] = pData->m_dxv3Pt[i];
			i32Back++;
		}
		else
		{
			dxv3Front[i32Front] = pData->m_dxv3Pt[i];
			i32Front++;
		}
	}

	D3DXVECTOR3 dxv3Result[2];

	// 앞면에 점 1개, 뒷면에 점 2개
	if (i32Front == 1 && i32Back == 2)
	{
		D3DXPlaneIntersectLine(&dxv3Result[0], pdxplData, &dxv3Front[0], &dxv3Back[0]);
		D3DXPlaneIntersectLine(&dxv3Result[1], pdxplData, &dxv3Front[0], &dxv3Back[1]);

		// 앞 폴리곤 1개
		pSplit1->m_dxv3Pt[0] = dxv3Front[0];
		pSplit1->m_dxv3Pt[1] = dxv3Result[0];
		pSplit1->m_dxv3Pt[2] = dxv3Result[1];

		// 뒤 폴리곤 2개 중 1번째
		pSplit2->m_dxv3Pt[0] = dxv3Back[0];
		pSplit2->m_dxv3Pt[1] = dxv3Result[0];
		pSplit2->m_dxv3Pt[2] = dxv3Result[1];

		// 뒤 폴리곤 2개 중 2번째
		pSplit3->m_dxv3Pt[0] = dxv3Back[1];
		pSplit3->m_dxv3Pt[1] = dxv3Back[0];
		pSplit3->m_dxv3Pt[2] = dxv3Result[1];

		return 0;
	}
	// 앞면에 점 2개, 뒷면에 점 1개
	else if (i32Front == 2 && i32Back == 1)
	{
		D3DXPlaneIntersectLine(&dxv3Result[0], pdxplData, &dxv3Front[0], &dxv3Back[0]);
		D3DXPlaneIntersectLine(&dxv3Result[1], pdxplData, &dxv3Front[1], &dxv3Back[0]);

		// 앞 폴리곤 2개 중 1번째
		pSplit1->m_dxv3Pt[0] = dxv3Front[0];
		pSplit1->m_dxv3Pt[1] = dxv3Front[1];
		pSplit1->m_dxv3Pt[2] = dxv3Result[0];

		// 앞 폴리곤 2개 중 2번째
		pSplit2->m_dxv3Pt[0] = dxv3Front[1];
		pSplit2->m_dxv3Pt[1] = dxv3Result[0];
		pSplit2->m_dxv3Pt[2] = dxv3Result[1];

		// 뒤 폴리곤 1개
		pSplit3->m_dxv3Pt[0] = dxv3Result[0];
		pSplit3->m_dxv3Pt[1] = dxv3Result[1];
		pSplit3->m_dxv3Pt[2] = dxv3Back[0];

		return 1;
	}
	else
	{
		assert(FALSE);
	}

	return 2;
}

void g_Draw3DLine(D3DXVECTOR3* pdxv3A, D3DXVECTOR3* pdxv3B, D3DCOLOR color)
{ 
	//struct T_LINE_3D
	//{ 
	//	D3DVECTOR pos;
	//	D3DCOLOR color;
	//};
	//const DWORD fvf = D3DFVF_XYZ|D3DFVF_DIFFUSE;
	//D3DXMATRIX zero_mat;
	//D3DXMatrixTranslation(&zero_mat, 0.f, 0.f, 0.f);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &zero_mat);
	//T_LINE_3D lines[2];
	//memcpy(&lines[0].pos, pdxv3A, sizeof(D3DVECTOR));
	//lines[0].color = color;
	//memcpy(&lines[1].pos, pdxv3B, sizeof(D3DVECTOR));
	//lines[1].color = color;
	//g_pd3dDevice->SetFVF(fvf);
	//g_pd3dDevice->SetTexture(0, nullptr);
	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, lines, sizeof(T_LINE_3D));
}

BOOL g_KeyPress(INT32 i32Key)
{
	static BOOL bKeyStae[VK_OEM_CLEAR + 1] = { FALSE, };
	if (GetAsyncKeyState(i32Key))
	{
		if (!bKeyStae[i32Key])
		{
			bKeyStae[i32Key] = TRUE;
			return TRUE;
		}
	}
	else bKeyStae[i32Key] = FALSE;
	return FALSE;
}

BOOL g_GetIntersectPoint(DS_LINE* plnA, DS_LINE* plnB, D3DXVECTOR2* pdxv2Out)
{
	D3DXVECTOR2 dxv2AP1, dxv2AP2, dxv2BP1, dxv2BP2;
	memcpy(&dxv2AP1, &plnA->m_p1, sizeof(D3DXVECTOR2));
	memcpy(&dxv2AP2, &plnA->m_p2, sizeof(D3DXVECTOR2));
	memcpy(&dxv2BP1, &plnB->m_p1, sizeof(D3DXVECTOR2));
	memcpy(&dxv2BP2, &plnB->m_p2, sizeof(D3DXVECTOR2));

	REAL32 r32t;
	REAL32 r32s; 
	REAL32 r32Under = (dxv2BP2.y - dxv2BP1.y)*(dxv2AP2.x - dxv2AP1.x)-(dxv2BP2.x - dxv2BP1.x)*(dxv2AP2.y - dxv2AP1.y);
	if (r32Under == 0) return FALSE;

	REAL32 r32_t = (dxv2BP2.x - dxv2BP1.x)*(dxv2AP1.y - dxv2BP1.y) - (dxv2BP2.y - dxv2BP1.y)*(dxv2AP1.x - dxv2BP1.x);
	REAL32 r32_s = (dxv2AP2.x - dxv2AP1.x)*(dxv2AP1.y - dxv2BP1.y) - (dxv2AP2.y - dxv2AP1.y)*(dxv2AP1.x - dxv2BP1.x); 

	r32t = r32_t/r32Under;
	r32s = r32_s/r32Under; 

	if (r32t < 0.f || r32t > 1.f || r32s < 0.f || r32s > 1.f) return FALSE;
	if (r32_t == 0 && r32_s == 0) return FALSE;

	pdxv2Out->x = dxv2AP1.x + r32t*(dxv2AP2.x - dxv2AP1.x);
	pdxv2Out->y = dxv2AP1.y + r32t*(dxv2AP2.y - dxv2AP1.y);

	return TRUE;
}

BOOL g_GetIntersectPoint(DS_LINE* plnA, DS_LINE* plnB)
{
	D3DXVECTOR2 dxv2AP1, dxv2AP2, dxv2BP1, dxv2BP2;
	memcpy(&dxv2AP1, &plnA->m_p1, sizeof(D3DXVECTOR2));
	memcpy(&dxv2AP2, &plnA->m_p2, sizeof(D3DXVECTOR2));
	memcpy(&dxv2BP1, &plnB->m_p1, sizeof(D3DXVECTOR2));
	memcpy(&dxv2BP2, &plnB->m_p2, sizeof(D3DXVECTOR2));

	REAL32 r32t;
	REAL32 r32s; 
	REAL32 r32Under = (dxv2BP2.y - dxv2BP1.y)*(dxv2AP2.x - dxv2AP1.x)-(dxv2BP2.x - dxv2BP1.x)*(dxv2AP2.y - dxv2AP1.y);
	if (r32Under == 0) return FALSE;

	REAL32 r32_t = (dxv2BP2.x - dxv2BP1.x)*(dxv2AP1.y - dxv2BP1.y) - (dxv2BP2.y - dxv2BP1.y)*(dxv2AP1.x - dxv2BP1.x);
	REAL32 r32_s = (dxv2AP2.x - dxv2AP1.x)*(dxv2AP1.y - dxv2BP1.y) - (dxv2AP2.y - dxv2AP1.y)*(dxv2AP1.x - dxv2BP1.x); 

	r32t = r32_t/r32Under;
	r32s = r32_s/r32Under; 

	if (r32t < 0.f || r32t > 1.f || r32s < 0.f || r32s > 1.f) return FALSE;
	if (r32_t == 0 && r32_s == 0) return FALSE;

	return TRUE;
}

INT32 g_CCW(D3DXVECTOR2 dxv2Pt0, D3DXVECTOR2 dxv2Pt1, D3DXVECTOR2 dxv2Pt2)
{
	REAL32 r32dx1 = dxv2Pt1.x - dxv2Pt0.x;
	REAL32 r32dy1 = dxv2Pt1.y - dxv2Pt0.y;
	REAL32 r32dx2 = dxv2Pt2.x - dxv2Pt0.x;
	REAL32 r32dy2 = dxv2Pt2.y - dxv2Pt0.y;

	if (r32dx1*r32dy2 > r32dy1*r32dx2)										return 1;
	if (r32dx1*r32dy2 < r32dy1*r32dx2)										return -1;
	if (r32dx1 == 0 && r32dy1 == 0)											return 0;
	if ((r32dx1*r32dx2 < 0) || (r32dy1*r32dy2 < 0))							return -1;
	if ((r32dx1*r32dx1 + r32dy1*r32dy1) < (r32dx2*r32dx2 + r32dy2*r32dy2))	return 1;

	return 0;
}

BOOL g_Intersect(DS_LINE ln1, DS_LINE ln2) 
{
	return ((g_CCW(ln1.m_p1, ln1.m_p2, ln2.m_p1)*(g_CCW(ln1.m_p1, ln1.m_p2, ln2.m_p2)) <= 0) && (g_CCW(ln2.m_p1, ln2.m_p2, ln1.m_p1)*(g_CCW(ln2.m_p1, ln2.m_p2, ln1.m_p2)) <= 0));
} 

void DS_LINE::Draw(D3DCOLOR color)
{		
	D3DXVECTOR3 p1, p2;

	p1.x = m_p1.x;
	p1.y = 1.99f;
	p1.z = m_p1.y;

	p2.x = m_p2.x;
	p2.y = 1.99f;
	p2.z = m_p2.y;

	g_Draw3DLine(&p1, &p2, color);
}

void DS_LINE::Draw()
{		
	D3DXVECTOR3 p1, p2;

	p1.x = m_p1.x;
	p1.y = 1.99f;
	p1.z = m_p1.y;

	p2.x = m_p2.x;
	p2.y = 1.99f;
	p2.z = m_p2.y;

	D3DCOLOR color = RGB(30, 200, 50);
	g_Draw3DLine(&p1, &p2, color);
}

void DS_LINE::Set(PATHNODE* pPathNode)
{
	D3DXVECTOR3 dxv3Pos;

	pPathNode->GetPosBegin(&dxv3Pos);
	m_p1.x = dxv3Pos.x;
	m_p1.y = dxv3Pos.z;
	
	pPathNode->GetGoal(&dxv3Pos);
	m_p2.x = dxv3Pos.x;
	m_p2.y = dxv3Pos.z;
}

void DS_LINE::Set(PATHNODE_PULLED* pPathNode)
{
	pPathNode->GetPosBegin(&m_p1);
	pPathNode->GetGoal(&m_p2);
}