#if !defined( __I3_NAMED_RESOURCE_MANAGER_H)
#define __I3_NAMED_RESOURCE_MANAGER_H

//#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3Mutex.h"
#include "itl/vector_set.h"

class i3ResourceObject;
class i3ResourceFile;

bool	operator<(const i3ResourceObject& a1, const i3ResourceObject& a2);
template<> struct i3::less<i3ResourceObject*> : i3::less_ptr<i3ResourceObject*> {};


class I3_EXPORT_BASE i3NamedResourceManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NamedResourceManager, i3ElementBase );
protected:

	i3::vector_multiset<i3ResourceObject*>	m_List;

	i3NamedResourceManager *		m_pNextResManager;			// [initialize at constructor]

	i3Mutex2 *						m_pMutex;					// [initialize at constructor]
	
public:
	i3NamedResourceManager(void);
	~i3NamedResourceManager(void);

	INT32							DeleteUnusedObjects(void);

	void							AddResource( i3ResourceObject * pObj, const char * pszName);
	void							DeleteResource( UINT32 idx);

	i3ResourceObject *				FindResource( const char * pszName);
	/** \brief Resource를 얻어온다.
		\note Find를 하고 없으면 로딩을 합니다.
		\param[in] pszResFile	Full path name
		\param[in] pszBasePath	base path name
		\param[in] szName		Res File Name
		\param[in] bBackgroundLoad	bg 로드 여부
		\param[in] pOwner bg로드시 로드하는 주체(로드 완료시 접근하기 위한 용도) */
	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName,
										bool bBackgroundLoad = false);

	UINT32							GetResourceCount( void);
	virtual i3ResourceObject *		GetResource( UINT32 idx);
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);

	void							Dump(void);

	// 불필요한 Resource들을 모두 해제한다.
	static	void					Reset(void);
	static i3NamedResourceManager *	FindByMeta( i3ClassMeta * pMeta);

	static void						SetCommonResourceState( bool bState);

	static void						DumpAll( void);
};

#endif
