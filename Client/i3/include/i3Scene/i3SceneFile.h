#if !defined( __I3_SCENE_FILE_H)
#define __I3_SCENE_FILE_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3SceneGraphInfo.h"

class I3_EXPORT_SCENE i3SceneFile : public i3ResourceFile
{
	I3_EXPORT_CLASS_DEFINE( i3SceneFile, i3ResourceFile);

protected:
	i3SceneGraphInfo *		m_pInfo = nullptr;
	bool					m_bExtTexSkipped = false;

	virtual bool			CheckExternNeedLoad(i3ClassMeta* RscClassMeta) const;
	virtual bool			IsCheckedExternNeedLoad() const { return true; }
public:
	virtual ~i3SceneFile(void);
	
	void						SetExternTextureSkip(bool bSkip) { m_bExtTexSkipped = bSkip; }

	i3SceneGraphInfo *			getSceneGraphInfo(void)		{ return m_pInfo; }
	void						setSceneGraphInfo( i3SceneGraphInfo * pInfo)
	{
		I3_REF_CHANGE( m_pInfo, pInfo);
	}

	void		SetSceneGraph( i3Node * pNode);
	i3Node *	GetSceneGraph(void)					
	{ 
		if( m_pInfo == nullptr)
			return nullptr;

		return m_pInfo->getInstanceSg(); 
	}

	void		SetCommonSceneGraph( i3Node * pNode);
	i3Node *	GetCommonSceneGraph(void)
	{
		if( m_pInfo == nullptr)
			return nullptr;

		return m_pInfo->getCommonSg();
	}

	virtual UINT32				Load( i3Stream * pStream, bool bCommonRes = false) override;
	virtual UINT32				Save( i3Stream * pStream) override;

	virtual UINT32				Load( const char * pszPath, bool bCommonRes = false) override;
	virtual UINT32				Save( const char * pszPath) override;
};

#endif
