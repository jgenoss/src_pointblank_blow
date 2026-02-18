#if !defined( __I3_MACRO_H)
#define __I3_MACRO_H

#define			SETBIT( a, b)							((a) |= (b))
#define			UNSETBIT( a, b)							((a) &= ~(b))
#define			TESTBIT( a, b)							((a) & (b))

#define			MIN( a, b)								( (a) > (b) ? (b) : (a))
#define			MAX( a, b)								( (a) > (b) ? (a) : (b))
#define			MINMAX( _min_, _v_, _max_ )				( MIN( MAX( (_min_), (_v_) ), (_max_) ) )

#define			LO_16( a)								((UINT16)(a & 0xFFFF))
#define			HI_16( a)								(UINT16)(( (a >> 16) & 0xFFFF))
#define			MAKE_32( l, h)							(UINT32)(( (h << 16) | (l & 0xFFFF)))

#define			I3_SAFE_DELETE(P)						if(P!=NULL){delete P; P=NULL;}
#define			I3_SAFE_DELETE_ARRAY(P)					if(P!=NULL){delete[] P; P=NULL;}
#define			I3_SAFE_RELEASE(P)						if(P!=NULL){P->Release(); P=NULL;}
#define			I3_SAFE_RELEASE_NONULL_POINTER(P)		if(P!=NULL){ P->Release(); }

#define			I3_SAFE_FREE(P)							if(P!=NULL){free(P); P=NULL;}
#define			I3_SAFE_ADDREF( p)						if((p) != NULL) { (p)->AddRef(); }

//i3Memory에서 메모리 삭제하기 
#define			I3MEM_SAFE_FREE_POINTER(P)				if(P!=NULL){i3MemFree(P); P=NULL; }
#define			I3MEM_SAFE_FREE(P)						if(P!=NULL){i3MemFree(P); P=NULL; }

#define			I3_REF_CHANGE( pVar, pNew)\
	if( (pNew) != NULL)		(pNew)->AddRef();\
	I3_SAFE_RELEASE( pVar);\
	(pVar) = (pNew)

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
	#define	__INVALID_FP						INVALID_HANDLE_VALUE
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
	#define	__INVALID_FP						-1
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
	#define __INVALID_FP						M_E_ERROR 
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
	#define __CHECKIORESULT( a)					(a == NULL)
	#define __INVALID_FP						NULL
	#define	__HANDLE_TYPE						FILE * 

	#define	__SEEK_SET							SEEK_SET
	#define	__SEEK_END							SEEK_END
	#define	__SEEK_CUR							SEEK_CUR
#endif

#define		I3_CHKIO( rc)						if( rc == STREAM_ERR) { I3FATAL( ""); return STREAM_ERR; }
#define		I3_BOUNDCHK( a, b)					I3ASSERT( ((a) >= 0) && ((a) < (b)))

#endif
