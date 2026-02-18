#pragma once

#include "Common.h"

class PATHNODE
{
	D3DXVECTOR3	m_dxv3PosBegin;		// 시작점(제일 첫노드일땐 삼각형의 내부중에서, 그 외의 노드일땐 삼각형의 변을 가리킵니다.)
	D3DXVECTOR3 m_dxv3Normal;		// 목적지방향의 노멀
	REAL32		m_r32Length;		// 시작점에서 목적지까지의 길이

	INT8		m_i8Set;

public:
	PATHNODE() { m_i8Set = 0; }

	void SetPosBegin(D3DXVECTOR3* pdxv3PosBegin)
	{
		m_i8Set = 1;
		memcpy(&m_dxv3PosBegin, pdxv3PosBegin, sizeof(D3DXVECTOR3));
	}

	void GetPosBegin(D3DXVECTOR3* pdxv3PosBegin)
	{
		if (m_i8Set == 1 || m_i8Set == 2)
		{
			memcpy(pdxv3PosBegin, &m_dxv3PosBegin, sizeof(D3DXVECTOR3));
		}
		else assert(FALSE);
	}

	void GetNormal(D3DXVECTOR3* pdxv3Normal)
	{
		if (m_i8Set == 2)
		{
			memcpy(pdxv3Normal, &m_dxv3Normal, sizeof(D3DXVECTOR3));
		}
		else assert(FALSE);
	}

	REAL32 GetLength()
	{
		if (m_i8Set == 2)
		{
			return m_r32Length;
		}
		assert(FALSE);
	}

	void BuildLengthNormal(D3DXVECTOR3 dxv3Goal)
	{
		if (m_i8Set == 1 || m_i8Set == 2)
		{
			m_i8Set = 2;
			D3DXVECTOR3 dxv3Chai;
			dxv3Chai = dxv3Goal - m_dxv3PosBegin;
			m_r32Length = D3DXVec3Length(&dxv3Chai);
			D3DXVec3Normalize(&m_dxv3Normal, &dxv3Chai);
		}
		else assert(FALSE);
	}
	void GetGoal(D3DXVECTOR3* pdxv3Goal)
	{
		if (m_i8Set == 2)
		{
			D3DXVECTOR3 dxv3Ret;
			dxv3Ret = m_dxv3PosBegin + m_dxv3Normal*m_r32Length;
			memcpy(pdxv3Goal, &dxv3Ret, sizeof(D3DXVECTOR3));
		}
		else assert(FALSE);
	}
};

class PATHNODE_PULLED
{
	D3DXVECTOR2 m_dxv2PosBegin;
	D3DXVECTOR2	m_dxv2Normal;
	REAL32		m_r32Length;
	REAL32		m_r32AvgY;

	INT8		m_i8Set;

public:
	PATHNODE_PULLED() { m_i8Set = 0; }

	void SetPosBegin(D3DXVECTOR2 dxv2Pos)
	{
		m_i8Set = 1;
		memcpy(&m_dxv2PosBegin, &dxv2Pos, sizeof(D3DXVECTOR2));
	}

	void GetPosBegin(D3DXVECTOR2* pdxv2PosBegin)
	{
		if (m_i8Set == 1 || m_i8Set == 2)
		{
			memcpy(pdxv2PosBegin, &m_dxv2PosBegin, sizeof(D3DXVECTOR2));
		}
		else assert(FALSE);
	}

	void BuildLengthNormal(D3DXVECTOR2 dxv2Goal)
	{
		if (m_i8Set == 1 || m_i8Set == 2)
		{
			m_i8Set = 2;
			D3DXVECTOR2 dxv2Chai;
			dxv2Chai = dxv2Goal - m_dxv2PosBegin;
			m_r32Length = D3DXVec2Length(&dxv2Chai);
			D3DXVec2Normalize(&m_dxv2Normal, &dxv2Chai);
		}
		else assert(FALSE);
	}
	void BuildLengthNormal(D3DXVECTOR3 dxv3Goal)
	{
		if (m_i8Set == 1 || m_i8Set == 2)
		{
			m_i8Set = 2;
			D3DXVECTOR2 dxv2Chai;
			dxv2Chai.x = dxv3Goal.x - m_dxv2PosBegin.x;
			dxv2Chai.y = dxv3Goal.z - m_dxv2PosBegin.y;
			m_r32Length = D3DXVec2Length(&dxv2Chai);
			D3DXVec2Normalize(&m_dxv2Normal, &dxv2Chai);
		}
		else assert(FALSE);
	}
	void GetGoal(D3DXVECTOR2* pdxv2Goal)
	{
		if (m_i8Set == 2)
		{
			D3DXVECTOR2 dxv2Ret;
			dxv2Ret = m_dxv2PosBegin + m_dxv2Normal*m_r32Length;
			memcpy(pdxv2Goal, &dxv2Ret, sizeof(D3DXVECTOR2));
		}
		else assert(FALSE);
	}
};

// 한개의 AI마다 각각 가지고 있는 AI패스 정보(=길찾은 결과)
class CAIPathData
{
public:
	// 실시간 변경 값
	INT32		m_i32IdxCurrent;	// 현재가고 있는 번호. 이 값이 m_i16Cnt - 1과 같은 값이 되면 목적지에 도착한 것입니다.
	REAL32		m_r32LengthMoved;	// 노드시작점에서 0.f으로 셋팅후 이동시마다 이동거리가 더해집니다. 더한 결과값이 m_r32P1Length를 넘어설경우 다음 노드로 넘어간 것으로 처리합니다. 다시 0.f로 변경

	// 패스파인딩으로 채워지는 값. 이후 새로 패스파인딩을 하기전까지 변경되지 않습니다.
	UINT16		m_ui16Idx[MAX_WAYPATH_COUNT];
	DS_LINE		m_lnInner[MAX_WAYPATH_COUNT];

	INT32		m_i32PathNode;
	PATHNODE	m_PathNode[MAX_WAYPATH_COUNT]; // [0]단순중점연결 [1]끈당기기 [2]세밀한끈당기기
	INT32		m_i32PathNodePulled;
	PATHNODE_PULLED	m_PathNodePulled[MAX_WAYPATH_COUNT];
	D3DXVECTOR3	m_dxv3Memory;

	VOID		Clear();
	VOID		InsertFirstNode(D3DXVECTOR3 dxv3Begin, D3DXVECTOR3 dxv3Goal, UINT16 ui16NMIdx, DS_LINE* plnData);
	BOOL		InsertNode(D3DXVECTOR3 dxv3Goal, UINT16 ui16NMIdx, DS_LINE* plnData);
	VOID		InsertData(DS_LINE* plnData, UINT16 ui16NMIdx);
	BOOL		Contain(UINT16 ui16Idx);
	VOID		Pull(D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos);
	VOID		Pull1st(D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos);
	//BOOL		IntersectLine(INT32 i32Begin, INT32 i32End, D3DXVECTOR2* pdxv2Out);
	VOID		PullOld(D3DXVECTOR3 dxv3Begin, D3DXVECTOR3 dxv3End);
	BOOL		InsertsectAGJ(INT32 i32a, INT32 i32b, DS_LINE* plnData);
	BOOL		CheckVisible(D3DXVECTOR3* pdxv3Begin, D3DXVECTOR3* pdxv3End);
};
