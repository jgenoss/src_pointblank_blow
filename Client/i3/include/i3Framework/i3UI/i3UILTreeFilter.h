#if !defined( _I3UI_L_TREEFILTER__)
#define _I3UI_L_TREEFILTER__

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CUITreeFilter : Filter Class
//	분류 기능을 제공하기 위한 트리 노드 클래스
//	멤버로, 컨트롤 클래스로 부터 파생된 컨트롤들의 리스트가 위치하게 됨.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "i3UILTreeElement.h"

class I3_EXPORT_FRAMEWORK i3UILTreeFilter : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3UILTreeFilter, i3TreeNode);
	
public:
	void	AddFilter( i3UILTreeFilter * pFilter);
	void	RemoveFilter( i3UILTreeFilter * pFilter);

	//
	void	AddTemplate( i3UITemplate * pTemplate);
	//void	RemoveControl( i3UITemplate * pTemplate);

	i3UILTreeElement * FindElementByMeta( i3ClassMeta * pMeta);

public:
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif