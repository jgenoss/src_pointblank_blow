#if !defined( __I3UIL_TREE_NODE_TEMPLATE_H__)
#define __I3UIL_TREE_NODE_TEMPLATE_H__

//#include "i3UITemplate.h"
class i3UITemplate;

class I3_EXPORT_FRAMEWORK i3UILTreeNode_Template : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3UILTreeNode_Template, i3TreeNode);

protected:
	i3UITemplate*	m_pTemplate = nullptr;

public:
	virtual ~i3UILTreeNode_Template(void);

	void			SetTemplate( i3UITemplate* pTemplate);
	i3UITemplate*	getTemplate( void)		{	return m_pTemplate;	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	SetName( const i3::rc_string& strName ) override;
};

#endif	//__I3UIL_TREE_NODE_TEMPLATE_H__