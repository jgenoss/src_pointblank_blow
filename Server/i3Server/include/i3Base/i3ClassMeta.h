#if !defined( __I3_CLASS_META_H)
#define __I3_CLASS_META_H

//#include "i3CommonType.h"
#include "i3System.h"
#include "i3Macro.h"

class i3ElementBase;
class i3List;
class i3MemoryPool;
class i3ClassMeta;

#define		I3_CLASSMETA_STYLE_ABSTRACT		0x00000001
#define		I3_CLASSMETA_STYLE_CONCRETE		0x00000002
#define		I3_CLASSMETA_STYLE_GHOST		0x00000004

enum I3_CLASSMETA_TYPE
{
	I3_CLASSMETA_TYPE_GENERAL,
	I3_CLASSMETA_TYPE_CONCRETE,
	I3_CLASSMETA_TYPE_GHOST
};

#define		I3_CLASSMETA_PROXY_MAX		4

struct I3CLASSMETAPROXYINFO
{
	i3ClassMeta *	m_pMeta;
};

class I3_EXPORT_BASE i3ClassMeta
{
protected:
	i3ClassMeta *	m_pNext;			// internal usage

protected:
	UINT32		m_ClassSize;
	char *		m_pszClassName;
	char *		m_pszBaseClassName;
	UINT32		m_Style;
	UINT32		m_HashCode;

	i3ClassMeta *	m_pBaseClass;
	I3CLASSMETAPROXYINFO 	m_ProxyInfo[I3_CLASSMETA_PROXY_MAX];
	INT32					m_ProxyCount;
	i3MemoryPool *	m_pPool;

#if defined( I3_DEBUG)
	INT32		m_InstanceCount;
#endif

public:
	i3ClassMeta( UINT32 classSz, char * pszClassName, char * pszBaseClassName, I3_CLASSMETA_TYPE Type) NOTHROW;
	virtual ~i3ClassMeta(void) NOTHROW;

	#if defined( I3_NO_STATIC_CLASSMETA)
		#if defined( I3_WIPI_GIGA)
			void *	operator new( UINT32 sz) NOTHROW;
		#else
			void *	operator new( size_t sz) NOTHROW;
		#endif
		void	operator delete( void * p) NOTHROW;
	#endif

	i3MemoryPool *	GetMemoryPool(void)							{ return m_pPool; }
	void			SetMemoryPool( i3MemoryPool * pPool)		{ m_pPool = pPool; }

	#if defined( I3_DEBUG)
		virtual i3ElementBase *	CreateInstance( I3_CLASSMETA_TYPE type, const char * pszFile, int line);
		virtual i3ElementBase * BindInstance( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line);
	#else
		virtual i3ElementBase *	CreateInstance( I3_CLASSMETA_TYPE type);
		virtual i3ElementBase * BindInstance( void * pBuf, I3_CLASSMETA_TYPE type);
	#endif

	UINT32		GetClassSize(void)
	{
		return m_ClassSize;
	}

	char *		GetClassName(void)
	{
		return m_pszClassName;
	}

	char *		GetPersistClassName(void);

	bool		IsExactTypeOf( i3ClassMeta * pMeta)
	{
		return pMeta == this;
	}

	bool			IsTypeOf( i3ClassMeta * pMeta);

	void			Register(void);
	static	void	UnRegister(void);

	void			SetStyle( UINT32 style)					{ m_Style = style; }
	UINT32			GetStyle(void)							{ return m_Style; }
	void			AddStyle( UINT32 style)					{ m_Style |= style; }
	void			RemoveStyle( UINT32 style)				{ m_Style &= ~style; }
	BOOL			IsStyle( UINT32 style)					{ return (m_Style & style) == style; }

	BOOL			IsAbstractClass(void)					{ return IsStyle( I3_CLASSMETA_STYLE_ABSTRACT); }
	BOOL			IsConcreteClass(void)					{ return IsStyle( I3_CLASSMETA_STYLE_CONCRETE); }
	i3ClassMeta *	GetConcreteClass( I3_CLASSMETA_TYPE type = I3_CLASSMETA_TYPE_GENERAL);
	i3ClassMeta *	GetBaseClassMeta(void)					{ return m_pBaseClass; }
	void			SetConcreteClass( i3ClassMeta * pMeta);
	void			SetGhostClass( i3ClassMeta * pMeta);
	void			RemoveProxyInfo(void);

	UINT32			getHashCode(void)						{ return m_HashCode; }

	// 파생된 클래스들을 리스트에 저장합니다.
	void			GetAllDerivedClasses( i3List * pList, bool bRecursive = true);

	static	i3ClassMeta *	FindClassMetaByName( const char * pszClassName);
	static	i3ClassMeta *	FindClassMetaByHash( UINT32 hash);
	static	void			PrintClassMeta();

	static BOOL				_ClassMetaTerminateProc( void * pData);

	#if defined( I3_DEBUG)
	static	void			DumpClassMeta(void);
	static	void			ReportMetaInstanceToFile(void);
	
	void					AddInstanceCount(INT32 cnt)		{	m_InstanceCount += cnt;}
	INT32					getInstanceCount(void)			{	return m_InstanceCount;}
	
	bool isAlready( char * pszClassName);
	static void				CheckCollision(void);
	#endif

};

#if defined( I3_DEBUG)
	#define	CREATEINSTANCE( pMeta)				( pMeta->CreateInstance( I3_CLASSMETA_TYPE_GENERAL, __FILE__, __LINE__))
	#define	CREATEINSTANCE_TYPE( pMeta, type)	( pMeta->CreateInstance( type, __FILE__, __LINE__))
	#define BINDINSTANCE( pMeta, p)				( pMeta->BindInstance( p, I3_CLASSMETA_TYPE_GENERAL, __FILE__, __LINE__))
	#define BINDINSTANCE_TYPE( pMeta, p, type)	( pMeta->BindInstance( p, type, __FILE__, __LINE__))
#else
	#define CREATEINSTANCE( pMeta)				( pMeta->CreateInstance( I3_CLASSMETA_TYPE_GENERAL))
	#define CREATEINSTANCE_TYPE( pMeta, type)	( pMeta->CreateInstance(type))
	#define BINDINSTANCE( pMeta, p)				( pMeta->BindInstance( p, I3_CLASSMETA_TYPE_GENERAL))
	#define BINDINSTANCE_TYPE( pMeta, p, type)	( pMeta->BindInstance( p, type))
#endif


#endif
