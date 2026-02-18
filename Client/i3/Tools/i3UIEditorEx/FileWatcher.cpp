#include "StdAfx.h"
#include "FileWatcher.h"
#include "i3Base/string/compare/generic_is_iequal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


I3_CLASS_INSTANCE( FileWatcher); //, i3Thread );

FileWatcher::FileWatcher(void)
{
	m_szWorkingFolder[0] = 0;
	m_szScript_Scr[0] = 0;
	m_szScript_Lib[0] = 0;
}

FileWatcher::~FileWatcher(void)
{
}

BOOL FileWatcher::Create( const char* pszWorkingFolder, 
						const char* pszScript_Screen, 
						const char* pszScript_Library)
{
	i3::string_ncopy_nullpad( m_szWorkingFolder, pszWorkingFolder, MAX_PATH);
	i3::string_ncopy_nullpad( m_szScript_Scr, pszScript_Screen, MAX_PATH);
	i3::string_ncopy_nullpad( m_szScript_Lib, pszScript_Library, MAX_PATH);

	return i3Thread::Create( "FileWatcher");
}

UINT32 FileWatcher::OnRunning( void* pUserData)
{
	//Reference
	//http://www.codeguru.com/forum/archive/index.php/t-339903.html
	//http://www.codeproject.com/KB/files/FileSpyArticle.aspx?msg=3251487	//Spying A File System
	USES_CONVERSION;

	CString strDir = CString( m_szWorkingFolder);

	HANDLE hDir = CreateFile( strDir, // pointer to the file name
					FILE_LIST_DIRECTORY,                // access (read/write) mode
					FILE_SHARE_READ|FILE_SHARE_DELETE,  // share mode
					NULL,                               // security descriptor
					OPEN_EXISTING,                      // how to create
					FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
					NULL                                // file with attributes to copy
					);

	FILE_NOTIFY_INFORMATION Buffer[1024];
	DWORD BytesReturned;
	while( ReadDirectoryChangesW(	hDir,                                  // handle to directory
									&Buffer,                                    // read results buffer
									sizeof(Buffer),                                // length of buffer
									TRUE,                                 // monitoring option
									FILE_NOTIFY_CHANGE_SECURITY|
									FILE_NOTIFY_CHANGE_CREATION|
									FILE_NOTIFY_CHANGE_LAST_ACCESS|
									FILE_NOTIFY_CHANGE_LAST_WRITE|
									FILE_NOTIFY_CHANGE_SIZE|
									FILE_NOTIFY_CHANGE_ATTRIBUTES|
									FILE_NOTIFY_CHANGE_DIR_NAME|
									FILE_NOTIFY_CHANGE_FILE_NAME,             // filter conditions
									&BytesReturned,              // bytes returned
									NULL,                          // overlapped buffer
									NULL					// completion routine
									))
	{
		//CTime tm = CTime::GetCurrentTime();

		//CString helper_txt;

		//switch(Buffer[0].Action)
		//{
		//case FILE_ACTION_ADDED: helper_txt = "The file was added to the directory"; break; 
		//case FILE_ACTION_REMOVED: helper_txt = "The file was removed from the directory"; break; 
		//case FILE_ACTION_MODIFIED: helper_txt = "The file was modified. This can be a change in the time stamp or attributes."; break; 
		//case FILE_ACTION_RENAMED_OLD_NAME: helper_txt = "The file was renamed and this is the old name."; break; 
		//case FILE_ACTION_RENAMED_NEW_NAME: helper_txt = "The file was renamed and this is the new name."; break;
		//}

		int i = 0;

		do
		{
			if( Buffer[0].Action == FILE_ACTION_MODIFIED)
			{
				CString strFilename = CString(Buffer[i].FileName).Left(Buffer[i].FileNameLength / 2);

				if( i3::generic_is_iequal( m_szScript_Scr, strFilename.GetString()) )
				{
					I3TRACE("FILE_ACTION_MODIFIED: m_szScript_Scr = true\n");
					m_bModified_Scr = true;
				}
				else if( i3::generic_is_iequal( m_szScript_Lib, strFilename.GetString()) )
				{
					I3TRACE("FILE_ACTION_MODIFIED: m_szScript_Lib = true\n");
					m_bModified_Lib = true;
				}
			}

			i++;

		}while (!Buffer[i].NextEntryOffset);
	}

	return 0;
}

