#if !defined( __I3_OPT_EXTERN_REF_H)
#define __I3_OPT_EXTERN_REF_H

#include "i3SceneOptimizer.h"

typedef bool (*EXTERN_RES_CALLBACK)( i3NamedElement * pObj, void * pUserData);

class I3_EXPORT_OPT i3OptExternRef : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptExternRef);
protected:
	char				m_szBasePath[ MAX_PATH];

	EXTERN_RES_CALLBACK	m_pUserProc;
	void *				m_pUserData;

	i3List				m_TexList;

	bool		m_bExternTexture;

public:
	i3OptExternRef(void);
	virtual ~i3OptExternRef(void);

	char *		getBasePath(void)							{ return m_szBasePath; }
	void		setBasePath( char * pszPath)				{ i3String::Copy( m_szBasePath, pszPath); }

	void		setUserProc( EXTERN_RES_CALLBACK pProc, void * pData)
	{
		m_pUserProc = pProc;
		m_pUserData = pData;
	}

	bool		getExternTextureEnable(void)				{ return m_bExternTexture; }
	void		setExternTextureEnable( bool bState)		{ m_bExternTexture = bState; }

	virtual void Trace( i3Node * pRoot);
	virtual BOOL OnNode( i3Node * pNode);
};

#endif
