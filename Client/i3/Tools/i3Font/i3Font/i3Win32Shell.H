#if !defined( __I3_WIN32_SHELL_H)
#define __I3_WIN32_SHELL_H

#if defined( I3_WINDOWS)
#include <shlobj.h>

class i3Win32FolderSelectDialog
{
protected:
	LPMALLOC pMalloc;

public:
	char szDir[MAX_PATH];
	char szDirName[MAX_PATH];
	int  iDirImageIndex;

	BOOL Init(void);
	BOOL Release(void);

public:
	i3Win32FolderSelectDialog(void);
	virtual ~i3Win32FolderSelectDialog(void);

	char *			GetSelectedFolderPath(void)			{ return szDir; }
	char *			GetSelectedFolderName(void)			{ return szDirName; }

	//////////////////////////////////////////////////
	// Flag :
	//		BIF_BROWSEFORCOMPUTER
	//			Only return computers. If the user selects anything other than a computer,
	//			the OK button is grayed.
	//
	//		BIF_BROWSEFORPRINTER
	//			Only return printers. If the user selects anything other than a printer,
	//			the OK button is grayed. 
	//
	//		BIF_BROWSEINCLUDEFILES
	//			The browse dialog will display files as well as folders.  
	//
	//		BIF_DONTGOBELOWDOMAIN
	//			Do not include network folders below the domain level in the tree view control.  
	//
	//		BIF_EDITBOX
	//			Version 4.71. The browse dialog includes an edit control in which the user can type
	//			the name of an item.  
	//
	//		BIF_RETURNFSANCESTORS
	//			Only return file system ancestors. If the user selects anything other than a file system
	//			ancestor, the OK button is grayed.
	//
	//		BIF_RETURNONLYFSDIRS
	//			Only return file system directories. If the user selects folders that are not part of the
	//			file system, the OK button is grayed.
	//
	//		BIF_STATUSTEXT
	//			Include a status area in the dialog box. The callback function can set the status text by 
	//			sending messages to the dialog box.  
	//
	//		BIF_VALIDATE
	//			Version 4.71. If the user types an invalid name into the edit box, the browse dialog will
	//			call the application's BrowseCallbackProc with the BFFM_VALIDATEFAILED message.
	//			This flag is ignored if BIF_EDITBOX is not specified.  
	//
	////////////////////////////////////////////////////////////////////////////
	// 
	// pRoot :
	//
	//	(SHGetSpecialFolderLocation)
	//
	//		CSIDL_CONTROLS
	//			Virtual folder containing icons for the Control Panel applications.
	//
	//		CSIDL_DESKTOP
	//			Windows Desktop?virtual folder at the root of the namespace. 
	//
	//		CSIDL_DESKTOPDIRECTORY
	//			File system directory used to physically store file objects on the desktop
	//			(not to be confused with the desktop folder itself). 
	//
	//		CSIDL_DRIVES
	//			My Computer--virtual folder containing everything on the local computer: storage devices,
	//			printers, and Control Panel. The folder may also contain mapped network drives.
	//
	//		CSIDL_INTERNET
	//			Virtual folder representing the Internet. 
	//
	//		CSIDL_NETWORK
	//			Network Neighborhood Folder--virtual folder representing the top level of the network hierarchy.
	//
	//		CSIDL_PERSONAL
	//			File system directory that serves as a common repository for documents. 
	//
	//		CSIDL_PRINTERS
	//			Virtual folder containing installed printers.
	//
	// for more, reference SHGetSpecialFolderLocation function.
	//
	virtual BOOL Execute( HWND hwndOwner, LPCTSTR lpszCaption = NULL, UINT Flag = BIF_RETURNONLYFSDIRS, 
					BFFCALLBACK UserProc = NULL,
					int Root = 0);
};
#endif

#endif