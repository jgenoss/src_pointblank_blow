#if !defined( __I3_SYSTEM_H)
#define __I3_SYSTEM_H

#include "i3CPUID.h"
#include <iosfwd>		// std::string을 위한 전방선언이 안에 들어있음..
#include <vector>		// 안타깝게도, 벡터에 대해서는 전방선언 헤더가 주어지지 않았고, std네임스페이스는 함부로 못건드리는것이 표준이라..그냥 헤더포함....

#if defined( I3_WINDOWS)
	#pragma warning( push)
	#pragma warning( disable : 4100)
#elif defined( I3_PSP)
	#include "kernelutils.h"
	#include "scratchpad.h"
#endif

#define FFT_SUBDIR			0x00000001

typedef bool (*FINDFILEPROC)( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData);
typedef bool (*I3_TERMINATE_PROC)( void * pUserData);

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct I3_TERMINATE_INFO
{
	struct I3_TERMINATE_INFO *	m_pNext;

	INT32					m_Priority;
	I3_TERMINATE_PROC		m_pProc;
	void *					m_pUserData;
} ;

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

enum I3_FPU_PRECISION
{
	I3_PC_24 = 0x00020000,	//24bits
	I3_PC_53 = 0x00010000,	//53bits
	I3_PC_64 = 0x00000000,	//64bits
};

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
		return nullptr;
		#endif
	}

	// 시스템 정보
	I3_EXPORT_BASE void		GetCPUInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetGPUInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetOSInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetMemoryInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetMultiMediaInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetDXInfo(char * pszTemp);
	I3_EXPORT_BASE void		GetProcessList( char * pszTemp);

	I3_EXPORT_BASE INT32	Benchmark_FLOPS( INT32 nTimes = 10, INT32 PassCount = 10, REAL32 runSecs = 1.0f);

	I3_EXPORT_BASE void		SendMailBugReport( char * pszTemp, char* pszFileName = nullptr);


	I3_EXPORT_BASE void	AddTerminateProc( INT32 prio, I3_TERMINATE_PROC pProc, void * pUserData);
	I3_EXPORT_BASE void	RemoveTerminateProc( I3_TERMINATE_PROC pProc);
	I3_EXPORT_BASE void	TerminateSys(void);

	I3_EXPORT_BASE void HoldDllModule(HMODULE module);

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
	// bool CALLBACK (*FINDFILEPROC)( int Level, LPCTSTR Path, WIN32_FIND_DATA *FileInfo, LPVOID UserData);
	//
	// 작성자			: 이진균
	// 작성일자			: 1999.6.1
	I3_EXPORT_BASE bool LookForFiles( const char * pszPath, const char * pszFilter, FINDFILEPROC userproc, void * userdata, UINT32 Type = FFT_SUBDIR);
	

	////////////////////////////////////////////////////////////////////////////////////////
	// 주어진 경로명이 Directory에 관한 것인지 판별한다.
	//
	// LPSTR Path	: 검사할 문자열
	//
	// Return		: 주어진 문자열이 경로명이면 true,
	//                그렇지 않으면 false
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1
	I3_EXPORT_BASE bool IsDirectory( const char * Path);

	I3_EXPORT_BASE bool	IsFile( const char * pszPath);
	I3_EXPORT_BASE bool	GetRealFilename( const char * pszPath, char * pszName);

	///////////////////////////////////////////////////////////////
	// 주어진 디렉토리를 준비한다. (없으면 만든다.)
	//
	// char *Path	: 준비하려는 경로의 포인터
	//
	// Return		: 성공 = true
	//				  실패 = false
	//
	// Note			: Path로 주어진 경로가 없으면 Directory를 만든다. 이미 해당 디렉토리가
	//				  있으면 아무런 일도 하지 않는다.
	//
	// 작성자		: 이진균
	// 작성일자		: 1999.6.1	
	I3_EXPORT_BASE bool PrepareDirectory(const char *Path);

//	I3_EXPORT_BASE void	ResolvePath( const char * pszSrc, char * pszDest, INT32 len);

//	I3_EXPORT_BASE void	GetExecPath( char * pszPath);

	/** \brief 현재 디렉토리를 뽑아낸다. 
		\param pszPath [OUT]출력 버퍼
		\param bufSize 버퍼의 크기
		\return none	*/
	I3_EXPORT_BASE void GetCurrentPath( char * pszPath, INT32 bufSize);

	/** \brief 현재 디렉토리를 설정한다.
		\param pszPath [IN]설정할 디렉토리명
		\return none	*/
	I3_EXPORT_BASE void SetCurrentPath( const char * pszPath);

	/** \brief 주어진 스트링이 절대 경로인지 검사한다.
		\param pszPath [IN]검사할 스트링
		\return 절대경로일 경우 true, 아니면 false	*/
	I3_EXPORT_BASE bool	isAbsolutePath( const char * pszPath);

	// 파일 삭제
	I3_EXPORT_BASE bool RemoveFile( const char *szFileName );

	// 파일명 변경
	I3_EXPORT_BASE bool RenameFile( const char *szOldFileName, const char *szNewFileName );

	//////////////////////////////////////////////////////////////////////////////////////////
	// CPU 지원 여부 체크 by dfly79@zepetto.com	2006. 1. 9
	// strCPUName	: I3_CPU_MAX_MNAME_LEN 사이즈의 char[30]
	// return	: _CPU_FEATURE_MMX		MMX 지원
	//			: _CPU_FEATURE_SSE		SSE 지원
	//			: _CPU_FEATURE_SSE2		SSE2 지원
	//			: _CPU_FEATURE_3DNOW	3DNOW 지원
	I3_EXPORT_BASE INT32 CheckCPUName( char *strCPUName );

	//////////////////////////////////////////////////////////////////////////////////////////
	//	sets the floating-point control word(precision only in this function)
	// 	I3_PC_24 = 0,	//24bits
	//	I3_PC_53,		//53bits
	//	I3_PC_64,		//64bits
	//	return value	//기존의 fp precision을 반환
	I3_EXPORT_BASE UINT32 SetFPUPrecision( I3_FPU_PRECISION prec);
}

#endif
