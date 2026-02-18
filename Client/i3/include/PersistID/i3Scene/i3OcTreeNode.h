#if !defined( __I3SCENE_OCTREENODE_H_)
#define __I3SCENE_OCTREENODE_H_

#include "i3Node.h"
#include "i3Geometry.h"

//GeometryAttr단위로 공간분할을 합니다.
//현재 TriangleStrip, NoIndexedArray를 기준으로 합니다.
class I3_EXPORT_SCENE i3OcTreeNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3OcTreeNode);

public:
	enum DIVIDE_NODE
	{
		TOP_LEFT_FRONT = 0,		// 0
		TOP_LEFT_BACK,			// 1
		TOP_RIGHT_BACK,			// etc...
		TOP_RIGHT_FRONT,
		BOTTOM_LEFT_FRONT,
		BOTTOM_LEFT_BACK,
		BOTTOM_RIGHT_BACK,
		BOTTOM_RIGHT_FRONT,
		LEAF_CURRENT,
	};

protected:
	bool					m_bLoose;			//하위노드들중 일반적인 크기로는 걸치는 경우가 발생하여 확장되었는지의 여부
	bool					m_bSubDivided;		//하위노드들로 분할되었는지의 여부

	REAL32					m_rSize;			//현 노드의 크기
	VEC3D					m_vCenter;			//현 노드의 중앙점
	UINT32					m_nDepth;
	
	i3OcTreeNode		*	m_pChildNode[8];	//분할된 자식 노드들

public:
	i3OcTreeNode(void);
	virtual			~i3OcTreeNode(void);

	void			InitTree(void);
	void			ReleaseTree(void);

	void			setSize( REAL32 rSize)		{	m_rSize = rSize;}
	REAL32			getSize( void)				{	return m_rSize;}

	void			setCenter( VEC3D * pCenter)	{	i3Vector::Copy( &m_vCenter, pCenter);}
	VEC3D*			getCenter( void)			{	return &m_vCenter;}

	void			setDepth( UINT32	nDepth)	{	m_nDepth = nDepth;}
	UINT32			getDepth( void)				{	return m_nDepth;}

	bool			isSubDivided(void)				{	return m_bSubDivided;}
	void			setSubdivided( bool bDivided)	{	m_bSubDivided = bDivided;}

	i3OcTreeNode*	getChildTreeNode( DIVIDE_NODE id)	{	return m_pChildNode[id];}
	void			SetChildTreeNode( DIVIDE_NODE id, i3OcTreeNode * pChild)
	{
		I3_REF_CHANGE( m_pChildNode[id], pChild);
	}

	void			setLoose( bool bLoose)			{	m_bLoose = bLoose;}
	bool			getLoosed( void)				{	return m_bLoose;}

//Construct Tree Functions
protected:
	void			_GetNewCenter( VEC3D * vOutCenter, UINT32 id);
	void			_CalcDimensions();
	void			_CreateTree( void);
	bool			_CreateNewTree( i3OcTreeNode * pNew, i3OcTreeNode::DIVIDE_NODE ID, bool bLoose);

	UINT32			_DivideByAAPlane( VEC3D * pTri, VEC3D	*pvCenter);
	UINT32			_DivideByAAPlaneExt( VEC3D * pTri, VEC3D *pvCenter, VEC3D *pBoxCenter);

public:
	void			BuildOcTree( INT32 nDepth, REAL32 rSize, VEC3D * vCenter = NULL);
	void			AssignToLeaf();	//더이상 계산할 필요가 없는노드로 임명합니다...

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3SCENE_OCTREE_H_