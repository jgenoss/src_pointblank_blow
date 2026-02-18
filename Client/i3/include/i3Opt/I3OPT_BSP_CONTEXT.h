#pragma once

#define SPLIT_COND_CONVEX			0x00000001
#define	SPLIT_COND_DEPTH			0x00000002
#define	SPLIT_COND_POLYCOUNT		0x00000004

enum SPLIT_PLANE
{
	SPLIT_PLANE_XAXIS,
	SPLIT_PLANE_YAXIS,
	SPLIT_PLANE_ZAXIS
};

enum SPLIT_MODE
{
	SPLIT_MODE_HYPERPLANE,				// Geometry 중의 한 Polygon을 분할 Plane으로 선정하는 방식
	SPLIT_MODE_HALFPLANE,				// Geometry Bound Volume을 양분하는 Plane
};

struct I3OPT_BSP_CONTEXT
{
	INT32			m_Depth = 0;
	INT32			m_LimitDepth = 0;
	INT32			m_LimitCount = 0;
	UINT32			m_Condition = 0;
	SPLIT_PLANE		m_Plane = SPLIT_PLANE_XAXIS;
	SPLIT_MODE		m_Mode = SPLIT_MODE_HYPERPLANE;
	bool			m_bSplitPoly = false;

	INT32			m_NodeCount = 0;
};
