#include "stdafx.h"
#include "AIPathData.h"
#include <vector>
using namespace std;

void CAIPathData::Clear()
{
	m_i32IdxCurrent = 0;
	m_i32PathNode = 0;
}
/*
BOOL CAIPathData::InsertNode(D3DXVECTOR3* pdxv3Vec, UINT16 ui16Mesh, DS_LINE* plnInner)
{
	if (m_i16Cnt >= MAX_WAYPATH_COUNT) return FALSE;
	
	memcpy(&m_dxv3Pos[m_i16Cnt], pdxv3Vec, sizeof(D3DXVECTOR3));
	m_ui16dx[m_i16Cnt] = ui16Mesh;
	memcpy(&m_lineIn[m_i16Cnt], plnInner, sizeof(DS_LINE));
	
	m_i16Cnt++;
	if (m_i16Cnt == MAX_WAYPATH_COUNT) return FALSE;
	return TRUE;
}*/

VOID CAIPathData::InsertFirstNode(D3DXVECTOR3 dxv3Begin, D3DXVECTOR3 dxv3Goal, UINT16 ui16NMIdx, DS_LINE* plnData)
{
	m_PathNode[0].SetPosBegin(&dxv3Begin);
	m_PathNode[0].BuildLengthNormal(dxv3Goal);
	m_ui16Idx[0] = ui16NMIdx;
	memcpy(&m_lnInner[0], plnData, sizeof(DS_LINE));

	memcpy(&m_dxv3Memory, &dxv3Goal, sizeof(D3DXVECTOR3));
	m_i32PathNode = 1;
}

BOOL CAIPathData::InsertNode(D3DXVECTOR3 dxv3Goal, UINT16 ui16NMIdx, DS_LINE* plnData)
{
	if (m_i32PathNode >= MAX_WAYPATH_COUNT) return FALSE;

	m_PathNode[m_i32PathNode].SetPosBegin(&m_dxv3Memory);
	m_PathNode[m_i32PathNode].BuildLengthNormal(dxv3Goal);
	m_ui16Idx[m_i32PathNode] = ui16NMIdx;
	if (plnData == nullptr)
	{
		m_lnInner[m_i32PathNode].Clear();
	}
	else
	{
		memcpy(&m_lnInner[m_i32PathNode], plnData, sizeof(DS_LINE));
	}

	memcpy(&m_dxv3Memory, &dxv3Goal, sizeof(D3DXVECTOR3));	// 목표지점은 다음노드의 시작점으로 사용되기 때문에 저장해놓습니다.

	m_i32PathNode++;
	if (m_i32PathNode == MAX_WAYPATH_COUNT) return FALSE;
	return TRUE;
}

BOOL CAIPathData::Contain(UINT16 ui16Idx)
{
	for(INT32 i = 0; i < m_i32PathNode; i++)
	{
		if (ui16Idx == m_ui16Idx[i]) return TRUE;
	}
	return FALSE;
}

/*
BOOL CAIPathData::IntersectLine(INT32 i32Begin, INT32 i32End, D3DXVECTOR2* pdxv2Out)
{
	DS_LINE lnA;
	D3DXVECTOR2 dxv2Tmp;

	if (i32Begin == 0)
	{
		lnA.Set(m_PathNode[0].m_dxv3PosBegin, m_PathNode[i32End].m_dxv3PosEnd);
	}
	else
	{
		lnA.Set(m_PathNodePulled[i32Begin - 1].m_dxv2PosEnd, m_PathNode[i32End].m_dxv3PosEnd);
	}

	if (g_GetIntersectPoint(&m_lnInner[i32Begin], &lnA, pdxv2Out) == FALSE) return FALSE;

	for(INT32 i = i32Begin + 1; i < i32End; i++)
	{
		if (g_GetIntersectPoint(&m_lnInner[i], &lnA, &dxv2Tmp) == FALSE) return FALSE;
	}
	return TRUE;
}*/

vector<DS_LINE> g_vtlnDraw;
vector<D3DXVECTOR2> g_dxv2Pos;
/*
void CAIPathData::Pull1st(D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos)
{
	INT32 i = 0;
	INT32 j = 0;
	D3DXVECTOR2 dxv2Out;

	g_vtlnDraw.clear();
	g_dxv2Pos.clear();
	
	while(i < m_i32PathNode)
	{
		j = i + 1;
		while(1)
		{
			if (IntersectLine(i, j, &dxv2Out) == FALSE) break;
			j++;
		}
		// 패스 i번부터 j번까지의 선 연결엔 실패했고, i번부터 j-1번까지는 성공한 상태

		DS_LINE lnChai;
		if (i == 0)
		{
			lnChai.Set(dxv3BeginPos, m_PathNode[j - 1].m_dxv3PosEnd);
		}
		else
		{
			lnChai.Set(m_PathNode[i - 1].m_dxv3PosEnd, m_PathNode[j - 1].m_dxv3PosEnd);
		}
		g_vtlnDraw.push_back(lnChai);

		D3DXVECTOR2 dxv2Normal;
		lnChai.GetNormal(&dxv2Normal);
		
		for(INT32 k = i; k < j; k++)
		{
			assert(g_GetIntersectPoint(&lnChai, &m_lnInner[k], &dxv2Out));
			g_dxv2Pos.push_back(dxv2Out);
			if (k != 0) m_PathNodePulled[k].m_dxv2PosBegin = m_PathNodePulled[k - 1].m_dxv2PosEnd;
			else
			{
				m_PathNodePulled[0].m_dxv2PosBegin.x = dxv3BeginPos.x;
				m_PathNodePulled[0].m_dxv2PosBegin.y = dxv3BeginPos.z;
			}
			m_PathNodePulled[k].BuildLengthNormal(dxv2Out);
		}
		m_PathNodePulled[j - 1].GetGoal(&m_PathNodePulled[j].m_dxv2PosBegin);
		m_PathNodePulled[j].BuildLengthNormal(m_PathNode[j].m_dxv3POS);
		i = j;
	}
}*/
/*
void CAIPathData::Pull(D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos)
{
	D3DXVECTOR2 dxv2Pos;

	for(INT32 i = 0; i < m_i32PathNode; i++)
	{
		if (i == 0)	memcpy(&m_PathNodePulled[0].m_dxv3PosBegin, &dxv3BeginPos, sizeof(D3DXVECTOR3));
		else		memcpy(&m_PathNodePulled[i].m_dxv3PosBegin, &m_PathNodePulled[i - 1].m_dxv3PosEnd, sizeof(D3DXVECTOR3));

		for(INT32 i32EIdx = i + 1;; i32EIdx++)
		{
			if (IntersectLine(i, i32EIdx, &dxv2Pos) == FALSE) break;
		}

		m_PathNodePulled[i].m_dxv3PosEnd.x = dxv2Pos.x;
		m_PathNodePulled[i].m_dxv3PosEnd.z = dxv2Pos.y;
		D3DXVECTOR2 dxv2Chai;
		dxv2Chai.x = dxv2Pos.x - m_PathNodePulled[i].m_dxv3PosBegin.x;
		dxv2Chai.y = dxv2Pos.y - m_PathNodePulled[i].m_dxv3PosBegin.z;
		m_PathNodePulled[i].m_r32Length = D3DXVec2Length(&dxv2Chai);
		D3DXVec2Normalize(&dxv2Chai, &dxv2Chai);
		m_PathNodePulled[i].m_dxv3Normal.x = dxv2Chai.x;
		m_PathNodePulled[i].m_dxv3Normal.z = dxv2Chai.y;
	}
}*/

BOOL CAIPathData::CheckVisible(D3DXVECTOR3* pdxv3Begin, D3DXVECTOR3* pdxv3End)
{
	DS_LINE lnVisibleTest;
	lnVisibleTest.Set(pdxv3Begin, pdxv3End);
	return InsertsectAGJ(0, m_i32PathNode - 1, &lnVisibleTest);
}

BOOL CAIPathData::InsertsectAGJ(INT32 i32a, INT32 i32b, DS_LINE* plnData)
{
	for(INT32 i = i32a; i < i32b; i++)
	{
		if (g_GetIntersectPoint(&m_lnInner[i], plnData) == FALSE)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// 끈당기기 함수. 2012.03.16 안국정 2012.03.21 수정
VOID CAIPathData::PullOld(D3DXVECTOR3 dxv3Begin, D3DXVECTOR3 dxv3End)
{
	D3DXVECTOR2 dxv2pt;
	dxv2pt.x = dxv3Begin.x;
	dxv2pt.y = dxv3Begin.z;

	m_i32PathNodePulled = 0;
	INT32 i32SIdx = 0;

	while(1)
	{
		DS_LINE lnTmp;
		lnTmp.SetBegin(&dxv2pt);
		INT32 i32RemSIdx = i32SIdx;
		i32SIdx++;
		if (i32SIdx == m_i32PathNode) return; // tmp code

		m_PathNodePulled[m_i32PathNodePulled].SetPosBegin(dxv2pt);

		while(1)
		{
			D3DXVECTOR3 dxv3Tmp;
			m_PathNode[i32SIdx].GetGoal(&dxv3Tmp);
			lnTmp.SetEnd(&dxv3Tmp);

			if (InsertsectAGJ(i32RemSIdx, i32SIdx, &lnTmp) == FALSE) break;
			
			i32SIdx++;
			if (i32SIdx == m_i32PathNode)
			{
				m_PathNodePulled[m_i32PathNodePulled++].BuildLengthNormal(dxv3End);
				return;
			}
		}
		D3DXVECTOR3 dxv3Tmp2;
		m_PathNode[i32SIdx - 1].GetGoal(&dxv3Tmp2);
		m_PathNodePulled[m_i32PathNodePulled++].BuildLengthNormal(dxv3Tmp2);

		dxv2pt.x = dxv3Tmp2.x;
		dxv2pt.y = dxv3Tmp2.z;
	}
}

/*
BOOL CAIPathData::MoveOneStep(D3DXVECTOR3* pMyPos, D3DXVECTOR3* pTargetPos, REAL32 r32Speed)
{
	// 현재 위치
	D3DXVECTOR3 loc;
	memcpy(&loc, pMyPos, sizeof(D3DXVECTOR3));

	// 이동할 위치
	D3DXVECTOR3 goal_loc;

	if (m_i16Going == m_i16Cnt)	memcpy(&goal_loc, pTargetPos, sizeof(D3DXVECTOR3));
	else						memcpy(&goal_loc, &m_dxv3Pos[m_i16Going], sizeof(D3DXVECTOR3));

	D3DXVECTOR3	dxv3Out;
	REAL32		r32Length = D3DXVec3Length(&(goal_loc - loc));
	REAL32		r32dx;
	REAL32		r32dz;
	BOOL		ReturnValue = FALSE;

	// 유저의 좌표와 완전히 일치될때까지 쫓아가면 겹쳐버리므로 1.4m이내로 접근하면 완벽하게 찾아간걸로 인식하게 합니다.
	if (m_i16Going == m_i16Cnt && r32Length < 1.4f) return true;

	// 목적지까지의 남은 거리가 1프레임당 이동거리 보다 짧다면
	if (r32Speed > r32Length)
	{
		// 목적지로 이동하고
		memcpy(pMyPos, &goal_loc, sizeof(D3DXVECTOR3));

		r32dx = goal_loc.x - loc.x;
		r32dz = goal_loc.z - loc.z;

		if (m_i16Going == m_i16Cnt) ReturnValue = TRUE;

		// 다음 목적지를 향하게 합니다.
		else m_i16Going++;
	}
	else
	{
		// 이동거리만큼 좌표를 이동시킵니다.
		D3DXVec3Lerp(&dxv3Out, &loc, &goal_loc, r32Speed/r32Length);

		r32dx = dxv3Out.x - loc.x;
		r32dz = dxv3Out.z - loc.z;

		memcpy(pMyPos, &dxv3Out, sizeof(D3DXVECTOR3));
	}

	return ReturnValue;
}*/