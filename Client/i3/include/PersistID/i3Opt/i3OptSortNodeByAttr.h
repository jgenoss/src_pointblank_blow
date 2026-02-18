#if !defined( I3OPT_SORTNODE_BYATTR_H__)
#define I3OPT_SORTNODE_BYATTR_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptSortNodeByAttr : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSortNodeByAttr);

protected:
	i3Node	*	m_pNewRoot;

	i3List	m_AttrInfoList;
	i3List	m_AttrUseInfoList;

public:
	i3OptSortNodeByAttr(void);
	virtual ~i3OptSortNodeByAttr(void);

protected:
	void	_AddUsingState( i3RenderAttr * pAttr);

	INT32	_GetMaxUsedItem( i3Node * pNode);

	INT32	_HasSameAttrUse( i3RenderAttr * pAttr);
	INT32	_HasSameAttr( i3RenderAttr * pTargetAttr, i3AttrSet * pAttrSet);
	void	_GatherAttrList( i3Node * pNode, i3AttrSet * pCurAttr);

	BOOL	_Rec_SortNode( i3Node * pRoot);
	BOOL	_SortNodeByAttrUseCount( i3Node * pNewRoot);

	BOOL	_DivideChildByAttr( i3AttrSet * Parent, i3AttrSet * pExecptParent, i3RenderAttr * pAttr, i3Node * pPrevParent);

public:
	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
};

#endif