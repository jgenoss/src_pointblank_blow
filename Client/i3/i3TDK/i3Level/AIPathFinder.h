#pragma once

#include "AIMesh.h"
#include "Common.h"
#include "AIPathData.h"

class CAIStage;

class CAIPathFinderNode
{
public:
	INT32			m_i32State;			// 0(none) 1(open) 2(close)
	REAL32			m_r32G;				// 부모 메시~현재 메시까지의 길이
	REAL32			m_r32H;				// 현재 메시~목적지 메시까지의 길이
	REAL32			m_r32F;				// m_fG + m_fH
	UINT16			m_ui16ParentMesh;	// 부모 메시 인덱스
	CAIMesh*		m_pParentMesh;		// 부모 메시 포인터
	CAIPathFinderNode*	m_pParentNode;	// 부모 노드
	INT32			m_i32Byun;
	CAIMesh*		m_pMyMesh;			// 자신 메시 포인터
	UINT16			m_ui16MyMesh;		// 자신 메시 인덱스

	BOOL			IsStateOpen()		{ return (m_i32State == STATE_ASTAR_ALGORITHM_OPEN); }
	BOOL			IsStateClose()		{ return (m_i32State == STATE_ASTAR_ALGORITHM_CLOSE); }
	VOID			SetStateOpen()		{ m_i32State = STATE_ASTAR_ALGORITHM_OPEN; }
	VOID			SetStateClose()		{ m_i32State = STATE_ASTAR_ALGORITHM_CLOSE; }
	VOID			SetG(REAL32 r32G)	{ m_r32G = r32G; }
	VOID			SetH(REAL32 r32H)	{ m_r32H = r32H; }
	REAL32			GetG()				{ return m_r32G; }
	VOID			AddGHToF()			{ m_r32F = m_r32G + m_r32F; }
};

class CAIPathFinder
{
	CAIStage*		m_pAIStage;
	INT32			m_i32BeginIdx;

public:
	VOID			ClearState();
	BOOL			BuildNode(INT32 i32Idx);
	INT32			FindMinFInOpenState();
	BOOL			FindPath(CAIPathData* pPathData, CAIStage* pStage, INT32 i32BeginIdx, INT32 i32GoalIdx, D3DXVECTOR3 dxv3BeginPos, D3DXVECTOR3 dxv3GoalPos);

	CAIPathFinderNode	m_arNode[MAX_NAVIMESH_COUNT];
};

extern CAIPathFinder g_AIPathFinder;