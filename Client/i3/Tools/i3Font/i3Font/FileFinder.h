#pragma once

typedef BOOL (*FF_CALLBACK)( LPCTSTR szRootPath, LPCTSTR szFileName, void *pArg );

BOOL FindFiles( LPCTSTR szRootPath, LPCTSTR szExt, FF_CALLBACK CallBack, void *pCallBackArg, BOOL bSearchSubDir );


