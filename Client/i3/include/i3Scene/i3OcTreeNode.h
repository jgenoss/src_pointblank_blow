#if !defined( __I3SCENE_OCTREENODE_H_)
#define __I3SCENE_OCTREENODE_H_

#include "i3Node.h"
#include "i3Geometry.h"

//GeometryAttr단위로 공간분할을 합니다.
//현재 TriangleStrip, NoIndexedArray를 기준으로 합니다.
enum PBSRC
{
	PBS_MIN = 0,
	PBS_MAX,
	PBS_CTR
};

#define	I3_OCTST_ROOT			0x00000001
#define I3_OCTST_LEAF			0x00000010

class I3_EXPORT_SCENE i3OcTreeNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3OcTreeNode, i3Node);

public:
	enum DIVIDE_NODE
	{
		BOTTOM_LEFT_FRONT = 0,		// 0
		BOTTOM_LEFT_BACK,			// 1
		BOTTOM_RIGHT_FRONT,			// etc...
		BOTTOM_RIGHT_BACK,
		TOP_LEFT_FRONT,
		TOP_LEFT_BACK,
		TOP_RIGHT_FRONT,
		TOP_RIGHT_BACK,
		LEAF_CURRENT,
	};

	enum FACE
	{
		FACE_RIGHT = 0,
		FACE_LEFT,
		FACE_TOP,
		FACE_BOTTOM,
		FACE_FRONT,
		FACE_BACK
	};

protected:
	UINT32					m_OctreeStyle = 0;
	VEC3D					m_Min;				//현 노드의 크기
	VEC3D					m_Max;
	VEC3D					m_Center;			//현 노드의 중앙점
	REAL32					m_K = 2.0f;
	UINT32					m_nDepth = 0;

	// Locale Code
	//
	// Ref : "Simple and Efficient Traversal Methods for Quadtrees and Octrees", Sarah F. Frisken, Ronald N. Perry
	//
	UINT32					m_LocaleCode = 0;
	
	i3OcTreeNode *			m_pNeighbor[6] = { nullptr };
	i3OcTreeNode		*	m_pChildNode[8] = { nullptr };	//분할된 자식 노드들

public:
	i3OcTreeNode(void);
	virtual			~i3OcTreeNode(void);

	UINT32			getOctreeStyle(void)							{	return m_OctreeStyle; }
	void			setOctreeStyle( UINT32 style)					{	m_OctreeStyle = style; }
	void			addOctreeStyle( UINT32 style)					{	m_OctreeStyle |= style; }
	void			removeOctreeStyle( UINT32 style)				{	m_OctreeStyle &= ~style; }
	bool			isOctreeStyle( UINT32 style)					{	return (m_OctreeStyle & style) != 0; }
	bool			isRootNode(void)								{	return isOctreeStyle( I3_OCTST_ROOT); }
	bool			isLeafNode(void)								{	return isOctreeStyle( I3_OCTST_LEAF); }

	void			setMinMax( VEC3D * pMin, VEC3D * pMax)			
	{	
		i3Vector::Copy( &m_Min, pMin);
		i3Vector::Copy( &m_Max, pMax);
	}
	VEC3D *			getMin( void)									{	return &m_Min; }
	VEC3D *			getMax( void)									{	return &m_Max; }

	void			setCenter( VEC3D * pCenter)						{	i3Vector::Copy( &m_Center, pCenter);}
	void			setCenter( REAL32 x, REAL32 y, REAL32 z)		{	i3Vector::Set( &m_Center, x, y, z); }
	VEC3D *			getCenter( void)								{	return &m_Center;}

	void			setDepth( UINT32	nDepth)						{	m_nDepth = (m_nDepth & 0xFFFF0000) | (nDepth & 0x0000FFFF);}
	UINT32			getDepth( void)									{	return (m_nDepth & 0x0000FFFF);}

	void			setMaxDepth( UINT32 depth)						{	m_nDepth = (m_nDepth & 0x0000FFFF) | ((depth & 0x0000FFFF) << 16); }
	UINT32			getMaxDepth(void)								{	return (m_nDepth >> 16) & 0x0000FFFF; }

	REAL32			getK(void)										{	return m_K; }
	void			setK( REAL32 k)									{	m_K = k; }

	UINT32			getLocaleCode(void)								{	return m_LocaleCode; }
	void			setLocaleCode( UINT32 code)						{	m_LocaleCode = code; }
	UINT32			calcLocaleCode( i3BoundBox * pBox);
	void			UpdateLocaleCode( i3BoundBox * pBox, bool bRecursive = true);
	i3OcTreeNode *	GetNodeByLocaleCode( UINT32 code);
	i3OcTreeNode *	FindNodeByLocaleCode( UINT32 code);

#if defined( I3_DEBUG)
	void			CheckLocaleCode( UINT32 code = 0, INT32 depth = 0);
#endif

	i3OcTreeNode*	getChildOctree( DIVIDE_NODE id)					{	return m_pChildNode[id];}
	void			SetChildOctree( DIVIDE_NODE id, i3OcTreeNode * pChild)
	{
		I3_REF_CHANGE( m_pChildNode[id], pChild);

		if( pChild != nullptr)
			pChild->SetParent( this);
	}

	i3OcTreeNode *	getNeighbor( FACE face)							{	I3_BOUNDCHK( face, 6); return m_pNeighbor[face]; }
	void			setNeighbor( FACE face, i3OcTreeNode * pNode)
	{
		I3_BOUNDCHK( face, 6);

		// Ref. Counting을 하지 않는다.
		// Child Node에 대한 RefCounting과 동시에 했을 때, 오히려 복잡성이 올라가 관리가 어렵게 되기 때문.
		m_pNeighbor[ face] = pNode;
	}

//Construct Tree Functions
protected:
	void			_GetNewCenter( VEC3D * vOutCenter, UINT32 id);
	void			_CalcDimensions();
	void			_CreateTree( void);
	bool			_CreateNewTree( i3OcTreeNode * pNew, i3OcTreeNode::DIVIDE_NODE ID, bool bLoose);

	UINT32			_DivideByAAPlane( VEC3D * pTri, VEC3D	*pvCenter);
	UINT32			_DivideByAAPlaneExt( VEC3D * pTri, VEC3D *pvCenter, VEC3D *pBoxCenter);

	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo) override;
	virtual void	CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled) override;

public:
	void			BuildOcTree( INT32 nDepth, REAL32 rSize, VEC3D * vCenter = nullptr);
	void			AssignToLeaf();	//더이상 계산할 필요가 없는노드로 임명합니다...

	void			getSubBound( INT32 idx, VEC3D * pMin, VEC3D * pMax);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif //__I3SCENE_OCTREE_H_