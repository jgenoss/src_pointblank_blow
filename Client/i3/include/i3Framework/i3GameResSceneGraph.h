#if !defined( __I3_GAME_RES_SCENEGRAPH_H)
#define __I3_GAME_RES_SCENEGRAPH_H

#include "i3GameRes.h"

class i3GameObjBase;

class I3_EXPORT_FRAMEWORK i3GameResSceneGraph : public i3GameRes
{
	I3_EXPORT_CLASS_DEFINE( i3GameResSceneGraph, i3GameRes);
public:

private:
	INT32		m_completionCount = 0;			// Use Internal only

protected:
	// Persistent Members
	INT32		m_Layer = -1;

	i3SceneGraphInfo *		m_pSgInfo = nullptr;

	// Volatile Memebers
	i3Node *	m_pBodyRoot = nullptr;

	bool		m_bBindedCommonSG = false;			// common scenegraph°¡ bind‰ç³ª?

public:
	i3GameResSceneGraph(void);
	virtual ~i3GameResSceneGraph(void);

	virtual void	Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing = true, bool bClone = true);

	void		setCompletionCount( INT32 c)		{ m_completionCount = c; }
	INT32		decCompletionCount(void)
	{
		m_completionCount--;
		return m_completionCount;
	}

	INT32		getLayer(void)						{ return m_Layer; }
	void		setLayer( INT32 type)				{ m_Layer = type; }

	i3Node *	getCommonScene(void)				{ return m_pSgInfo->getCommonSg(); }
	void		SetCommonScene( i3Node * pNode);

	i3Node *	getScene(void) const { I3ASSERT_RETURN(m_pSgInfo, nullptr);	return m_pSgInfo->getInstanceSg(); }
	void		SetScene( i3Node * pNode);

	i3SceneGraphInfo *	getSceneGraphInfo(void) const { return m_pSgInfo; }
	void				setSceneGraphInfo( i3SceneGraphInfo * pSgInfo)
	{
		I3_REF_CHANGE( m_pSgInfo, pSgInfo);
	}

	i3Node *	getAttachRoot(void)					{ return m_pBodyRoot; }

	virtual void	Destroy( void) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual UINT32	SaveToFile( i3Stream * pStream) override;
	virtual UINT32	LoadFromFile( i3Stream * pStream, INT32 nVersion) override;
	virtual UINT32	LoadFromFile(i3IniParser * pParser, INT32 nVersion, INT32 nType) override;
};

#endif
