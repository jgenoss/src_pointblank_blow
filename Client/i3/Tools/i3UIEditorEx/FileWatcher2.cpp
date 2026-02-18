#include "StdAfx.h"
#include "FileWatcher2.h"
#include "i3Base/string/compare/generic_is_iequal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


I3_CLASS_INSTANCE( FileWatcher2); //, i3Thread );

FileWatcher2::FileWatcher2(void)
{
	m_szFolder[0] = 0;
	m_szFile[0] = 0;
}

FileWatcher2::~FileWatcher2(void)
{
}

BOOL FileWatcher2::Create( const char* pszFolderToWatch, const char* pszFileToWatch)
{
	i3::string_ncopy_nullpad( m_szFolder, pszFolderToWatch, MAX_PATH);

	if( pszFileToWatch != NULL)
		i3::string_ncopy_nullpad( m_szFile, pszFileToWatch, MAX_PATH);

	return i3Thread::Create( "FileWatcher2");
}

UINT32 FileWatcher2::OnRunning( void* pUserData)
{
	//Reference
	//http://www.codeguru.com/forum/archive/index.php/t-339903.html
	//http://www.codeproject.com/KB/files/FileSpyArticle.aspx?msg=3251487	//Spying A File System
	USES_CONVERSION;

	HANDLE hDir = CreateFile( m_szFolder, // pointer to the file name
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

				if( m_szFile[0] == 0)
				{
					I3TRACE("FILE_ACTION_MODIFIED: %s\n", m_szFile);
					m_bModified = true;
				}
				else if( i3::generic_is_iequal( m_szFile, strFilename.GetString()) )
				{
					I3TRACE("FILE_ACTION_MODIFIED: %s\n", m_szFile);
					m_bModified = true;
				}
			}

			i++;

		}while (!Buffer[i].NextEntryOffset);
	}

	return 0;
}

