#include "stdafx.h"
#include "Cube.h"
#include "Common.h"

#include <stdio.h>

CCube::CCube()
{
	Init();
}

void CCube::Init()
{
	m_dxv3PosLow.x = FLT_MAX;
	m_dxv3PosLow.y = FLT_MAX;
	m_dxv3PosLow.z = FLT_MAX;

	m_dxv3PosHigh.x = FLT_MIN;
	m_dxv3PosHigh.y = FLT_MIN;
	m_dxv3PosHigh.z = FLT_MIN;
}

void CCube::GetPlaneXYZAxis(D3DXPLANE* pXplane, D3DXPLANE* pYplane, D3DXPLANE* pZplane)
{
	D3DXVECTOR3	dxv3Center = (m_dxv3PosLow + m_dxv3PosHigh) / 2;

	D3DXVECTOR3 dxv3Xnormal(1.f, 0.f, 0.f);
	D3DXVECTOR3 dxv3Ynormal(0.f, 1.f, 0.f);
	D3DXVECTOR3 dxv3Znormal(0.f, 0.f, 1.f);

	/*D3DXPlaneFromPointNormal( x, &vCenter, &vXnormal );
	D3DXPlaneFromPointNormal( y, &vCenter, &vYnormal );
	D3DXPlaneFromPointNormal( z, &vCenter, &vZnormal );*/

	D3DXPlaneFromPoints(pXplane, &dxv3Center, &(dxv3Center + dxv3Ynormal), &(dxv3Center + dxv3Znormal));
	D3DXPlaneFromPoints(pYplane, &dxv3Center, &(dxv3Center + dxv3Xnormal), &(dxv3Center + dxv3Znormal));
	D3DXPlaneFromPoints(pZplane, &dxv3Center, &(dxv3Center + dxv3Xnormal), &(dxv3Center + dxv3Ynormal));
}

// 폴리곤을 포함하는 최소의 육면체를 만듭니다.
void CCube::SetCumulative(CPolygon* pPoly)
{
	m_dxv3PosLow.x = min(min(min(pPoly->m_dxv3Point[0].x, pPoly->m_dxv3Point[1].x), pPoly->m_dxv3Point[2].x), m_dxv3PosLow.x);
	m_dxv3PosLow.y = min(min(min(pPoly->m_dxv3Point[0].y, pPoly->m_dxv3Point[1].y), pPoly->m_dxv3Point[2].y), m_dxv3PosLow.y);
	m_dxv3PosLow.z = min(min(min(pPoly->m_dxv3Point[0].z, pPoly->m_dxv3Point[1].z), pPoly->m_dxv3Point[2].z), m_dxv3PosLow.z);

	m_dxv3PosHigh.x = max(max(max(pPoly->m_dxv3Point[0].x, pPoly->m_dxv3Point[1].x), pPoly->m_dxv3Point[2].x), m_dxv3PosHigh.x);
	m_dxv3PosHigh.y = max(max(max(pPoly->m_dxv3Point[0].y, pPoly->m_dxv3Point[1].y), pPoly->m_dxv3Point[2].y), m_dxv3PosHigh.y);
	m_dxv3PosHigh.z = max(max(max(pPoly->m_dxv3Point[0].z, pPoly->m_dxv3Point[1].z), pPoly->m_dxv3Point[2].z), m_dxv3PosHigh.z);
}

void CCube::SetCumulative(D3DXVECTOR3* pdxv3Vec)
{
	m_dxv3PosLow.x = min(m_dxv3PosLow.x, pdxv3Vec->x);
	m_dxv3PosLow.y = min(m_dxv3PosLow.y, pdxv3Vec->y);
	m_dxv3PosLow.z = min(m_dxv3PosLow.z, pdxv3Vec->z);

	m_dxv3PosHigh.x = max(m_dxv3PosHigh.x, pdxv3Vec->x);
	m_dxv3PosHigh.y = max(m_dxv3PosHigh.y, pdxv3Vec->y);
	m_dxv3PosHigh.z = max(m_dxv3PosHigh.z, pdxv3Vec->z);
}

void CCube::Set(CPolygon* pPoly)
{
	m_dxv3PosLow.x = min(min(pPoly->m_dxv3Point[0].x, pPoly->m_dxv3Point[1].x), pPoly->m_dxv3Point[2].x);
	m_dxv3PosLow.y = min(min(pPoly->m_dxv3Point[0].y, pPoly->m_dxv3Point[1].y), pPoly->m_dxv3Point[2].y);
	m_dxv3PosLow.z = min(min(pPoly->m_dxv3Point[0].z, pPoly->m_dxv3Point[1].z), pPoly->m_dxv3Point[2].z);

	m_dxv3PosHigh.x = max(max(pPoly->m_dxv3Point[0].x, pPoly->m_dxv3Point[1].x), pPoly->m_dxv3Point[2].x);
	m_dxv3PosHigh.y = max(max(pPoly->m_dxv3Point[0].y, pPoly->m_dxv3Point[1].y), pPoly->m_dxv3Point[2].y);
	m_dxv3PosHigh.z = max(max(pPoly->m_dxv3Point[0].z, pPoly->m_dxv3Point[1].z), pPoly->m_dxv3Point[2].z);
}

void CCube::Set(CAIMesh* pMesh)
{
	m_dxv3PosLow.x = min(min(pMesh->m_dxv3Pt[0].x, pMesh->m_dxv3Pt[1].x), pMesh->m_dxv3Pt[2].x);
	m_dxv3PosLow.y = min(min(pMesh->m_dxv3Pt[0].y, pMesh->m_dxv3Pt[1].y), pMesh->m_dxv3Pt[2].y);
	m_dxv3PosLow.z = min(min(pMesh->m_dxv3Pt[0].z, pMesh->m_dxv3Pt[1].z), pMesh->m_dxv3Pt[2].z);

	m_dxv3PosHigh.x = max(max(pMesh->m_dxv3Pt[0].x, pMesh->m_dxv3Pt[1].x), pMesh->m_dxv3Pt[2].x);
	m_dxv3PosHigh.y = max(max(pMesh->m_dxv3Pt[0].y, pMesh->m_dxv3Pt[1].y), pMesh->m_dxv3Pt[2].y);
	m_dxv3PosHigh.z = max(max(pMesh->m_dxv3Pt[0].z, pMesh->m_dxv3Pt[1].z), pMesh->m_dxv3Pt[2].z);
}

void CCube::GetPlaneLeftRight(D3DXPLANE* pdxplLeft, D3DXPLANE* pdxplRight)
{
	D3DXVECTOR3 dxv3NormalLeft(-1.f, 0.f, 0.f);
	D3DXVECTOR3 dxv3NormalRight(1.f, 0.f, 0.f);

	D3DXPlaneFromPointNormal(pdxplLeft, &m_dxv3PosLow, &dxv3NormalLeft);
	D3DXPlaneFromPointNormal(pdxplRight, &m_dxv3PosHigh, &dxv3NormalRight);
}

void CCube::GetPlaneUpDown(D3DXPLANE* pdxplUp, D3DXPLANE* pdxplDown)
{
	D3DXVECTOR3 dxv3NormalUp(0.f, 1.f, 0.f);
	D3DXVECTOR3 dxv3NormalDown(0.f, -1.f, 0.f);

	D3DXPlaneFromPointNormal(pdxplUp, &m_dxv3PosHigh, &dxv3NormalUp);
	D3DXPlaneFromPointNormal(pdxplDown, &m_dxv3PosLow, &dxv3NormalDown);
}

void CCube::GetPlaneFrontBack(D3DXPLANE* pdxplFront, D3DXPLANE* pdxplBack)
{
	D3DXVECTOR3 dxv3NormalFront(0.f, 0.f, -1.f);
	D3DXVECTOR3 dxv3NormalBack(0.f, 0.f, 1.f);

	D3DXPlaneFromPointNormal(pdxplFront, &m_dxv3PosLow, &dxv3NormalFront);
	D3DXPlaneFromPointNormal(pdxplBack, &m_dxv3PosHigh, &dxv3NormalBack);
}

void CCube::DrawScreen()
{
	D3DXVECTOR3 pt[8];

	for(INT32 i = 0; i < 8; i += 4)
	{
		pt[0+i].x = m_dxv3PosLow.x;
		pt[0+i].y = m_dxv3PosHigh.y;
		
		pt[1+i].x = m_dxv3PosHigh.x;
		pt[1+i].y = m_dxv3PosHigh.y;

		pt[2+i].x = m_dxv3PosLow.x;
		pt[2+i].y = m_dxv3PosLow.y;

		pt[3+i].x = m_dxv3PosHigh.x;
		pt[3+i].y = m_dxv3PosLow.y;
	}

	for(INT32 i = 0; i < 4; i++)
	{
		pt[i].z = m_dxv3PosLow.z;
	}
	for(INT32 i = 4; i < 8; i++)
	{
		pt[i].z = m_dxv3PosHigh.z;
	}

	D3DCOLOR col = D3DCOLOR(RGB(24, 66, 170));
	
	for(INT32 i = 0; i < 8; i += 4)
	{
		g_Draw3DLine(&pt[0+i], &pt[1+i], col);
		g_Draw3DLine(&pt[2+i], &pt[3+i], col);
		g_Draw3DLine(&pt[0+i], &pt[2+i], col);
		g_Draw3DLine(&pt[1+i], &pt[3+i], col);
	}
	for(INT32 i = 0; i < 4; i++)
	{
		g_Draw3DLine(&pt[i], &pt[i+4], col);
	}
	
}

void CCube::DrawPlane(D3DXPLANE* pdxplData)
{
	D3DXVECTOR3 dxv3Pt[8];

	for(INT32 i = 0; i < 8; i += 4)
	{
		dxv3Pt[0 + i].x = m_dxv3PosLow.x;
		dxv3Pt[0 + i].y = m_dxv3PosHigh.y;
		
		dxv3Pt[1 + i].x = m_dxv3PosHigh.x;
		dxv3Pt[1 + i].y = m_dxv3PosHigh.y;

		dxv3Pt[2 + i].x = m_dxv3PosLow.x;
		dxv3Pt[2 + i].y = m_dxv3PosLow.y;

		dxv3Pt[3 + i].x = m_dxv3PosHigh.x;
		dxv3Pt[3 + i].y = m_dxv3PosLow.y;
	}

	for(INT32 i = 0; i < 4; i++)
	{
		dxv3Pt[i].z = m_dxv3PosLow.z;
	}
	for(INT32 i = 4; i < 8; i++)
	{
		dxv3Pt[i].z = m_dxv3PosHigh.z;
	}

	D3DCOLOR col = D3DCOLOR(RGB(24, 66, 170));
	

	INT32 a = 0;
	D3DXVECTOR3 result[12];

	for(INT32 i = 0; i < 8; i += 4)
	{
		D3DXPlaneIntersectLine(&result[a++], pdxplData, &dxv3Pt[0+i], &dxv3Pt[1+i]);
		D3DXPlaneIntersectLine(&result[a++], pdxplData, &dxv3Pt[2+i], &dxv3Pt[3+i]);
		D3DXPlaneIntersectLine(&result[a++], pdxplData, &dxv3Pt[0+i], &dxv3Pt[2+i]);
		D3DXPlaneIntersectLine(&result[a++], pdxplData, &dxv3Pt[1+i], &dxv3Pt[3+i]);
	}
	for(INT32 i = 0; i < 4; i++)
	{
		D3DXPlaneIntersectLine(&result[a++], pdxplData, &dxv3Pt[i], &dxv3Pt[i+4]);
	}

	/*
	FILE* fp = fopen("plane.txt", "wt");
	for( int i = 0; i < a; i++)
		fprintf(fp, "[%d] (%f,%f,%f)\n", i, result[i].x, result[i].y, result[i].z);
	fclose(fp);*/

	/*
	for( int i = 0; i < 12; i++ )
		for( int j = 0; j < 12; j++ )
		{
			if ( result[i].x < 1000.f && result[i].x > -1000.f &&
				result[j].x < 1000.f && result[j].x > -1000.f &&
				result[i].y < 1000.f && result[i].y > -1000.f &&
				result[j].y < 1000.f && result[j].y > -1000.f &&
				result[i].z < 1000.f && result[i].z > -1000.f &&
				result[j].z < 1000.f && result[j].z > -1000.f &&
				result[i].x != 0.f && result[j].x != 0.f &&
				result[i].y != 0.f && result[j].y != 0.f &&
				result[i].z != 0.f && result[j].z != 0.f )
				Draw3DLine( result[i], result[j], col );
		}
	*/
}

void CCube::MakeRandomPos(D3DXVECTOR3* pdxv3Pos)
{
	pdxv3Pos->x = ((REAL32)(rand()%(INT32)(((m_dxv3PosHigh.x - m_dxv3PosLow.x)*1000))))/1000;
	pdxv3Pos->y = ((REAL32)(rand()%(INT32)(((m_dxv3PosHigh.y - m_dxv3PosLow.y)*1000))))/1000;
	pdxv3Pos->z = ((REAL32)(rand()%(INT32)(((m_dxv3PosHigh.z - m_dxv3PosLow.z)*1000))))/1000;
}

void CCube::Cut(INT32 i, INT32 j, INT32 k, CCube* pCube)
{
	D3DXVECTOR3 dxv3Half = (m_dxv3PosHigh - m_dxv3PosLow)/2;
	
	pCube->m_dxv3PosLow = m_dxv3PosLow;
	if (i == 0) pCube->m_dxv3PosLow.x += dxv3Half.x;
	if (j == 1) pCube->m_dxv3PosLow.y += dxv3Half.y;
	if (k == 0) pCube->m_dxv3PosLow.z += dxv3Half.z;

	pCube->m_dxv3PosHigh = pCube->m_dxv3PosLow;
	pCube->m_dxv3PosHigh += dxv3Half;
}

BOOL CCube::IsIn(D3DXVECTOR3* pdxv3Pos)
{
	if (m_dxv3PosLow.x <= pdxv3Pos->x && pdxv3Pos->x <= m_dxv3PosHigh.x && m_dxv3PosLow.y <= pdxv3Pos->y && pdxv3Pos->y <= m_dxv3PosHigh.y && m_dxv3PosLow.z <= pdxv3Pos->z && pdxv3Pos->z <= m_dxv3PosHigh.z)
	{
		return TRUE;
	}
	return FALSE;
}