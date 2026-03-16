#if !defined( __I3_ANIMATION_RES_MANAGER_H)
#define __I3_ANIMATION_RES_MANAGER_H

#include "i3Animation.h"

class I3_EXPORT_MATH i3AnimationResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3AnimationResManager);
protected:

public:
	i3AnimationResManager(void);
	virtual ~i3AnimationResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);
};

#endif
