#pragma once

#include "AIMesh.h"
#include "AIPathFinder.h"
#include <vector>
#include "Cube.h"
using namespace std;

typedef IDirect3DVertexBuffer9 VB9;

class CAIStage
{
private:
	INT32			PickByRay(D3DXVECTOR3* pdxv3RayPos, D3DXVECTOR3* pdxv3RayDir, D3DXVECTOR3* pdxv3Out);

public:
	CAIStage();
	void			Delete();
	void			Destroy();
	void			FindPath( D3DXVECTOR3* pPtBegin, D3DXVECTOR3* pPtEnd ); // 시작점과 끝점을 설정해 길을 찾습니다.
	BOOL			LoadASE(CFile* pFile);	// 배경팀에서 3DSMAX로 작업한 파일
	BOOL			SetNmData();			// niv 메쉬툴 데이터를 nm 데이터로 변경
	BOOL			LoadNM(CFile* pFile);	// NM기본 파일
	BOOL			LoadNMC(CFile* pFile);	// 클라이언트가 서버로부터 NM을 수신받은 파일
	void			LoadRay(CFile* pFile);	// 선Raycast데이터 파일
	void			LoadRayV2(CFile* pFile);
	void			Picking(CRect* pRect);
	BOOL			RebuildVB();
	void			Render();
	void			BuildRAY();
	void			DrawBeginEndPos();
	void			Save(CFile* pFile);
	void			BuildNearInfo();
	void			BuildAngle();
	void			BuildDistance();
	UINT8			SendRayV2(UINT32 ui32BeginIdx, UINT32 ui32EndIdx)
	{
		INT32 i32Idx = ui32EndIdx + ui32BeginIdx*m_vtMesh.size();
		if (i32Idx%2 == 0)
			return m_pui8Ray[i32Idx/2] & 0x0F;
		return m_pui8Ray[i32Idx/2] >> 4;
	}
	INT32			CheckVisibleTest(INT32 i32IdxBegin, INT32 i32IdxEnd);
	void			Thread(INT32 i32BeginIdx, FILE* pFile);
	UINT8*			m_pui8Result;

	UINT8*			m_pui8Ray;
	bool*			m_pbRay;
	vector<CAIMesh>	m_vtMesh;
	CAIPathFinder	m_PathFinder;			// 길찾기 클래스 인스턴스 배열
	//VB9*			m_pVB;
	vector<INT32>	m_vtIdxPicked;
	CString			m_cstrName;

	CAIPathData		m_Path;
	BOOL			m_bBuildNearInfo;			// 패스파인딩을 할때 이웃정보가 빌드되어 있어야합니다. 하지만 한번만 하면 되므로 빌드되어있는지 여부를 기록합니다. 편집으로 인해 NM데이터가 조금이라도 수정되었을 경우 FALSE로 셋팅됩니다.
	BOOL			m_bBuildAngle;
	BOOL			m_bBuildDistance;

	INT32			m_i32ClickIdx[2];		// 마우스로 찍은 두개의 삼각형 인덱스
	D3DXVECTOR3		m_dxv3ClickPos[2];		// 마우스로 찍은 두개의 삼각형 좌표정보. 시작메쉬의 좌표와 끝메쉬의 좌표.
};

class CAIStageArray
{
public:
					CAIStageArray()		{ m_pSelected = nullptr; }
	void			Empty()				
	{
		for(UINT32 i = 0; i < m_vtpChild.size(); i++)
		{
			m_vtpChild[i]->Destroy();
			delete m_vtpChild[i]; 
		}
		m_vtpChild.clear();
	}
	CAIStage*		GetSelected()		{ return m_pSelected; }
	BOOL			InsertASE(CFile* pFile);
	BOOL			Load(CFile* pFile);
	void			Save(CFile* pFile);
	void			Select(INT32 idx)	{ m_pSelected = m_vtpChild[idx]; }

	vector<CAIStage*> m_vtpChild;
	CAIStage*		m_pSelected;
};
