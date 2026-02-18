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
	I3_CLASS_DEFINE( i3OptCollapseNode);

public:
	i3OptCollapseNode(void);
	virtual ~i3OptCollapseNode(void);

	virtual BOOL OnNode( i3Node * pNode); 

	void	CollapseNode( i3Node * pNode);

protected:	

	BOOL	_RemoveUselessGeometry( i3Node * pNode, INT32 nChildIdx);
	BOOL	_RemoveUselessAttrSet( i3Node * pNode, INT32 nChildIdx);
	BOOL	_RemoveUselessI3Node( i3Node * pNode, INT32 nChildIdx);

	BOOL	_IsUselessGeometry( i3Geometry * pGeometry);
	BOOL	_IsUselessAttrSet( i3AttrSet * pAttrSet);
	BOOL	_IsUselessI3Node( i3Node * pI3Node);
};

#endif // __I3_OPT_COLLAPSE_NODE_H


