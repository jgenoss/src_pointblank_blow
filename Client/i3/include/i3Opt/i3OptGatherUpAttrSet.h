#if !defined( __I3_OPT_GATHERUP_ATTRSET_H)
#define __I3_OPT_GATHERUP_ATTRSET_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptGatherUpAttrSet : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptGatherUpAttrSet, i3SceneOptimizer);

protected:
	i3::vector<struct GATHER_ATTR*>			m_GatherUpAttrList;
	i3::vector<i3Node*>						m_AttrSetList;
	i3::vector<i3ClassMeta*>				m_ExclusionAttrList;

protected:
	//i3AttrSet *		_BuildGatherUp( List * pAttrSetList, List * pWorkingList);
	void			_BuildGatherUp( i3Node * pRoot);
	void			_BuildGatherUpAttrList( i3Node * pRoot, i3::vector<GATHER_ATTR*>& OutList, bool bRecursive = true);
	
	void			_SortGatherUpListByShareCnt( bool bAscending = true);
	bool			_IsContact( i3Node * pForefatherNode, i3Node * pOffspringNode);
	bool			_IsCommonContact( i3Node * pCheckNode, i3::vector<i3AttrSet*>& NodeList);
	i3Node *		_GetCommonParent(i3::vector<i3AttrSet*>& NodeList);
	bool			_InsertToBetweenNode( i3Node * pDst, i3Node * pSrc);

public:
	i3OptGatherUpAttrSet( void);

	virtual bool	OnNode(i3Node * pNode, i3Node * pParent) override { return true; }
	virtual void	Trace( i3Node * pRoot) override;
	virtual void	SetProperty( char * pszFieldName, char * pszValue) override;

	void			Destroy( void);

};

#endif //__I3_OPT_GATHERUP_ATTRSET_H

