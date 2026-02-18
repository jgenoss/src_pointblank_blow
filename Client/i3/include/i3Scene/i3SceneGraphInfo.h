#if !defined( __I3_SCENEGRAPH_INFO_H)
#define __I3_SCENEGRAPH_INFO_H

#include "i3Node.h"

class I3_EXPORT_SCENE i3SceneGraphInfo : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE( i3SceneGraphInfo, i3ResourceObject);
protected:
	i3Node *		m_pCommonSg = nullptr;
	i3Node *		m_pInstanceSg = nullptr;
	bool			m_bExternTextureSkipped = false;
public:
	virtual ~i3SceneGraphInfo(void);

	i3Node *		getCommonSg(void)				{ return m_pCommonSg; }
	void			setCommonSg( i3Node * pNode)	{ I3_REF_CHANGE( m_pCommonSg, pNode); }

	i3Node *		getInstanceSg(void)	const		{ return m_pInstanceSg; }
	void			setInstanceSg( i3Node * pNode)	{ I3_REF_CHANGE( m_pInstanceSg, pNode); }
	
	void			SetExternTextureSkipped(bool bSkip) { m_bExternTextureSkipped = bSkip; }
	bool			IsExternTextureSkipped() const		{ return m_bExternTextureSkipped; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
