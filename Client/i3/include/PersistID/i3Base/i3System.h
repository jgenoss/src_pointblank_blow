#if !defined( __I3_SYSTEM_H)
#define __I3_SYSTEM_H

#if defined( I3_WINDOWS)
	#pragma warning( push)
	#pragma warning( disable : 4100)
#elif defined( I3_PSP)
	#include "kernelutils.h"
	#include "scratchpad.h"
#endif

#define FFT_SUBDIR			0x00000001

typedef BOOL (*FINDFILEPROC)( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData);
typedef BOOL (*I3_TERMINATE_PROC)( void * pUserData);

typedef struct _tagTerminateInfo
{
	struct _tagTerminateInfo *	m_pNext;
	I3_TERMINATE_PROC		m_pProc;
	void *					m_pUserData;
} I3_TERMINATE_INFO;

namespace i3System
{
	inline
	void DCacheWritebackAll(void)
	{
		#if defined( I3_PSP)
		sceKernelDcacheWritebackAll();
		#endif
	}

	inline
	void DCacheWriteback( void * pStart, INT32 range)
	{
		#if defined( I3_PSP)
		sceKernelDcacheWritebackRange( (const char *) pStart, (unsigned int) range);
		#endif
	}

	inline
	void DCacheFlushAll(void)
	{
		#if defined( I3_PSP)
		sceKernelDcacheWritebackInvalidateAll();
		#endif
	}

	inline
	void DCacheFlush( void * pStart, INT32 range)
	{
		#if defined( I3_PSP)
		sceKernelDcacheWritebackInvalidateRange( (const char *) pStart, (unsigned int) range);
		#endif
	}

	I3_EXPORT_BASE void EnableInterrupt(void);
	I3_EXPORT_BASE void DisableInterrupt(void);
	
	inline
	UINT32	GetScratchPadSize(void)
	{
		#if defined( I3_PSP)
		return SCE_SCRATCHPAD_SIZE;
		//	05.06.15 by KOMET (리턴값이 없어서 컴파일시 에러 때문에)
		#else
		return 0;
		#endif
	}

	inline
	char *	GetScratchPad(void)
	{
		#if defined( I3_PSP)
		return (char *) SCE_SCRATCHPAD_ADDR;
		//	05.06.15 by KOMET (리턴값이 없어서 컴파일시 에러 때문에)
		#else
		return NULL;
		#endif
	}

	// 시스템 정보
	void				GetCPUInfo(char * pszTemp);
	void				GetGPUInfo(char * pszTemp);
	void				GetOSInfo(char * pszTemp);
	void				GetMemoryInfo(char * pszTemp);
	void				GetMultiMediaInfo(char * pszTemp);
	void				GetDXInfo(char * pszTemp);
	void				GetProcessList( char * pszTemp);

	I3_EXPORT_BASE void				SendMailBugReport( char * pszTemp, char* pszFileName = NULL );


	I3_EXPORT_BASE void	AddTerminateProc( I3_TERMINATE_PROC pProc, void * pUserData);
	I3_EXPORT_BASE void	RemoveTerminateProc( I3_TERMINATE_PROC pProc);
	I3_EXPORT_BASE void	TerminateSys(void);

	////////////////////////////////////////////////////////////////////////////////////////
	// 특정 Directory에서 조건에 맞는 파일을 검색하여 사용자-정의 CALLBACK 함수를 호출한다.
	//
	// const char * pszPath		: 파일을 검색할 Directory 경로 ( 끝에 \가 없어야 한다.)
	// const char * pszFilter	: 검색할 파일명 ( ?, * 등의 와일드 카드를 사용할 수 있다.)
	// FINDFILEPROC userproc	: 사용자-정의 CALLBACK 함수의 포인터
	// void * pUseData			: 사용자 정의 함수에 넘겨질 사용자-정의 포인터
	// UINT32 Type				: 아래의 옵션을 OR 연산으로 추가할 수 있다.
	//								  FFT_SUBDIR : 하부 디렉토리까지 검사한다.
	//
	// Note						: 사용자-정의 함수의 프로토타입은 아래와 같다.
	//
	// BOOL CALLBACK (*FINDFILEPROC)( int Level, LPCTSTR Path, WIN32_FIND_DATA *FileInfo, LPVOID UserData);
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	I3_EXPORT_BASE BOOL LookForFiles( const char * pszPath, const char * pszFilter, FINDFILEPROC userproc, void * userdata, UINT32 Type = FFT_SUBDIR);

	////////////////////////////////////////////////////////////////////////////////////////
	// 주어진 경로명이 Directory에 관한 것인지 판별한다.
	//
	// LPSTR Path	: 검사할 문자열
	//
	// Return		: 주어진 문자열이 경로명이면 TRUE,
	//                그렇지 않으면 FALSE
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE BOOL IsDirectory( char * Path);

	I3_EXPORT_BASE BOOL	IsFile( char * pszPath);

	///////////////////////////////////////////////////////////////
	// 주어진 디렉토리를 준비한다. (없으면 만든다.)
	//
	// char *Path	: 준비하려는 경로의 포인터
	//
	// Return		: 성공 = TRUE
	//				  실패 = FALSE
	//
	// Note			: Path로 주어진 경로가 없으면 Directory를 만든다. 이미 해당 디렉토리가
	//				  있으면 아무런 일도 하지 않는다.
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1	
	I3_EXPORT_BASE BOOL PrepareDirectory( char *Path);

	I3_EXPORT_BASE void	ResolvePath( const char * pszSrc, char * pszDest, INT32 len);

	I3_EXPORT_BASE void	GetExecPath( char * pszPath);

	// 파일 삭제
	I3_EXPORT_BASE BOOL RemoveFile( const char *szFileName );

	// 파일명 변경
	I3_EXPORT_BASE BOOL RenameFile( const char *szOldFileName, const char *szNewFileName );

	//////////////////////////////////////////////////////////////////////////////////////////
	// CPU 지원 여부 체크 by dfly79@zepetto.com	2006. 1. 9
	// strCPUName	: I3_CPU_MAX_MNAME_LEN 사이즈의 char[30]
	// return	: _CPU_FEATURE_MMX		MMX 지원
	//			: _CPU_FEATURE_SSE		SSE 지원
	//			: _CPU_FEATURE_SSE2		SSE2 지원
	//			: _CPU_FEATURE_3DNOW	3DNOW 지원
	I3_EXPORT_BASE INT32 CheckCPUName( char *strCPUName );
}

#endif
