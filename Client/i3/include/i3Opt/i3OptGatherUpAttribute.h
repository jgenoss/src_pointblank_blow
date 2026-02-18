#if !defined( __I3_OPT_GATHERUP_ATTRIBUTE_H)
#define __I3_OPT_GATHERUP_ATTRIBUTE_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGatherUpAttribute : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptGatherUpAttribute, i3SceneOptimizer);
protected:
	i3ClassMeta *		m_pMeta = nullptr;

protected:
	i3::vector<struct I3OPT_ATTR_INFO*>	m_AttrInfoList;
	i3::vector<i3Transform*>			m_TxList;

protected:
	void				_RemoveAllAttrInfo(void);
	INT32				_FindAttrInfoByAttr( i3RenderAttr * pAttr);
	i3Node *			_FindPivotNode( i3Node * pNode, i3RenderAttr * pAttr);
	void				_FindAttrGroupByTx( i3Node * pNode, i3::vector<I3OPT_ATTR_INFO*>& List);
	void				_FindAttrGroupByValueAndPivot( i3RenderAttr * pAttr, i3Node * pPivotNode, i3::vector<I3OPT_ATTR_INFO*>& List);
	bool				_IsParentOfDifferAttr( i3Node * pNode, i3RenderAttr * pAttr);

public:
	i3OptGatherUpAttribute(void);

	i3ClassMeta *		getMeta(void)					{ return m_pMeta; }
	void				setMeta( i3ClassMeta * pMeta)	{ m_pMeta = pMeta; }

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;

	virtual void	Trace( i3Node * pRoot) override;
};

#endif
