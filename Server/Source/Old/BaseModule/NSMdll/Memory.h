#ifndef __NSM_MEMORY_H
#define __NSM_MEMORY_H

#include "NSMHeader.h"

namespace NSM_nsMemory
{
	EXPORT_BASE void	Create(void);
	EXPORT_BASE void	Destroy(void);

	EXPORT_BASE void *	Alloc(INT32 nBytes, const char * FileName = NULL, INT32 Line = 0);
	EXPORT_BASE void *	Realloc(void * pPointer, INT32 nBytes, const char * FileName, INT32 Line);
	EXPORT_BASE void	Free(void * pPointer, const char * pszFile = NULL, INT32 line = 0);
		
	inline void	Copy( void * pDest, void * pSrc, UINT32 sz)		{ memcpy( pDest, pSrc, sz);	}
	inline void	FillZero( void * pDest, UINT32 sz)				{ memset( pDest, 0,	sz);	}
	inline void	Fill(void * pDest, INT32 Val, UINT32 sz)		{ memset( pDest, Val, sz);	}
	void Dump(void); 
}

#define NSM_COPY(pDest, pSrc, size)		NSM_nsMemory::Copy(pDest, pSrc, size); 

#define NSM_ALLOC(size)					NSM_nsMemory::Alloc(size, __FILE__, __LINE__); 
#define NSM_FILLZERO(P, S)				NSM_nsMemory::FillZero(P,S); 
#define	NSM_SAFE_FREE(P)				if(P!=NULL){NSM_nsMemory::Free(P); P=NULL; }

#define	SAFE_DELETE(P)					if(P!=NULL){delete(P); P=NULL; }
#define	SAFE_DELETE_ARRAY(P)			if(P!=NULL){delete[](P); P=NULL; }
#define	SAFE_RELEASE(P)					if(P!=NULL){p->Release();P=NULL; }

#endif
