#if !defined( __I3_META_TEMPLATE_H)
#define __I3_META_TEMPLATE_H

#include "i3ElementBase.h"
#include "i3MemoryPool.h"
#include "i3ClassMeta.h"
#include "i3Macro.h"

/////////////////////////////////////////////////////////////////////////////
// 
template <class T>
class i3MetaTemplate  : public i3ClassMeta
{
public:
	i3MetaTemplate( UINT32 classSz, char * pszClassName, char * pszBaseClassName, BOOL bConcrete) NOTHROW;

#if defined( I3_DEBUG)
	virtual i3ElementBase * CreateInstance( INT32 type, const char * pszFile, int line);
	virtual i3ElementBase * BindInstance( void * pBuf, INT32 type, const char * pszFIle, int line);
#else
	virtual i3ElementBase * CreateInstance( INT32 type);
	virtual i3ElementBase * BindInstance( void * pBuf, INT32 type);
#endif
};

template <class T>
i3MetaTemplate<T>::i3MetaTemplate( UINT32 classSz, char * pszClassName, char * pszBaseClassName, BOOL bConcrete)
: i3ClassMeta( classSz, pszClassName, pszBaseClassName, bConcrete)
{
}

#undef new

#if defined( I3_DEBUG)
	template <class T>
	i3ElementBase * i3MetaTemplate<T>::CreateInstance( INT32 type, const char * pszFile, int line)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->CreateInstance( 0, pszFile, line);
		}
		else
		{
			if( m_pPool == NULL)
			{
				i3ElementBase * pNew;

				pNew = (i3ElementBase *)new( pszFile, line) T;

				return pNew;
			}
			else
			{
				i3ElementBase * p = (i3ElementBase *) m_pPool->Alloc( sizeof(T), pszFile, line);

				BindInstance( p, 0, pszFile, line);
				p->__SetMemoryPool( m_pPool);

				return p;
			}			
		}
	}

	template <class T>
	i3ElementBase * i3MetaTemplate<T>::BindInstance( void * pBuf, INT32 type, const char * pszFile, int line)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->BindInstance( pBuf, 0, pszFile, line);
		}
		else
		{
			#if defined( I3_DEBUG)
				return (i3ElementBase *) new( pBuf, pszFile, line) T;
			#else
				return (i3ElementBase *) new( pBuf, pszFile, line) T;
			#endif
		}
	}
#else
	template <class T>
	i3ElementBase * i3MetaTemplate<T>::CreateInstance( INT32 type)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->CreateInstance( 0);
		}
		else
		{
			if( m_pPool == NULL)
			{
				#if defined( I3_DEBUG)
					return (i3ElementBase *) new T;
				#else
					return (i3ElementBase *) new T;
				#endif
			}
			else
			{
				i3ElementBase * p = (i3ElementBase *) m_pPool->Alloc( sizeof(T), NULL, 0);
	
				BindInstance( p, 0);
				p->__SetMemoryPool( m_pPool);

				return p;
			}
		}
	}

	template <class T>
	i3ElementBase * i3MetaTemplate<T>::BindInstance( void * pBuf, INT32 type)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->BindInstance( pBuf, 0);
		}
		else
		{
			#if defined( I3_DEBUG)
				return (i3ElementBase *) new( pBuf ) T;
			#else
				return (i3ElementBase *) new( pBuf ) T;
			#endif
		}
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// 
template <class T>
class i3AbstractMetaTemplate  : public i3ClassMeta
{
public:
	i3AbstractMetaTemplate( UINT32 classSz, char * pszClassName, char * pszBaseClassName, BOOL bConcrete) NOTHROW;

#if defined( I3_DEBUG)
	virtual i3ElementBase * CreateInstance( INT32 type, const char * pszFile, int line);
	virtual i3ElementBase * BindInstance( void * pBuf, INT32 type, const char * pszFile, int line);
#else
	virtual i3ElementBase * CreateInstance( INT32 type);
	virtual i3ElementBase * BindInstance( void * pBuf, INT32 type);
#endif
};

template <class T>
i3AbstractMetaTemplate<T>::i3AbstractMetaTemplate( UINT32 classSz, char * pszClassName, char * pszBaseClassName, BOOL bConcrete)
: i3ClassMeta( classSz, pszClassName, pszBaseClassName, bConcrete)
{
	AddStyle( I3_CLASSMETA_STYLE_ABSTRACT);
}

#if defined( I3_DEBUG)
	template <class T>
	i3ElementBase * i3AbstractMetaTemplate<T>::CreateInstance( INT32 type, const char * pszFile, int line)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->CreateInstance( 0, pszFile, line);
		}
		else
		{
			#if defined( I3_DEBUG)
				return NULL;
			#else
				return NULL;
			#endif
		}
	}

	template <class T>
	i3ElementBase * i3AbstractMetaTemplate<T>::BindInstance( void * pBuf, INT32 type, const char * pszFile, int line)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->BindInstance( pBuf, 0, pszFile, line);
		}
		else
		{
			#if defined( I3_DEBUG)
				return NULL;
			#else
				return NULL;
			#endif
		}
	}
#else
	template <class T>
	i3ElementBase * i3AbstractMetaTemplate<T>::CreateInstance( INT32 type)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->CreateInstance(0);
		}
		else
		{
			#if defined( I3_DEBUG)
				return NULL;
			#else
				return NULL;
			#endif
		}
	}

	template <class T>
	i3ElementBase * i3AbstractMetaTemplate<T>::BindInstance( void * pBuf, INT32 type)
	{
		if( m_pProxyInfo != NULL)
		{
			i3ClassMeta * pMeta = GetConcreteClass( type);

			return pMeta->BindInstance( pBuf, 0);
		}
		else
		{
			#if defined( I3_DEBUG)
				return NULL;
			#else
				return NULL;
			#endif
		}
	}
#endif

#endif
