
#pragma once

#include "CCollisionData.h"

#define MAX_SUBDIVIED 1

enum OCTREE_NODES
{
	OCTREE_NODES_BOTTOM_LEFT_FRONT,
	OCTREE_NODES_BOTTOM_RIGHT_FRONT,
	OCTREE_NODES_TOP_LEFT_FRONT,
	OCTREE_NODES_TOP_RIGHT_FRONT,
	OCTREE_NODES_BOTTOM_LEFT_BACK,
	OCTREE_NODES_BOTTOM_RIGHT_BACK,
	OCTREE_NODES_TOP_LEFT_BACK,
	OCTREE_NODES_TOP_RIGHT_BACK,

	OCTREE_NODES_MAX,
};


struct ListNode
{
	vector<tFaceList> pList;


};
struct OctreeBoundBox
{
	bool			m_bEmpty;
	RT_VEC3D		m_Min;
	RT_VEC3D		m_Max;
	VEC3D			m_Center;

	OctreeBoundBox(void)						{ Reset(); }

	RT_VEC3D *		GetMin(void)				{ return &m_Min; }
	RT_VEC3D *		GetMax(void)				{ return &m_Max; }

	void	Reset(void)
	{
		i3Vector::Set( &m_Min, __RT_0, __RT_0, __RT_0);
		i3Vector::Set( &m_Max, __RT_0, __RT_0, __RT_0);
		i3Vector::Set( &m_Center, 0.0f, 0.0f, 0.0f);

		m_bEmpty = TRUE;
	}

	void	SetMinMax( RT_VEC3D * pMin, RT_VEC3D * pMax)
	{
		i3Vector::Copy( &m_Min, pMin);
		i3Vector::Copy( &m_Max, pMax);

		i3Vector::Add( &m_Center, &m_Min, &m_Max);
		i3Vector::Scale( &m_Center, &m_Center, 0.5f);

		m_bEmpty = FALSE;
	}

	void	SetBoundBox( RT_VEC3D * pCenter, REAL32 rWitdh)
	{
	
		REAL32 width = rWitdh *0.5f;

		i3Vector::Set(&m_Max, pCenter->x + width,pCenter->y + width,pCenter->z + width);
		i3Vector::Set(&m_Min, pCenter->x - width,pCenter->y - width,pCenter->z - width);

//		I3TRACE("Bound Box MAX: %2f %2f %2f  \n",m_Max.x,m_Max.y,m_Max.z);
//		I3TRACE("Bound Box MIN: %2f %2f %2f  \n",m_Min.x,m_Min.y,m_Min.z);
	
		m_bEmpty = FALSE;
	}


	bool isInside( VEC3D * pPos)
	{
		if((pPos->x < m_Min.x - 0.1f) || (pPos->x > m_Max.x + 0.1f))
			return false;

		if((pPos->y < m_Min.y - 0.1f) || (pPos->y > m_Max.y + 0.1f))
			return false;

		if((pPos->z < m_Min.z - 0.1f) || (pPos->z > m_Max.z + 0.1f))
			return false;

		return true;
	}

	VEC3D *	getCenter(void)						{ return &m_Center; }
};




struct RaycastResult
{
	VEC3D m_vNormal;
	VEC3D m_vIntersect;
	REAL32 m_rScale;

	void	Reset(void)
	{
		i3Vector::Set( &m_vNormal, 0.0f, 0.0f, 0.0f);
		i3Vector::Set( &m_vIntersect, 0.0f, 0.0f, 0.0f);
		m_rScale =0.f;
	
	}
	
	void SetResult(VEC3D *pIntersect, VEC3D *pNormal, REAL32 rScale)
	{	
		i3Vector::Copy(&m_vIntersect,pIntersect);
		i3Vector::Copy(&m_vNormal,pNormal);
		m_rScale = rScale;
	}

	VEC3D *getNormal(){return &m_vNormal;}
	VEC3D *getIntersect(){return &m_vIntersect;}
	REAL32 getScale(){return m_rScale;}



};



class COctreeNode{

private:

	CCollisionData	*			m_pCollisionData;
	INT8						m_i8Depth;
	COctreeNode *				m_pChildNode[8];				// 자식 노드
	VEC3D						m_vCenter;
	REAL32						m_r32Width;
	bool						m_bSubDivided;
	INT32						m_i32TriangleCount;
	OctreeBoundBox				m_BoundBox;



	UINT16						m_ui16NodeIndex;
	char			*			m_pNodeName;


public:

	COctreeNode();
	~COctreeNode();

	
	CCollisionData*				GetWorld()							{return m_pCollisionData;}

	tObject*					GetObjData(INT32 Idx)				{return GetWorld()->getObjData(Idx); }

	void						FindPartitionPoint(CCollisionData * pData);

	void						CreateNode(CCollisionData * pData,INT32 i32TotalTriangleCount,VEC3D * pCenter, REAL32 rWidth,OCTREE_NODES NODE_ID,INT32 NodeIndex);

	void						CreateNewNode(CCollisionData * pData, vector<tFaceList> pList , INT32 triangleCount, OCTREE_NODES NODE_ID, VEC3D *pCenter, REAL32 r32Width,INT32 NodeIndex);
	
	void						CopyToData(CCollisionData * pOutData, CCollisionData *pInData, vector<tFaceList> pList);

	bool						ContainLineInNode(VEC3D * pA,VEC3D * pB,VEC3D * pC, VEC3D *pCenter,OctreeBoundBox *pBox, REAL32 rWidth,INT32 a[3],INT32 b);

	VEC3D*						GetNewNodeCenter(VEC3D *pPos, REAL32 width, OCTREE_NODES NODE_ID);
		
	void						CreateCollisionData()				{m_pCollisionData = new CCollisionData;}
	
	VEC3D*						GetCenterPos()						{return &m_vCenter;}
	
	REAL32						GetWidth()							{return m_r32Width;}
	
	INT8						GetDepth()							{return m_i8Depth;}

	UINT16						GetNodeIndex()						{return m_ui16NodeIndex;}

	UINT16						GetFaceCount()	;				

	OctreeBoundBox *			GetBoundBox()						{return	&m_BoundBox;}

	/** \brief 노드의 바운드 박스를 설정합니다.. */
	void						SetBoundBox();

	INT32						GetSceneTriangleCount();

	void						AssignTrianglesToNode(CCollisionData *pWorld, INT32 i32TriangleCount);

	COctreeNode *				GetNode( VEC3D * pVPoint );					

	bool						DoSphereCollision( VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut, CCollisionData *pObject);		
	
	bool						DoRaycast( VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut, CCollisionData *pObject);		

	bool						DoTestRaycast( VEC3D * pStart,VEC3D * pEnd, RaycastResult * pOut, CCollisionData *pObject);		


	
};
