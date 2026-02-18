#pragma once

#include <d3dx9.h>
#include "Polygon.h"
#include "AIMesh.h"

class CCube
{
	D3DXVECTOR3	m_dxv3PosLow;	// À°¸éÃ¼ ÁÂÃøÇÏ´Ü¾Õ ÁÂÇ¥
	D3DXVECTOR3	m_dxv3PosHigh;	// À°¸éÃ¼ ¿ìÃø»ó´ÜµÚ ÁÂÇ¥

public:
	CCube();
	void Init();
	void GetPlaneXYZAxis(D3DXPLANE* pdxplX, D3DXPLANE* pdxplY, D3DXPLANE* pdxplZ);

	// Æú¸®°ïÀ» Æ÷ÇÔÇÏ´Â ÃÖ¼ÒÀÇ À°¸éÃ¼¸¦ ¸¸µì´Ï´Ù.
	void SetCumulative(CPolygon* pPoly);
	void SetCumulative(D3DXVECTOR3* pdxv3Vec);
	void Set(CPolygon* pPoly);
	void Set(CAIMesh* pMesh);
	void GetPlaneLeftRight(D3DXPLANE* pdxplLeft, D3DXPLANE* pdxplRight);
	void GetPlaneUpDown(D3DXPLANE* pdxplUp, D3DXPLANE* pdxplDown);
	void GetPlaneFrontBack(D3DXPLANE* pdxplFront, D3DXPLANE* pdxplBack);
	void DrawScreen();
	void MakeRandomPos(D3DXVECTOR3* pdxv3Pos);
	void Cut(INT32 i, INT32 j, INT32 k, CCube* pCube);
	void DrawPlane(D3DXPLANE* pdxplData);
	BOOL IsIn(D3DXVECTOR3* pdxv3Pos);
};
