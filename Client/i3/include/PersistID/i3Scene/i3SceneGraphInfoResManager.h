#if !defined( __I3_SCENEGRAPHINFO_RES_MANAGER_H)
#define __I3_SCENEGRAPHINFO_RES_MANAGER_H

class i3SceneGraphInfoResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3SceneGraphInfoResManager);
protected:

public:
	i3SceneGraphInfoResManager(void);
	virtual ~i3SceneGraphInfoResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);
};

#endif
