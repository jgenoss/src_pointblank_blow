#if !defined( I3OPT_SPATIAL_PARTITION_H)
#define I3OPT_SPATIAL_PARTITION_H

#include "i3SceneOptimizer.h"

struct I3OPT_VERTEX_GROUP
{
	i3VertexFormat m_Format;
	i3VertexArray * m_pVA = nullptr;
	INT32			m_idxCur = 0;
	INT32			m_MaxCount = 0;
} ;

class I3_EXPORT_OPT i3OptSpatialPartition : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptSpatialPartition, i3SceneOptimizer);
protected:
	VEC3D		m_Size = { 30.0f, 30.0f, 30.0f };
	INT32		m_LimitTriCount = 5000;
	bool		m_bSplit = true;
	bool		m_bCreateBoundBox = true;

protected:
	INT32					_GetChildGeometry( i3Node * pParent);
	INT32					_GetChildGeometry( i3Node * pParent, i3::vector<i3Geometry*>& List);
	void					_MergeGeometry( i3Node * pParent);
	void					_SplitGeo( i3Node * pParent);
	i3Node *				_Rec_SplitGeo( i3Geometry * pGeo, i3Node * pParent);
	I3OPT_VERTEX_GROUP *	_GetVertexGroup( i3::vector<I3OPT_VERTEX_GROUP*>& GroupList, i3VertexFormat * pFormat);

public:
	i3OptSpatialPartition(void);

	VEC3D *			getLimitSize(void)						{ return &m_Size; }
	void			setLimitSize( VEC3D * pSize)			{ i3Vector::Copy( &m_Size, pSize); }

	INT32			getLimitTriangleCount(void)				{ return m_LimitTriCount; }
	void			setLimitTriangleCount( INT32 cnt)		{ m_LimitTriCount = cnt; }

	bool			getSplitEnable(void)					{ return m_bSplit; }
	void			setSplitEnable( bool bFlag)				{ m_bSplit = bFlag; }

	bool			getCreateBoundBoxState(void)			{ return m_bCreateBoundBox; }
	void			setCreateBoundBoxState( bool bState)	{ m_bCreateBoundBox = bState; }

public:
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;

	virtual	bool	SaveToXML( i3XMLElement * pXML) override;
	virtual	bool	LoadFromXML( i3XMLElement * pXML) override;
};

#endif