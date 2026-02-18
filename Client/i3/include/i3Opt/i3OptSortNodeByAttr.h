#if !defined( I3OPT_SORTNODE_BYATTR_H__)
#define I3OPT_SORTNODE_BYATTR_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptSortNodeByAttr : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptSortNodeByAttr, i3SceneOptimizer);

protected:
	i3Node	*	m_pNewRoot = nullptr;

	i3::vector<struct ALL_ATTRS_AT_GEOMETRY*>	m_AttrInfoList;
	i3::vector<struct ATTR_USED_INFO*>			m_AttrUseInfoList;

public:
	i3OptSortNodeByAttr(void);
	virtual ~i3OptSortNodeByAttr(void);

protected:
	void	_AddUsingState( i3RenderAttr * pAttr);

	INT32	_GetMaxUsedItem( i3Node * pNode);

	INT32	_HasSameAttrUse( i3RenderAttr * pAttr);
	INT32	_HasSameAttr( i3RenderAttr * pTargetAttr, i3AttrSet * pAttrSet);
	void	_GatherAttrList( i3Node * pNode, i3AttrSet * pCurAttr);

	bool	_Rec_SortNode( i3Node * pRoot);
	bool	_SortNodeByAttrUseCount( i3Node * pNewRoot);

	bool	_DivideChildByAttr( i3AttrSet * Parent, i3AttrSet * pExecptParent, i3RenderAttr * pAttr, i3Node * pPrevParent);

public:
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;
};

#endif