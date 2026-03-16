#if !defined( __I3_MEMORY_POOL_H)
#define __I3_MEMORY_POOL_H

class I3_EXPORT_BASE i3MemoryPool
{
protected:

public:
	i3MemoryPool(void) NOTHROW;
	virtual ~i3MemoryPool(void) NOTHROW;

	virtual	void *		Alloc( UINT32 Sz, const char * pszFile, INT32 line) = 0;
	virtual	void		Free( void * p) = 0;

	#if defined( I3_NO_STATIC_CLASSMETA)
		#if defined( I3_WIPI_GIGA)
			void *	operator new( UINT32 sz) NOTHROW;
		#else
			void * operator new( size_t sz) NOTHROW;
		#endif
		void	operator delete( void * p) NOTHROW;
	#endif
};

#endif
