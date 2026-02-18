// i3ElementBase.h

#ifndef __I3_ELEMENT_H
#define __I3_ELEMENT_H
 
#include "i3Debug.h"
#include "i3Memory.h"
#include "i3ClassMeta.h"
#include "i3PropertyGridDefines.h"

///////////////////////////////////////////////////////////////
// Debug용으로 사용하는 define 입니다. 
#define I3ELEMENT_DUMP_ALL			0
#define I3ELEMENT_DUMP_I3			1
#define I3ELEMENT_DUMP_NOT_I3		2


#define		I3_CLASS_DEFINE_BASIC( classname)\
		public:\
		static i3ClassMeta *		s_pClassMeta##classname;\
		public:\
		inline static i3ClassMeta *		GetClassMeta(void)				{ return classname::s_pClassMeta##classname; }\
		virtual i3ClassMeta *			GetMeta(void);\
		static void RegisterMeta(void);\
		static void SetConcreteClass( i3ClassMeta * pMeta);\
		static void SetGhostClass( i3ClassMeta * pMeta);

#if !defined( I3_NO_STATIC_CLASSMETA)
	// ClassMeta
	#define I3_CLASS_META_REGISTER(classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname->Register();\
		}

	#define I3_CLASS_META_REGISTER_CONCRETE(classname, basename)\
		I3_CLASS_META_REGISTER( classname, basename)\

	#define I3_CLASS_META_REGISTER_GHOST(classname, basename)\
		I3_CLASS_META_REGISTER( classname, basename)\

	#define I3_CLASS_META_REGISTER_ABSTRACT(classname, basename)\
		I3_CLASS_META_REGISTER( classname, basename)\

	#define I3_CLASS_META_STATIC_DEF( classname, basename)\
		static i3MetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GENERAL); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

	#define I3_CLASS_META_STATIC_DEF_CONCRETE( classname, basename)\
		static i3MetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_CONCRETE); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

	#define I3_CLASS_META_STATIC_DEF_GHOST( classname, basename)\
		static i3MetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GHOST); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

	#define I3_CLASS_META_STATIC_DEF_ABSTRACT( classname, basename)\
		static i3AbstractMetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GENERAL); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

#else
	#define I3_CLASS_META_REGISTER( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3MetaTemplate<classname>( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GENERAL); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_REGISTER_CONCRETE( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3MetaTemplate<classname>( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_CONCRETE); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_REGISTER_GHOST( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3MetaTemplate<classname>( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GHOST); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_REGISTER_ABSTRACT( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3AbstractMetaTemplate<classname>( sizeof(classname), #classname, #basename, I3_CLASSMETA_TYPE_GENERAL); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_STATIC_DEF( classname, basename)\
		i3ClassMeta * classname::s_pClassMeta##classname = NULL;\

	#define I3_CLASS_META_STATIC_DEF_CONCRETE( classname, basename)\
		i3ClassMeta * classname::s_pClassMeta##classname = NULL;\

	#define I3_CLASS_META_STATIC_DEF_ABSTRACT( classname, basename)\
		i3ClassMeta * classname::s_pClassMeta##classname = NULL;\

#endif

#if defined( I3_DEBUG)
	#define I3_CLASS_DEFINE_FUNC( classname)\
		static classname *	_NewObject( const char * pszFile, int line);\
		static classname *	_NewObjectRef( const char * pszFile, int line);\
		static classname *	_BindObject( void * pBuf, const char * pszFile, int line);\
		static classname *	_BindObjectRef( void * pBuf, const char * pszFile, int line);\
		static classname *	_NewObjectByType( I3_CLASSMETA_TYPE type, const char * pszFile, int line);\
		static classname *	_NewObjectRefByType( I3_CLASSMETA_TYPE type, const char * pszFile, int line);\
		static classname *	_BindObjectByType( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line);\
		static classname *	_BindObjectRefByType( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line)

	#define		I3_CLASS_INSTANCE_FUNC( classname) \
		classname *	classname::_NewObject( const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( I3_CLASSMETA_TYPE_GENERAL, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRef( const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance( I3_CLASSMETA_TYPE_GENERAL, pszFile, line); \
		}\
		classname *	classname::_BindObject( void * pBuf, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, I3_CLASSMETA_TYPE_GENERAL, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRef( void * pBuf, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, I3_CLASSMETA_TYPE_GENERAL, pszFile, line); \
		}\
		classname *	classname::_NewObjectByType( I3_CLASSMETA_TYPE type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( type, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRefByType( I3_CLASSMETA_TYPE type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance( type, pszFile, line); \
		}\
		classname *	classname::_BindObjectByType( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRefByType( void * pBuf, I3_CLASSMETA_TYPE type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type, pszFile, line); \
		}\

#else
	#define I3_CLASS_DEFINE_FUNC( classname)\
		static classname *	_NewObject(void);\
		static classname *	_NewObjectRef(void);\
		static classname *	_BindObject( void * pBuf);\
		static classname *	_BindObjectRef( void * pBuf);\
		static classname *	_NewObjectByType( I3_CLASSMETA_TYPE type);\
		static classname *	_NewObjectRefByType( I3_CLASSMETA_TYPE type);\
		static classname *	_BindObjectByType( void * pBuf, I3_CLASSMETA_TYPE type);\
		static classname *	_BindObjectRefByType( void * pBuf, I3_CLASSMETA_TYPE type);

	#define		I3_CLASS_INSTANCE_FUNC( classname)\
		classname *	classname::_NewObject(void)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( I3_CLASSMETA_TYPE_GENERAL);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRef(void)\
		{\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance( I3_CLASSMETA_TYPE_GENERAL); \
		}\
		classname *	classname::_BindObject( void * pBuf)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, I3_CLASSMETA_TYPE_GENERAL);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRef( void * pBuf)\
		{\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, I3_CLASSMETA_TYPE_GENERAL); \
		}\
		classname *	classname::_NewObjectByType( I3_CLASSMETA_TYPE type)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( type);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRefByType( I3_CLASSMETA_TYPE type)\
		{\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance(type); \
		}\
		classname *	classname::_BindObjectByType( void * pBuf, I3_CLASSMETA_TYPE type)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRefByType( void * pBuf, I3_CLASSMETA_TYPE type)\
		{\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type); \
		}\

#endif

#define		I3_CLASS_INSTANCE_COMMON( classname, basename)\
		void classname::SetConcreteClass( i3ClassMeta * pMeta)\
		{\
			classname::s_pClassMeta##classname->SetConcreteClass( pMeta);\
		}\
		void classname::SetGhostClass( i3ClassMeta * pMeta)\
		{\
			classname::s_pClassMeta##classname->SetGhostClass( pMeta);\
		}\
		i3ClassMeta * classname::GetMeta(void)\
		{\
			return classname::s_pClassMeta##classname;\
		}\

#define		I3_CLASS_INSTANCE( classname, basename) \
	I3_CLASS_META_STATIC_DEF( classname, basename)\
	I3_CLASS_META_REGISTER( classname, basename)\
	I3_CLASS_INSTANCE_COMMON( classname, basename)\
	I3_CLASS_INSTANCE_FUNC( classname)

#define		I3_CONCRETE_CLASS_INSTANCE( classname, basename) \
	I3_CLASS_META_STATIC_DEF_CONCRETE( classname, basename)\
	I3_CLASS_META_REGISTER_CONCRETE( classname, basename)\
	I3_CLASS_INSTANCE_COMMON( classname, basename)\
	I3_CLASS_INSTANCE_FUNC( classname)

#define		I3_GHOST_CLASS_INSTANCE( classname, basename) \
	I3_CLASS_META_STATIC_DEF_GHOST( classname, basename)\
	I3_CLASS_META_REGISTER_GHOST( classname, basename)\
	I3_CLASS_INSTANCE_COMMON( classname, basename)\
	I3_CLASS_INSTANCE_FUNC( classname)

#define		I3_ABSTRACT_CLASS_INSTANCE( classname, basename) \
	I3_CLASS_META_STATIC_DEF_ABSTRACT( classname, basename)\
	I3_CLASS_META_REGISTER_ABSTRACT( classname, basename)\
	I3_CLASS_INSTANCE_COMMON( classname, basename)\
	I3_CLASS_INSTANCE_FUNC( classname)


#if defined( I3_DEBUG)
	#define	NewObject()						_NewObject( __FILE__,__LINE__)
	#define	NewObjectRef()					_NewObjectRef( __FILE__,__LINE__)
	#define BindObject(p)					_BindObject( (p), __FILE__, __LINE)
	#define BindObjectRef(p)				_BindObjectRef( (p), __FILE__, __LINE)
	#define	NewObjectByType(type)			_NewObjectByType( (type), __FILE__,__LINE__)
	#define	NewObjectRefByType(type)		_NewObjectRefByType( (type), __FILE__,__LINE__)
	#define BindObjectByType(p, type)		_BindObjectByType( (p), (type), __FILE__, __LINE)
	#define BindObjectRefByType(p, type)	_BindObjectRefByType( (p), (type), __FILE__, __LINE)
#else
	#define	NewObject()						_NewObject()
	#define	NewObjectRef()					_NewObjectRef()
	#define	BindObject(p)					_BindObject((p))
	#define	BindObjectRef(p)				_BindObjectRef((p))
	#define	NewObjectByType( type)			_NewObjectByType(type)
	#define	NewObjectRefByType( type)		_NewObjectRefByType(type)
	#define	BindObjectByType(p, type)		_BindObjectByType((p), (type))
	#define	BindObjectRefByType(p, type)	_BindObjectRefByType((p), (type))
#endif

#define		I3_CLASS_DEFINE( classname) \
	I3_CLASS_DEFINE_BASIC( classname)\
	I3_CLASS_DEFINE_FUNC( classname)
		

#define		I3_ABSTRACT_CLASS_DEFINE( classname)\
	I3_CLASS_DEFINE_BASIC( classname)\
	I3_CLASS_DEFINE_FUNC( classname)


#if defined( I3_DEBUG)
	#if defined( I3_COMPILER_VC)
		#define	NEW						new( __FILE__, __LINE__)
	#else
		#define	NEW						new( __FILE__, __LINE__)
	#endif
#else
	#if defined( I3_COMPILER_VC)
		#define	NEW						new
	#else
		#define	NEW						new
	#endif
#endif

#if defined( size_t)
#undef size_t
#endif

enum I3_COPY_METHOD	
{ 
	I3_COPY_REF = 0,
	I3_COPY_INSTANCE
};

class i3MemoryPool;

class I3_EXPORT_BASE i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3ElementBase);

#if defined( I3_DEBUG)
protected:
	const char *		m_pSourceFilePath;
	INT32				m_SourceLineCount;
	i3ElementBase *			m_pLeft; 
	i3ElementBase *			m_pRight; 
	INT32					m_MemTag;
#endif

protected:
	

	INT32					m_RefCount;
	BOOL					m_bBounded;

	i3MemoryPool *			m_pPool;

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

	void			__SetMemoryPool( i3MemoryPool * pPool)			{ m_pPool = pPool; }
	i3MemoryPool *	__GetMemoryPool(void)							{ return m_pPool; }

	bool	IsExactTypeOf( i3ClassMeta * pMeta)
	{
		return GetMeta()->IsExactTypeOf( pMeta);
	}

	bool	IsTypeOf( i3ClassMeta * pMeta)
	{
		return GetMeta()->IsTypeOf( pMeta);
	}

	#ifdef I3_DEBUG
		void *	operator new( size_t sz, const char *  pszFile , int  line ) NOTHROW;
		void *	operator new( size_t, void * pBuf, const char * /* pszFile */, int /* line */) NOTHROW;
		void	operator delete( void * p) NOTHROW;

		#if defined( I3_COMPILER_VC)
			void operator delete( void * p, const char *, int) NOTHROW;
			void operator delete( void *, void *, const char *, int) NOTHROW;
		#endif

		const char *_getSourceFileName(void)						{ return m_pSourceFilePath; }
		INT32		_getSourceLineCount(void)						{ return m_SourceLineCount; }

		void		_setCreatedLocation( const char * pszSrc, INT32 line)
		{
			m_pSourceFilePath = pszSrc;
			m_SourceLineCount = line;
		}
		
	#else
		void *	operator new( size_t sz ) NOTHROW;
		void *	operator new( size_t, void * pBuf ) NOTHROW;
		void	operator delete( void * p) NOTHROW;

		#if defined( I3_COMPILER_VC)
			void operator delete( void * p, const char *, int) NOTHROW;
			void operator delete( void *, void *) NOTHROW;
		#endif
	#endif

	INT32	AddRef(void)
	{
		m_RefCount++;

		return m_RefCount;
	}

	void	Release(void);

	// Internal use only!!!!
	void	_internal_SetRefCount( INT32 cnt)
	{
		m_RefCount = cnt;
	}
	
	virtual	void				CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	#if defined( I3_DEBUG)
	virtual void				_getInfoString( char * pszStr, const INT32 nStrMax);
	static void					DumpObjects( INT32 nFlag = I3ELEMENT_DUMP_ALL , i3ClassMeta * pMeta = NULL, INT32 tag = 0);
	static void					EnumObjects( DUMPCALLBACK pUserProc, void * pUserData);
	static void					DumpLastObject(void);
	static void					ReportClassInstanceCounts(void);
	static void					SnapMemoryTag( INT32 tag);

	static void					GetAllInstance( i3ClassMeta * pMeta, i3List * pList);

	INT32						getMemTag(void)				{ return m_MemTag; }
	void						setMemTag( INT32 tag)		{ m_MemTag = tag; }

	static void					setMemTagValue( INT32 tag);

	static i3ElementBase * 		getElementHead(void);
	i3ElementBase		*		getRight(void)				{ return m_pRight;}

	virtual void				GetTDKGridProperties( i3List* pList)			{}
	virtual void				OnTDKPropertyChanged( const char* szName)		{}	// szName: name of property 

	#endif
};

extern i3ElementBase * g_pFreeingObj;

#endif
