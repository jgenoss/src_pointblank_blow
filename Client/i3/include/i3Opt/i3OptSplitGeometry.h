#if !defined( __I3OPT_SPLIT_GEOMETRY_H)
#define __I3OPT_SPLIT_GEOMETRY_H

#include "i3SceneOptimizer.h"

class i3OptTriSplitNode;

class I3_EXPORT_OPT i3OptSplitGeometry : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptSplitGeometry, i3SceneOptimizer);
protected:
	i3::vector<i3OptTriSplitNode*>	m_GroupList;

protected:
	void	_Split( i3Geometry * pSrcGeo, i3Node * pParentNode);
	void	_FindAndAdd_SharedTri( i3OptTriSplitNode * pGroup, i3VertexArray * pVA, i3IndexArray * pIA, INT32 idxStart, i3::vector<i3OptTriSplitNode*>& UsedList);

	INT32					getGroupCount(void)				{ return INT32(m_GroupList.size()); }
	i3OptTriSplitNode *		getGroup( INT32 idx)			{ return m_GroupList[idx]; }
	void					removeAllGroup(void);
	i3OptTriSplitNode *		addTri( i3VertexArray * pVA, i3IndexArray * pIA, INT32 idx0, INT32 idx1, INT32 idx2);

public:
	i3OptSplitGeometry(void);

	virtual void		Trace( i3Node * pRoot) override;
	virtual bool		isSupportProgress(void) override;

	virtual const char *	getDesc(void) override;
};

#endif
