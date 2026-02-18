#if !defined( __I3_SCENE_FILE_H)
#define __I3_SCENE_FILE_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3SceneGraphInfo.h"

class I3_EXPORT_SCENE i3SceneFile : public i3ResourceFile
{
	I3_CLASS_DEFINE( i3SceneFile);

protected:
	i3SceneGraphInfo *		m_pInfo;

public:
	i3SceneFile(void);
	virtual ~i3SceneFile(void);

	i3SceneGraphInfo *			getSceneGraphInfo(void)		{ return m_pInfo; }
	void						setSceneGraphInfo( i3SceneGraphInfo * pInfo)
	{
		I3_REF_CHANGE( m_pInfo, pInfo);
	}

	void		SetSceneGraph( i3Node * pNode);
	i3Node *	GetSceneGraph(void)					
	{ 
		if( m_pInfo == NULL)
			return NULL;

		return m_pInfo->getInstanceSg(); 
	}

	void		SetCommonSceneGraph( i3Node * pNode);
	i3Node *	GetCommonSceneGraph(void)
	{
		if( m_pInfo == NULL)
			return NULL;

		return m_pInfo->getCommonSg();
	}

	virtual UINT32				Load( i3Stream * pStream);
	virtual UINT32				Save( i3Stream * pStream);

	virtual UINT32				Load( const char * pszPath);
	virtual UINT32				Save( const char * pszPath);
};

#endif
