
// i3UITemplateTrashRemover.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Ci3UITemplateTrashRemoverApp:
// See i3UITemplateTrashRemover.cpp for the implementation of this class
//

class Ci3UITemplateTrashRemoverApp : public CWinAppEx
{
public:
	Ci3UITemplateTrashRemoverApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

	static I3ERROR_HANDLE CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);
};

extern Ci3UITemplateTrashRemoverApp theApp;