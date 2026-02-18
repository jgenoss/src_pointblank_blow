// i3ElementBase.h

#ifndef __I3_ELEMENT_H
#define __I3_ELEMENT_H
 
#include "i3Debug.h"
#include "i3Memory.h"
#include "i3ClassMeta.h"

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
		static void SetConcreteClass( i3ClassMeta * pMeta, INT32 type = 0);

#if !defined( I3_NO_STATIC_CLASSMETA)
	// ClassMeta
	#define I3_CLASS_META_REGISTER(classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname->Register();\
		}

	#define I3_CLASS_META_REGISTER_CONCRETE(classname, basename)\
		I3_CLASS_META_REGISTER( classname, basename)\

	#define I3_CLASS_META_REGISTER_ABSTRACT(classname, basename)\
		I3_CLASS_META_REGISTER( classname, basename)\

	#define I3_CLASS_META_STATIC_DEF( classname, basename)\
		static i3MetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, FALSE); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

	#define I3_CLASS_META_STATIC_DEF_CONCRETE( classname, basename)\
		static i3MetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, TRUE); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

	#define I3_CLASS_META_STATIC_DEF_ABSTRACT( classname, basename)\
		static i3AbstractMetaTemplate<classname> s_ClassMeta##classname( sizeof(classname), #classname, #basename, FALSE); \
		i3ClassMeta * classname::s_pClassMeta##classname = &s_ClassMeta##classname;\

#else
	#define I3_CLASS_META_REGISTER( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3MetaTemplate<classname>( sizeof(classname), #classname, #basename, FALSE); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_REGISTER_CONCRETE( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3MetaTemplate<classname>( sizeof(classname), #classname, #basename, TRUE); \
			classname::s_pClassMeta##classname->Register();\
		}\

	#define I3_CLASS_META_REGISTER_ABSTRACT( classname, basename)\
		void classname::RegisterMeta(void)\
		{\
			classname::s_pClassMeta##classname = new i3AbstractMetaTemplate<classname>( sizeof(classname), #classname, #basename, FALSE); \
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
		static classname *	_NewObjectByType( INT32 type, const char * pszFile, int line);\
		static classname *	_NewObjectRefByType( INT32 type, const char * pszFile, int line);\
		static classname *	_BindObjectByType( void * pBuf, INT32 type, const char * pszFile, int line);\
		static classname *	_BindObjectRefByType( void * pBuf, INT32 type, const char * pszFile, int line)

	#define		I3_CLASS_INSTANCE_FUNC( classname) \
		classname *	classname::_NewObject( const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( 0, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRef( const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance( 0, pszFile, line); \
		}\
		classname *	classname::_BindObject( void * pBuf, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, 0, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRef( void * pBuf, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, 0, pszFile, line); \
		}\
		classname *	classname::_NewObjectByType( INT32 type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( type, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRefByType( INT32 type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance( type, pszFile, line); \
		}\
		classname *	classname::_BindObjectByType( void * pBuf, INT32 type, const char * pszFile, int line)\
		{\
			I3ASSERT( classname::s_pClassMeta##classname != NULL );\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type, pszFile, line);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRefByType( void * pBuf, INT32 type, const char * pszFile, int line)\
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
		static classname *	_NewObjectByType( INT32 type);\
		static classname *	_NewObjectRefByType( INT32 type);\
		static classname *	_BindObjectByType( void * pBuf, INT32 type);\
		static classname *	_BindObjectRefByType( void * pBuf, INT32 type);

	#define		I3_CLASS_INSTANCE_FUNC( classname)\
		classname *	classname::_NewObject(void)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance(0);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRef(void)\
		{\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance(0); \
		}\
		classname *	classname::_BindObject( void * pBuf)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, 0);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRef( void * pBuf)\
		{\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, 0); \
		}\
		classname *	classname::_NewObjectByType( INT32 type)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->CreateInstance( type);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_NewObjectRefByType( INT32 type)\
		{\
			return (classname *) classname::s_pClassMeta##classname->CreateInstance(type); \
		}\
		classname *	classname::_BindObjectByType( void * pBuf, INT32 type)\
		{\
			classname * p = (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type);\
			p->AddRef();\
			return p;\
		}\
		classname *	classname::_BindObjectRefByType( void * pBuf, INT32 type)\
		{\
			return (classname *) classname::s_pClassMeta##classname->BindInstance( pBuf, type); \
		}\

#endif

#define		I3_CLASS_INSTANCE_COMMON( classname, basename)\
		void classname::SetConcreteClass( i3ClassMeta * pMeta, INT32 type)\
		{\
			classname::s_pClassMeta##classname->SetConcreteClass( pMeta, type);\
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
protected:
	#if defined( I3_DEBUG)

		#ifdef I3_WINDOWS
		char		m_SourceFileName[MAX_PATH];
		INT32		m_SourceLineCount;
		#endif

	i3ElementBase *			m_pLeft; 
	i3ElementBase *			m_pRight; 
	#endif

	INT32					m_RefCount;
	BOOL					m_bBounded;

	INT32					m_MemTag;
	i3MemoryPool *			m_pPool;

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

		#ifdef I3_WINDOWS
			char *	_getSourceFileName(void)						{ return m_SourceFileName; }
			INT32	_getSourceLineCount(void)						{ return m_SourceLineCount; }
		#endif
		
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
	
	virtual	void				CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	#if defined( I3_DEBUG)
	virtual char *				_getInfoString(void);
	static void					DumpObjects( INT32 nFlag = I3ELEMENT_DUMP_ALL , i3ClassMeta * pMeta = NULL, INT32 tag = 0);
	static void					DumpLastObject(void);
	static void					SnapMemoryTag( INT32 tag);

	INT32						getMemTag(void)				{ return m_MemTag; }
	void						setMemTag( INT32 tag)		{ m_MemTag = tag; }

	static void					setMemTagValue( INT32 tag);

	static i3ElementBase * 		getElementHead(void);
	i3ElementBase		*		getRight(void)				{ return m_pRight;}

	#endif
};

extern i3ElementBase * g_pFreeingObj;

#endif
