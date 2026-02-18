// i3ElementBase.h

#ifndef __I3_ELEMENT_H
#define __I3_ELEMENT_H
 
#include "i3Debug.h"
#include "i3Memory.h"
#include "i3PropertyGridDefines.h"
#include "class_meta/class_meta_macro.h"

#if defined( I3_DEBUG)
	#include "itl/vector_map.h"
#endif

///////////////////////////////////////////////////////////////
// Debug용으로 사용하는 define 입니다. 
#define I3ELEMENT_DUMP_ALL			0
#define I3ELEMENT_DUMP_I3			1
#define I3ELEMENT_DUMP_NOT_I3		2

#if defined( size_t)
#undef size_t
#endif

enum I3_COPY_METHOD	
{ 
	I3_COPY_REF = 0,
	I3_COPY_INSTANCE
};

class i3MemoryPool;


typedef i3::class_meta				i3ClassMeta;
typedef i3::class_meta_system		i3ClassMetaSystem;


class I3_EXPORT_BASE i3ElementBase 
{
	I3_EXPORT_ROOT_CLASS_DEFINE(i3ElementBase)

#if defined( I3_DEBUG)
protected:
	const char *		m_pSourceFilePath = nullptr;
	INT32				m_SourceLineCount = 0;
	i3ElementBase *		m_pLeft = nullptr; 
	i3ElementBase *		m_pRight = nullptr;
	INT32				m_MemTag = 0;

	struct vmData {
		i3::string	_str;
		UINT32		_refCnt;
	};
	typedef i3::vector<vmData*>		vm_hashData;
	vm_hashData		m_RefDebugList;
#endif

protected:
	INT32					m_RefCount = 0;

public:
	#if defined( I3_DEBUG)
	typedef bool	(*DUMPCALLBACK)( i3ElementBase * pObj, void * pUserData);
	#endif

public:
	i3ElementBase() NOTHROW;
	virtual ~i3ElementBase() NOTHROW;

	INT32	GetRefCount(void)
	{
		return m_RefCount;
	}


	#ifdef I3_DEBUG
		const char *_getSourceFileName(void)						{ return m_pSourceFilePath; }
		INT32		_getSourceLineCount(void)						{ return m_SourceLineCount; }

		void		_setCreatedLocation( const char * pszSrc, INT32 line)
		{
			m_pSourceFilePath = pszSrc;
			m_SourceLineCount = line;
		}
	#endif

	INT32	AddRef(void)
	{
#if defined( I3_WINDOWS)
		return ::InterlockedIncrement( (LONG*) &m_RefCount);
#else
		I3ASSERT_0;		// 다른 플렛폼에서 ThreadSafe하게 변경할 필요가 있다!!!!!
		return m_RefCount++;
#endif
	}
	void	Release(void);

#if defined( I3_DEBUG)
	INT32	AddRef( const char * pszFile, INT32 line);
	void	Release( const char * pszFile, INT32 line);
#endif

	// Internal use only!!!!
	void	_internal_SetRefCount( INT32 cnt)
	{
#if defined( I3_WINDOWS)
		::InterlockedExchange( (LONG*) &m_RefCount, (LONG) cnt);
#else
		I3ASSERT_0;		// 다른 플렛폼에서 ThreadSafe하게 변경할 필요가 있다!!!!!
		m_RefCount = cnt;
#endif
	}
	
	virtual	void				CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	#if defined( I3_DEBUG)
	virtual void				_getInfoString( char * pszStr, const INT32 nStrMax);
	static void					DumpObjects( INT32 nFlag = I3ELEMENT_DUMP_ALL , i3ClassMeta * pMeta = nullptr, INT32 tag = 0);
	static void					EnumObjects( DUMPCALLBACK pUserProc, void * pUserData);
	static void					DumpLastObject(void);
	static void					ReportClassInstanceCounts(void);
	static void					SnapMemoryTag( INT32 tag);

	static void					GetAllInstance( i3ClassMeta * pMeta, i3::vector<i3ElementBase*>& List);

	INT32						getMemTag(void)				{ return m_MemTag; }
	void						setMemTag( INT32 tag)		{ m_MemTag = tag; }

	static void					setMemTagValue( INT32 tag);

	static i3ElementBase * 		getElementHead(void);
	i3ElementBase		*		getRight(void)				{ return m_pRight;}

	virtual void				GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)	{}
	virtual void				OnTDKPropertyChanged( const char* szName)		{}	// szName: name of property 

	#endif
};

extern i3ElementBase * g_pFreeingObj;


namespace i3
{
	inline void element_add_ref(i3ElementBase* p) 
	{  
		I3_SAFE_ADDREF(p);
	}
	inline void element_on_create_file_line(i3ElementBase* p, const char* file, int line)
	{
#if defined(I3_DEBUG)
		p->_setCreatedLocation(file, line);
#endif
	}

	namespace container_util
	{
		namespace detail
		{
			struct SafeRelease
			{
				inline void operator()( i3ElementBase* p) const {	I3_SAFE_RELEASE( p); }
			};
		}

		template<class Ctn>
		void for_each_SafeRelease( Ctn& ctn )
		{
			i3::cu::for_each_container(ctn,	detail::SafeRelease() );
		}
		template<class Ctn>
		void for_each_SafeRelease_clear( Ctn& ctn )
		{
			i3::cu::for_each_container(ctn,	detail::SafeRelease() );	ctn.clear();
		}
	}
}

#endif
