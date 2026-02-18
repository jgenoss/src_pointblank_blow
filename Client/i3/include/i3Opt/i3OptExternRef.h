#if !defined( __I3_OPT_EXTERN_REF_H)
#define __I3_OPT_EXTERN_REF_H

#include "i3SceneOptimizer.h"

typedef bool (*EXTERN_RES_CALLBACK)( i3NamedElement * pObj, void * pUserData);

class I3_EXPORT_OPT i3OptExternRef : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptExternRef, i3SceneOptimizer);
protected:
	char				m_szBasePath[MAX_PATH] = { 0 };

	EXTERN_RES_CALLBACK	m_pUserProc = nullptr;
	void *				m_pUserData = nullptr;

	i3::vector<i3Texture*>	m_TexList;

	bool		m_bExternTexture = true;

public:
	i3OptExternRef(void);

	char *		getBasePath(void)							{ return m_szBasePath; }
	void		setBasePath( char * pszPath)				{ i3::safe_string_copy( m_szBasePath, pszPath, MAX_PATH); }

	void		setUserProc( EXTERN_RES_CALLBACK pProc, void * pData)
	{
		m_pUserProc = pProc;
		m_pUserData = pData;
	}

	bool		getExternTextureEnable(void)				{ return m_bExternTexture; }
	void		setExternTextureEnable( bool bState)		{ m_bExternTexture = bState; }

	virtual void Trace( i3Node * pRoot) override;
	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
