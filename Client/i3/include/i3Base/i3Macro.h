#if !defined( __I3_MACRO_H)
#define __I3_MACRO_H

#include "i3OverloadingMacro.h"

#define			SETBIT( a, b)							((a) |= (b))
#define			UNSETBIT( a, b)							((a) &= ~(b))
#define			TESTBIT( a, b)							((a) & (b))

#define			MIN( a, b)								( (a) > (b) ? (b) : (a))
#define			MAX( a, b)								( (a) > (b) ? (a) : (b))
#define			MINMAX( _min_, _v_, _max_ )				( MIN( MAX( (_min_), (_v_) ), (_max_) ) )

#define			LO_16( a)								((UINT16)(a & 0xFFFF))
#define			HI_16( a)								(UINT16)(( (a >> 16) & 0xFFFF))
#define			MAKE_32( l, h)							(UINT32)(( (h << 16) | (l & 0xFFFF)))

#if defined( I3_DEBUG)
	#define			I3_SAFE_DELETE(P)						if(P!=nullptr){ NetworkDump_Free(P, __FILE__, __LINE__); delete P; P=nullptr;}
	#define			I3_MUST_DELETE(P)						{ I3ASSERT(P != nullptr); NetworkDump_Free(P, __FILE__, __LINE__); delete P; }
	#define			I3_SAFE_DELETE_ARRAY(P)					if(P!=nullptr){ NetworkDump_Free(P, __FILE__, __LINE__); delete[] P; P=nullptr;}
	#define			I3_MUST_DELETE_ARRAY(P)					{ I3ASSERT( P != nullptr); NetworkDump_Free(P, __FILE__, __LINE__); delete[] P;}
	#define			I3_MUST_RELEASE(P)						{ I3ASSERT(P != nullptr); P->Release(__FILE__,__LINE__);}
	#define			I3_SAFE_RELEASE(P)						if(P!=nullptr){P->Release(__FILE__,__LINE__); P=nullptr;}
	#define			I3_SAFE_RELEASE_NONULL_POINTER(P)		if(P!=nullptr){ P->Release(__FILE__,__LINE__); }
	#define			I3_SAFE_RELEASE_NODBG(P)				if(P!=nullptr){P->Release(); P=nullptr;}
	#define			I3_SAFE_RELEASE_NONULL_POINTER_NODBG(P)	if(P!=nullptr){ P->Release(); }
#else
	#define			I3_SAFE_DELETE(P)						if(P!=nullptr){delete P; P=nullptr;}
	#define			I3_MUST_DELETE(P)						{ delete P; }
	#define			I3_SAFE_DELETE_ARRAY(P)					if(P!=nullptr){delete[] P; P=nullptr;}
	#define			I3_MUST_DELETE_ARRAY(P)					{ delete[] P;}
	#define			I3_MUST_RELEASE(P)						{P->Release();}
	#define			I3_SAFE_RELEASE(P)						if(P!=nullptr){P->Release(); P=nullptr;}
	#define			I3_SAFE_RELEASE_NONULL_POINTER(P)		if(P!=nullptr){ P->Release(); }
	#define			I3_SAFE_RELEASE_NODBG(P)				I3_SAFE_RELEASE(P)
	#define			I3_SAFE_RELEASE_NONULL_POINTER_NODBG(P)	I3_SAFE_RELEASE_NONULL_POINTER(P)
#endif

#if defined( I3_DEBUG)
	#define			I3_MUST_ADDREF(P)						{ if (P != nullptr) P->AddRef(__FILE__,__LINE__); else I3ASSERT(P != nullptr); }
	#define			I3_SAFE_ADDREF(P)						if(P != nullptr) { P->AddRef(__FILE__,__LINE__); }
	#define			I3_SAFE_ADDREF_NODBG(P)					if(P != nullptr) { P->AddRef(); }
#else
	#define			I3_MUST_ADDREF(P)						{ P->AddRef(); }
	#define			I3_SAFE_ADDREF(P)						if(P != nullptr) { P->AddRef(); }
	#define			I3_SAFE_ADDREF_NODBG(P)					I3_SAFE_ADDREF(P)
#endif

//i3Memory에서 메모리 삭제하기 
#define			I3MEM_SAFE_FREE_POINTER(P)				if(P!=nullptr){i3MemFree(P); P=nullptr; }
#define			I3MEM_SAFE_FREE(P)						if(P!=nullptr){i3MemFree(P); P=nullptr; }

#if defined( I3_DEBUG)
#define			I3_REF_CHANGE( pVar, pNew)\
	if( (pNew) != nullptr)		(pNew)->AddRef(__FILE__,__LINE__);\
	I3_SAFE_RELEASE( pVar);\
	(pVar) = (pNew)
#else	// I3_DEBUG
#define			I3_REF_CHANGE( pVar, pNew)\
	if( (pNew) != nullptr)		(pNew)->AddRef();\
	I3_SAFE_RELEASE( pVar);\
	(pVar) = (pNew)
#endif

#if defined( I3_WIPI)
#define			sprintf				MC_knlSprintk
#define			printf				MC_knlPrintk
#endif

#if defined( I3_WINDOWS) || defined( I3_WINCE )

	#define	__FOPEN( name, flag, mode)			CreateFile( name, flag, mode)
	#define __FCLOSE( h)						CloseHandle( h)
	#define __FREAD( pBuf, size, count, fp)		ReadFile( (fp), (pBuf), (size) * (count))
	#define __FWRITE( pBuf, size, count, fp)	sceIoWrite( (fp), (pBuf), (size) * (count))
	#define __FSEEK( fp, offset, origin)		sceIoLseek( fp, offset, origin)
	#define __FTELL( fp)						sceIoLseek( fp, 0, SCE_SEEK_CUR)
	#define __CHECKIORESULT( a)					(a < 0)
	#define	__HANDLE_TYPE						HANDLE

	#define	__SEEK_SET							SEEK_SET
	#define	__SEEK_END							SEEK_END
	#define	__SEEK_CUR							SEEK_CUR
#elif defined( I3_PSP)
	#define	__FOPEN( name, flag, mode)			sceIoOpen( name, flag, mode)
	#define __FCLOSE( h)						sceIoClose( h)
	#define __FREAD( pBuf, size, count, fp)		sceIoRead( (fp), (pBuf), (size) * (count))
	#define __FWRITE( pBuf, size, count, fp)	sceIoWrite( (fp), (pBuf), (size) * (count))
	#define __FSEEK( fp, offset, origin)		sceIoLseek( fp, offset, origin)
	#define __FTELL( fp)						sceIoLseek( fp, 0, SCE_SEEK_CUR)
	#define __CHECKIORESULT( a)					(a < 0)
	#define	__HANDLE_TYPE						SceUID

	#define	__SEEK_SET							SEEK_SET
	#define	__SEEK_END							SEEK_END
	#define	__SEEK_CUR							SEEK_CUR
#elif defined( I3_WIPI)
	#define	__FOPEN( name, flag, mode)			MC_fsOpen((M_Char*)(name), (flag), (mode))
	#define __FCLOSE( h)						MC_fsClose( h)
#if defined( I3_WIPI_GIGA)	// 05.09.27 by dfly -> KTF에서는 char*로 받는다.
	#define __FREAD( pBuf, size, count, fp)		MC_fsRead( (fp), (M_Byte*)(pBuf), (size) * (count))
	#define __FWRITE( pBuf, size, count, fp)	MC_fsWrite( (fp), (M_Byte*)(pBuf), (size) * (count))
#else
	#define __FREAD( pBuf, size, count, fp)		MC_fsRead( (fp), (char*)(pBuf), (size) * (count))
	#define __FWRITE( pBuf, size, count, fp)	MC_fsWrite( (fp), (char*)(pBuf), (size) * (count))
#endif
	#define __FSEEK( fp, offset, origin)		MC_fsSeek( (fp), (offset), (origin))
	#define __FTELL( fp)						MC_fsTell( fp)
	#define __CHECKIORESULT( a)					(a < 0)
	#define	__HANDLE_TYPE						INT32

	#define	__SEEK_SET							MC_FILE_SEEK_SET
	#define	__SEEK_END							MC_FILE_SEEK_END
	#define	__SEEK_CUR							MC_FILE_SEEK_CUR
#else
	#define	__FOPEN( name, flag, mode)			fopen( name, mode)
	#define __FCLOSE( h)						fclose( h)
	#define __FREAD( pBuf, size, count, fp)		fread( pBuf, size, count, fp)
	#define __FWRITE( pBuf, size, count, fp)	fwrite( pBuf, size, count, fp)
	#define __FSEEK( fp, offset, origin)		fseek( fp, offset, origin)
	#define __FTELL( fp)						ftell( fp)
	#define __CHECKIORESULT( a)					(a == nullptr)
	#define	__HANDLE_TYPE						FILE * 

	#define	__SEEK_SET							SEEK_SET
	#define	__SEEK_END							SEEK_END
	#define	__SEEK_CUR							SEEK_CUR
#endif

#define		I3_CHKIO( rc)						if( rc == STREAM_ERR) { I3PRINTLOG(I3LOG_FATAL,  ""); return STREAM_ERR; }
#define		I3_BOUNDCHK( a, b)					I3ASSERT( ((a) >= 0) && ((a) < (b)))

#define I3_BOUNDCHK_RETURN( a, b, ...)			I3_OVERLOADING_MACRO(I3_BOUNDCHK_RETURN, a, b, __VA_ARGS__)

#define I3_BOUNDCHK_RETURN2( a, b ) \
	{ \
		if ( ((a) < 0) || ((a) >= (b)) ) \
		{ \
			I3_BOUNDCHK(a, b); \
			return; \
		} \
	}

#define I3_BOUNDCHK_RETURN3( a, b, ReturnValue) \
	{ \
		if ( ((a) < 0) || ((a) >= (b)) ) \
		{ \
			I3_BOUNDCHK(a, b); \
			return (ReturnValue); \
		} \
	}

#endif
