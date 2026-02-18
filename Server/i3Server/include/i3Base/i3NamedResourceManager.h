#if !defined( __I3_NAMED_RESOURCE_MANAGER_H)
#define __I3_NAMED_RESOURCE_MANAGER_H

//#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3MetaTemplate.h"
#include "i3Mutex.h"

class i3ResourceObject;
class i3BinList;
class i3ResourceFile;

class I3_EXPORT_BASE i3NamedResourceManager : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NamedResourceManager );
protected:
	i3BinList *						m_pList;
	i3NamedResourceManager *		m_pNextResManager;

	i3Mutex2 *						m_pMutex;
	
public:
	i3NamedResourceManager(void);
	~i3NamedResourceManager(void);

	INT32							DeleteUnusedObjects(void);

	void							AddResource( i3ResourceObject * pObj, const char * pszName);

	i3ResourceObject *				FindResource( const char * pszName);
	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);

	void							Dump(void);

	// 불필요한 Resource들을 모두 해제한다.
	static	void					Reset(void);
	static i3NamedResourceManager *	FindByMeta( i3ClassMeta * pMeta);

	static void						SetCommonResourceState( bool bState);
};

#endif
