#if !defined( _I3UI_L_TREEELEMENT_H__)
#define _I3UI_L_TREEELEMENT_H__

#include "i3UITemplate.h"

class I3_EXPORT_FRAMEWORK i3UILTreeElement : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3UILTreeElement, i3TreeNode);

protected:
	i3ClassMeta	*	m_pClassMeta = nullptr;
	i3::vector<i3UITemplate*> m_ElmList;

public:
	virtual ~i3UILTreeElement(void);

public:
	i3ClassMeta	*	getElementMeta(void)					{	return m_pClassMeta;}
	void			setElementMeta( i3ClassMeta * pMeta)	{	m_pClassMeta = pMeta;}

	INT32			getElementCount(void)					{	return (INT32)m_ElmList.size();}
	INT32			AddElement( i3UITemplate * pElm);
	i3UITemplate*	getElement( INT32 idx)					{	I3_BOUNDCHK(idx, (INT32)m_ElmList.size()); return m_ElmList[idx];}

	void			removeElement( i3UITemplate * pElm);

public:
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif