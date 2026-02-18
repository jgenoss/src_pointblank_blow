//*************************************************************************************
//	class name : i3OptCollapseNode
//	불필요한 노드들을 검색해서 자동으로 제거한다.
//	by KOMET 2006.02.22
//*************************************************************************************
#if !defined( __I3_OPT_COLLAPSE_NODE_H)
#define __I3_OPT_COLLAPSE_NODE_H

#include "i3OptPCH.h"
#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptCollapseNode : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptCollapseNode, i3SceneOptimizer);

public:
	i3OptCollapseNode(void);

	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;

	void	CollapseNode( i3Node * pNode);

protected:	

	bool	_RemoveUselessGeometry( i3Node * pNode, INT32 nChildIdx);
	bool	_RemoveUselessAttrSet( i3Node * pNode, INT32 nChildIdx);
	bool	_RemoveUselessI3Node( i3Node * pNode, INT32 nChildIdx);

	bool	_IsUselessGeometry( i3Geometry * pGeometry);
	bool	_IsUselessAttrSet( i3AttrSet * pAttrSet);
	bool	_IsUselessI3Node( i3Node * pI3Node);
};

#endif // __I3_OPT_COLLAPSE_NODE_H


