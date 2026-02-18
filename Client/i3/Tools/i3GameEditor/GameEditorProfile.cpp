#include "stdafx.h"
#include "GameEditorProfile.h"
#include "i3GameEditor.h"

void GameEditorProfile::OnLoad()
{
	CString Str;

	//	Windows Registry에서 읽어온다.
	{
		//	Working Directory
		Str = theApp.GetProfileString( "Option", "WorkDir", 0);
		SetWorkDir( Str.GetBuffer());

		//	RSC Generator File
		Str = theApp.GetProfileString( "Option", "RSCGen", 0);
		SetRSCFile( Str.GetBuffer());
	}
}

void GameEditorProfile::OnSave()
{
	//	Windows Registry에 저장한다.
	{
		//	Working Directory
		theApp.WriteProfileString( "Option", "WorkDir", GetWorkDir());

		//	RSC Generator File
		theApp.WriteProfileString( "Option", "RSCGen", GetRSCFile());
	}
}


