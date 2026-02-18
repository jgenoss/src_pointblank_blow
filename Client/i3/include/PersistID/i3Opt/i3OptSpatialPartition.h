#if !defined( I3OPT_SPATIAL_PARTITION_H)
#define I3OPT_SPATIAL_PARTITION_H

#include "i3SceneOptimizer.h"

typedef struct _tagi3OptVertexGroup
{
	i3VertexFormat m_Format;
	i3VertexArray * m_pVA;
	INT32			m_idxCur;
	INT32			m_MaxCount;
} I3OPT_VERTEX_GROUP;

class I3_EXPORT_OPT i3OptSpatialPartition : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSpatialPartition);
protected:
	VEC3D		m_Size;
	INT32		m_LimitTriCount;
	bool		m_bSplit;

protected:
	INT32					_GetChildGeometry( i3Node * pParent, i3List * pList = NULL);
	void					_MergeGeometry( i3Node * pParent);
	void					_SplitGeo( i3Node * pParent);
	i3Node *				_Rec_SplitGeo( i3Geometry * pGeo, i3Node * pParent);
	I3OPT_VERTEX_GROUP *	_GetVertexGroup( i3List * pGroupList, i3VertexFormat * pFormat);

public:
	i3OptSpatialPartition(void);
	virtual ~i3OptSpatialPartition(void);

	VEC3D *			getLimitSize(void)						{ return &m_Size; }
	void			setLimitSize( VEC3D * pSize)			{ i3Vector::Copy( &m_Size, pSize); }

	INT32			getLimitTriangleCount(void)				{ return m_LimitTriCount; }
	void			setLimitTriangleCount( INT32 cnt)		{ m_LimitTriCount = cnt; }

	bool			getSplitEnable(void)					{ return m_bSplit; }
	void			setSplitEnable( bool bFlag)				{ m_bSplit = bFlag; }

public:
	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
};

#endif