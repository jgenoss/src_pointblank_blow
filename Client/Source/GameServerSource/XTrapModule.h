#ifndef __XTRAPMODULE_H__
#define __XTRAPMODULE_H__

#include "Xtrap_S_Interface.h"
#define	CSFILE_NUM								10			// Map File Count
#define	XTRAP_TIMEOUT							300			// sec,  for FPS

class CXTrapModule
{
	HANDLE					m_hThread;
	BOOL					m_bIsRunning;

	HANDLE					m_hChangeEvnet;

	char					m_strFileName[ MAX_PATH ];
	char					m_strDirName[ MAX_PATH ];

	BOOL					_FileOpen();

public:

	INT32					m_i32XTrapMapIdx;
	UINT8					m_ui8XTrapMap[ CSFILE_NUM ][ XTRAP_CS4_BUFSIZE_MAP ] ;	// Map File Store


	CXTrapModule();
	~CXTrapModule();

	BOOL					OnCreate( char* strFileName );
	void					OnDestroy();

	void					OnRunning();

	

};

#endif