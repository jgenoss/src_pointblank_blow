#if !defined( __I3_OPT_GATHERUP_ATTRIBUTE_H)
#define __I3_OPT_GATHERUP_ATTRIBUTE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGatherUpAttribute : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptGatherUpAttribute);
protected:
	i3ClassMeta *		m_pMeta;

protected:
	i3List				m_AttrInfoList;
	i3List				m_TxList;

protected:
	void				_RemoveAllAttrInfo(void);
	INT32				_FindAttrInfoByAttr( i3RenderAttr * pAttr);
	i3Node *			_FindPivotNode( i3Node * pNode, i3RenderAttr * pAttr);
	void				_FindAttrGroupByTx( i3Node * pNode, i3List * pList);
	void				_FindAttrGroupByValueAndPivot( i3RenderAttr * pAttr, i3Node * pPivotNode, i3List * pList);
	bool				_IsParentOfDifferAttr( i3Node * pNode, i3RenderAttr * pAttr);

public:
	i3OptGatherUpAttribute(void);
	virtual ~i3OptGatherUpAttribute(void);

	i3ClassMeta *		getMeta(void)					{ return m_pMeta; }
	void				setMeta( i3ClassMeta * pMeta)	{ m_pMeta = pMeta; }

	virtual BOOL	OnNode( i3Node * pNode);

	virtual void	Trace( i3Node * pRoot);
};

#endif
