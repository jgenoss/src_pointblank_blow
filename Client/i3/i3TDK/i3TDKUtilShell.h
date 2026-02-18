#if !defined( __I3_TDK_UTIL_SHELL_H)
#define __I3_TDK_UTIL_SHELL_H

#if defined( I3_WINDOWS)

#include <ShellAPI.h>

namespace i3TDK
{
	////////////////////////////////////////////////////////////////////////////////////////
	// Windows 내부 ImageList를 얻어온다.
	// 
	// SHGetImageList() 함수를 이용한다.
	//
	// type : SHIL_LARGE, SHIL_SMALL, SHIL_EXTRALARGE, SHIL_SYSSMALL, SHIL_LAST
	//
	// 반환된 CImageList Object는 App쪽에서 delete 해주어야 한다.
	I3_EXPORT_TDK
	CImageList *	GetSystemImageList( INT32 type = 1);

	/////////////////////////////////////////////////////////////////////////////////////////
	// pList에 TDK Resource의 Icon들을 ADD하고 추가된 icon들의 첫번째 인덱스를 반환한다.
	I3_EXPORT_TDK
	INT32			AddTDKImageListTo( CImageList * pList);
};
#endif

#endif
