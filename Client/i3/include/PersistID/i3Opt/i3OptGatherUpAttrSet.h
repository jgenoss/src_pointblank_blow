#if !defined( __I3_OPT_GATHERUP_ATTRSET_H)
#define __I3_OPT_GATHERUP_ATTRSET_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGatherUpAttrSet : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptGatherUpAttrSet);

protected:
	i3List			m_GatherUpAttrList;
	i3List			m_AttrSetList;

	i3List			m_ExclusionAttrList;

protected:
	//i3AttrSet *		_BuildGatherUp( i3List * pAttrSetList, i3List * pWorkingList);
	void			_BuildGatherUp( i3Node * pRoot);
	void			_BuildGatherUpAttrList( i3Node * pRoot, i3List * pOutList, BOOL bRecursive = TRUE);
	
	void			_SortGatherUpListByShareCnt( BOOL bAscending = TRUE);
	BOOL			_IsContact( i3Node * pForefatherNode, i3Node * pOffspringNode);
	BOOL			_IsCommonContact( i3Node * pCheckNode, i3List * pNodeList);
	i3Node *		_GetCommonParent( i3List * pNodeList);
	BOOL			_InsertToBetweenNode( i3Node * pDst, i3Node * pSrc);

public:
	i3OptGatherUpAttrSet( void);
	virtual ~i3OptGatherUpAttrSet( void);

	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
	virtual void	SetProperty( char * pszFieldName, char * pszValue);

	void			Destroy( void);

};

#endif //__I3_OPT_GATHERUP_ATTRSET_H

