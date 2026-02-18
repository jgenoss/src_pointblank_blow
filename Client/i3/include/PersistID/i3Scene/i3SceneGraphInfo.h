#if !defined( __I3_SCENEGRAPH_INFO_H)
#define __I3_SCENEGRAPH_INFO_H

#include "i3Node.h"

class I3_EXPORT_SCENE i3SceneGraphInfo : public i3ResourceObject
{
	I3_CLASS_DEFINE( i3SceneGraphInfo);
protected:
	i3Node *		m_pCommonSg;
	i3Node *		m_pInstanceSg;

public:
	i3SceneGraphInfo(void);
	virtual ~i3SceneGraphInfo(void);

	i3Node *		getCommonSg(void)				{ return m_pCommonSg; }
	void			setCommonSg( i3Node * pNode)	{ I3_REF_CHANGE( m_pCommonSg, pNode); }

	i3Node *		getInstanceSg(void)				{ return m_pInstanceSg; }
	void			setInstanceSg( i3Node * pNode)	{ I3_REF_CHANGE( m_pInstanceSg, pNode); }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
