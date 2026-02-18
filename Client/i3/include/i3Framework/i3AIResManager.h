#if !defined( __I3_TEXTURE_RES_MANAGER_H)
#define __I3_TEXTURE_RES_MANAGER_H

// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3AIResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3AIResManager, i3NamedResourceManager);

public:
	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName, bool bBackgroundLoad = false ) override;
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj) override;
};

#endif
