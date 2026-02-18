#if !defined( __I3_TEXTURE_RES_MANAGER_H)
#define __I3_TEXTURE_RES_MANAGER_H

class i3AIResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3AIResManager);
protected:

public:
	i3AIResManager(void);
	virtual ~i3AIResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);
};

#endif
