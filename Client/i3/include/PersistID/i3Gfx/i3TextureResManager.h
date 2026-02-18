#if !defined( __I3_TEXTURE_RES_MANAGER_H)
#define __I3_TEXTURE_RES_MANAGER_H

#include "i3Texture.h"

class i3TextureResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3TextureResManager);
protected:
	i3Texture *			_TryToLoad( const char * pszPath);

public:
	i3TextureResManager(void);
	virtual ~i3TextureResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);
};

#endif
