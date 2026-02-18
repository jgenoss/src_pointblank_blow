// UIScriptEditor.cpp : implementation file
#include "stdafx.h"

#include "ScriptWnd.h"

#include "Scintilla.h"
#include "SciLexer.h"
#include "ScriptDefines.h"
#include "FindDlg.h"
#include "GoToLineDlg.h"

// CScriptWnd

IMPLEMENT_DYNAMIC(CScriptWnd, CWnd)

CScriptWnd::CScriptWnd()
{
	m_pszBlockPath[0] = 0;
	m_pszFullPath[0] = 0;
	m_pszBlock[0] = 0;
	m_bFileOpened = false;

	m_SearchFlags = 0;
	m_SearchText[0] = 0;
	m_SearchDirection = SCI_SEARCHNEXT;
	m_bFindForward = false;
	m_nInitialPos = -1;

	m_bPreviousModifyState = false;
	m_WindowName[0] = 0;

	m_bReload = false;
}

CScriptWnd::~CScriptWnd()
{
}


BEGIN_MESSAGE_MAP(CScriptWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CScriptWnd message handlers
LRESULT CScriptWnd::SendEditor(UINT Msg, WPARAM wParam, LPARAM lParam) {
	return ::SendMessage(this->m_hWnd, Msg, wParam, lParam);
}

void CScriptWnd::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char *face) {
	SendEditor(SCI_STYLESETFORE, style, fore);
	SendEditor(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendEditor(SCI_STYLESETSIZE, style, size);
	if (face) 
		SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

void CScriptWnd::Initialize(void)
{
	SendEditor( SCI_SETLEXER, SCLEX_CPP);						// Lua lexer
	SendEditor( SCI_SETSTYLEBITS, 5 );							// Set number of style bits to use
	SendEditor( SCI_SETTABWIDTH, 4 );							// Set tab width
	//SendEditor( SCI_SETKEYWORDS, 0, (LPARAM)g_luakeywords );	// Use Lua keywords
	SendEditor( SCI_SETCODEPAGE, SC_CP_UTF8);

	SetAStyle( STYLE_DEFAULT, black, white, 10, "Courier New" );	// Set up the global default style. These attributes
																	// are used wherever no explicit choices are made.
	SendEditor( SCI_SETMARGINWIDTHN, 0, 50);

	SendEditor( SCI_SETCARETFORE, RGB( 0, 0, 0 ) );				// Set caret foreground color
	SendEditor( SCI_STYLECLEARALL );							// Set all styles

	SendEditor( SCI_SETSELBACK, TRUE, RGB( 210, 210, 210 ) );		// Set selection color
	
	for ( long i = 0; g_rgbSyntaxCpp[ i ].iItem != -1; i++ ) 
		SetAStyle( g_rgbSyntaxCpp[ i ].iItem, g_rgbSyntaxCpp[ i ].rgb );	// Set syntax colors
}

void CScriptWnd::New() 
{
	SendEditor(SCI_CLEARALL);
	SendEditor(EM_EMPTYUNDOBUFFER);
	//m_pszFullPath[0] = '\0';
	SendEditor(SCI_SETSAVEPOINT);
}

void CScriptWnd::GetRange(int start, int end, char *text) 
{
	TEXTRANGE tr;
	tr.chrg.cpMin = start;
	tr.chrg.cpMax = end;
	tr.lpstrText = text;
	::SendMessage(this->m_hWnd, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
}

void CScriptWnd::OpenFile( void)
{
	if( GetFilename() != NULL)
		_OpenFile( m_pszFullPath);

	m_pszBlock[0] = 0;
}

void CScriptWnd::SaveFile( void)
{
	if( GetFilename() != NULL)
		_SaveFile( m_pszFullPath);
}

void CScriptWnd::_OpenFile( const char* filename) 
{
	//if( i3::generic_is_iequal( filename, m_pszFullPath) )
	//	m_bFileOpened = true;
	//else
	//	m_bFileOpened = false;

	////새로 열기
	//New();
	//SendEditor(SCI_CANCEL);
	//SendEditor(SCI_SETUNDOCOLLECTION, 0);
	//
	//FILE *fp = fopen(filename, "rb");
	//if (fp) 
	//{
	//	char data[blockSize];
	//	
	//	//i3::string_ncopy_nullpad( data, "babo", sizeof( data));
	//	//SendEditor( SCI_ADDTEXT, 4, reinterpret_cast<LPARAM>(static_cast<char*>(data)));

	//	int lenFile = fread(data, 1, sizeof(data), fp);
	//	while (lenFile > 0) 
	//	{
	//		SendEditor(SCI_ADDTEXT, lenFile, reinterpret_cast<LPARAM>(static_cast<char *>(data)));
	//		lenFile = fread(data, 1, sizeof(data), fp);
	//	}
	//	fclose(fp);
	//}
	//else 
	//{
	//	char msg[MAX_PATH + 100];
	//	strcpy(msg, "Could not open file \"");
	//	strcat(msg, filename);
	//	strcat(msg, "\".");
	//	MessageBox(msg, appName, MB_OK);
	//}
	//
	//SendEditor(SCI_SETUNDOCOLLECTION, 1);
	//::SetFocus(this->m_hWnd);
	//SendEditor(EM_EMPTYUNDOBUFFER);
	//SendEditor(SCI_SETSAVEPOINT);

	//SendEditor(SCI_GOTOPOS, SendEditor(SCI_GETTEXTLENGTH));	//문서 맨 끝으로 이동

}

void CScriptWnd::AddText(char* data)
{
	SendEditor(SCI_GOTOPOS, SendEditor(SCI_GETTEXTLENGTH));

	SendEditor(SCI_ADDTEXT, strlen(data), reinterpret_cast<LPARAM>(static_cast<char *>(data)));

	SendEditor(SCI_SETUNDOCOLLECTION, 1);
	::SetFocus(this->m_hWnd);
	SendEditor(EM_EMPTYUNDOBUFFER);
	SendEditor(SCI_SETSAVEPOINT);
}

void CScriptWnd::_SaveFile( const char* filename) 
{
	/*FILE *fp = fopen(filename, "wb");

	if (fp)
	{
		char data[blockSize + 1];
		int lengthDoc = SendEditor(SCI_GETLENGTH);
		for (int i = 0; i < lengthDoc; i += blockSize)
		{
			int grabSize = lengthDoc - i;
			if (grabSize > blockSize)
				grabSize = blockSize;
			GetRange(i, i + grabSize, data);
			fwrite(data, grabSize, 1, fp);
		}
		fclose(fp);
		SendEditor(SCI_SETSAVEPOINT);
	}
	else
	{
		char msg[MAX_PATH + 100];
		strcpy(msg, "Could not save file \"");
		strcat(msg, filename);
		strcat(msg, "\".");
		MessageBox( msg, appName, MB_OK);
	}*/

	//SendEditor( SCI_EMPTYUNDOBUFFER);
}

void CScriptWnd::SetFilename( const char* filename)
{
	/*if( filename != NULL)
		i3::string_ncopy_nullpad( m_pszFullPath, filename, sizeof( m_pszFullPath));
	else
		m_pszFullPath[0] = 0;*/
}

const char* CScriptWnd::GetFilename( void)
{
	if( m_pszFullPath[0] == 0)
		return NULL;
	
	return m_pszFullPath;
}

bool CScriptWnd::IsModified( void)
{
	INT32 n = SendEditor( SCI_CANUNDO);

	if( n==0)	//nothing to undo
		return false;
	
	return true;
}

bool CScriptWnd::SaveIfModified( void)
{
	//SavePoint와 Undo Position를 비교하여 modified status를 결정한다.
	//INT32 nModifiedAfterSavePoint = SendEditor( SCI_GETMODIFY);

	//if( nModifiedAfterSavePoint > 0)
	//{
	//	//작업중이던 파일 저장
	//	char msg[MAX_PATH];
	//	i3::snprintf( msg, sizeof(msg), "%s\n%s%s\n", "Do you want to save changes to", GetFilename(), " \?");

	//	//OnCloseMiniFrame에서 2번째로 들어왔을때는 MessageBox가 안열리고 바로 다음코드로 넘어가네..뭐지

	//	INT32 nResult = MessageBox( msg, m_WindowName, MB_YESNOCANCEL);

	//	switch( nResult)
	//	{
	//	case IDYES:			Save();		return true;
	//	case IDNO:			return true;
	//	case IDCANCEL:		return false;
	//	default:			return false;
	//	}
	//}
	//
	return true;	//If returns true, it is okay to close. 
}

void CScriptWnd::OpenBlock( const char* block)
{
//	char pFolder[MAX_PATH];
////	i3String::SplitPath( m_pszFullPath, pFolder);
//	i3::extract_directoryname(m_pszFullPath, pFolder);
//	i3::snprintf( m_pszBlockPath, sizeof( m_pszBlockPath), "%s/%s.lua", pFolder, block);
//	i3::string_ncopy_nullpad( m_pszBlock, block, sizeof( m_pszBlock));
//
//	_ExtractBlock( m_pszFullPath, block, m_pszBlockPath);
//	_OpenFile( m_pszBlockPath);
	_DeleteBlockFile();		// Buffer에 내용이 있으므로 임시 파일은 지운다. 
}

void CScriptWnd::SaveBlock( const char* block)
{
	_SaveFile( m_pszBlockPath);
	_InsertBlock( m_pszBlockPath, block, m_pszFullPath);
	_DeleteBlockFile();		// 저장이 끝났으므로 임시 파일은 지운다.  
}

bool CScriptWnd::_ExtractBlock( const char* srcfile, const char* pszblock, const char* destfile)
{
	//BOOL bRes = true;

	//// Open for reading
	//CFile Src;
	//bRes = Src.Open( srcfile, CFile::modeRead);
	//if( !bRes)	return false;
	//
	////ULONGLONG SrcEnd = Src.SeekToEnd();
	////ULONGLONG SrcCur = Src.Seek( 0, CFile::begin);	//SeekToBegin
	//ULONGLONG SrcLen = Src.GetLength();

	//INT32 nBufSize = (INT32)SrcLen + 1;		//conversion ok?
	//char* pBuf = new char[nBufSize];	
	//
	//Src.Read( pBuf, nBufSize);		// returns the number of bytes transferred to the buffer
	//pBuf[nBufSize-1] = 0;

	//Src.Close();

	//// File Block
	//char szBlockStart[MAX_PATH];
	//char szBlockEnd[MAX_PATH];

	////i3::snprintf( szBlockStart, MAX_PATH, BLOCKFMT_START, pszblock);	
	////i3::snprintf( szBlockEnd, MAX_PATH, BLOCKFMT_END, pszblock);

	//INT32 nStart = (INT32)(strstr( pBuf, szBlockStart) - pBuf);	//INT32 nStart = i3String::Contain( pBuf, pszBlockStart);
	//INT32 nEnd = (INT32)(strstr( pBuf, szBlockEnd) - pBuf);		//INT32 nEnd = i3String::Contain( pBuf, pszBlockEnd);

	//// Open for writting
	//CFile Dest;
	//bRes = Dest.Open( destfile, CFile::modeCreate|CFile::modeWrite);
	//if( !bRes){
	//	delete[] pBuf;	
	//	return false;	
	//}

	//if( nStart < 0 || nEnd < 0 || nStart >= nEnd)
	//{
	//	// if tag doesn't exist or block is empty, then create an empty file
	//	//int result = MessageBox( "스크립트가 설정되어 있지 않습니다. 틀을 이용하여 생성하시겠습니까?", "", MB_YESNO);
	//	//if( result == IDYES)
	//	//{
	//	//	char pszInit[MAX_PATH];
	//	//	i3::snprintf( pszInit, sizeof( pszInit), "Use(\"%s\")\n", block);

	//	//	Dest.Write( pszInit, i3::generic_string_size(pszInit));
	//	//}
	//}
	//else	//if( nStart < nEnd)
	//{
	//	nStart = nStart + i3::generic_string_size( szBlockStart) + 1;		// because '\n' comes after
	//	nEnd = nEnd - 1;	// because '\n' comes before
	//	
	//	if( nEnd - nStart > 0)
	//	{
	//		char* pStart = pBuf + nStart;
	//		Dest.Write( pStart, nEnd - nStart);
	//	}
	//}

	//Dest.Close();
	//delete[] pBuf;
	return true;
}

//bool CScriptWnd::_FindBlock( const char* pszfile, const char* pszblock)
//{
//	BOOL bRes = true;
//
//	CFile file;
//	BOOL bRes = file.Open( pszfile, CFile::modeRead);
//	if( !bRes) return false;
//
//	ULONGLONG len = file.GetL
//}

bool CScriptWnd::_InsertBlock( const char* srcfile, const char* pszblock, const char* destfile)
{
	//BOOL bRes = true;

	//// Open for reading
	//CFile Src;
	//bRes = Src.Open( srcfile, CFile::modeRead);
	//if( !bRes)	return false;

	//ULONGLONG SrcLen = Src.GetLength();

	//INT32 nSrcSize = (INT32)SrcLen;		//conversion ok?
	//char* pSrc = new char[nSrcSize];	
	//Src.Read( pSrc, nSrcSize);		// returns the number of bytes transferred to the buffer

	//Src.Close();

	//// Open for writting
	//CFile Dest;
	//bRes = Dest.Open( destfile, CFile::modeReadWrite);		// ?
	//if( !bRes)
	//{
	//	delete[] pSrc;
	//	return false;
	//}

	//ULONGLONG DestLen = Dest.GetLength();

	//INT32 nDestSize = (INT32)DestLen;
	//char* pDest = new char[nDestSize];
	//Dest.Read( pDest, nDestSize);

	//// find tags
	//char szBlockStart[MAX_PATH];
	//char szBlockEnd[MAX_PATH];

	//i3::snprintf( szBlockStart, MAX_PATH, BLOCKFMT_START, pszblock);
	//
	//i3::snprintf( szBlockEnd, MAX_PATH,  BLOCKFMT_END, pszblock);

	//INT32 nStart = (INT32)(strstr( pDest, szBlockStart) - pDest);
	//INT32 nEnd = (INT32)(strstr( pDest, szBlockEnd) - pDest);

	//if( nStart < 0 || nEnd < 0)		// no tags : 
	//{
	//	const char* pLine = "\n---------------------------------------\n";
	//	Dest.SeekToEnd();
	//	
	//	//search insert position
	//	/*CWnd* pMain = AfxGetMainWnd();
	//	CUIPaneScreen* pScreenPane = ((CMainFrame*)pMain)->getScreenPane();

	//	StringList strList;
	//	pScreenPane->GetItemsNameList( &strList);*/
	//	
	//	//char pBuf[MAX_PATH];
	//	//i3::snprintf( pBuf, sizeof( pBuf), BLOCKFMT, pszBlock, pszBlock);
	//	//Dest.Write( pBuf, i3::generic_string_size( pBuf));
	//	
	//	Dest.Write( pLine, i3::generic_string_size(pLine));
	//	Dest.Write( szBlockStart, i3::generic_string_size( szBlockStart));
	//	Dest.Write( "\n", 1);
	//	Dest.Write( pSrc, nSrcSize);
	//	Dest.Write( "\n", 1);
	//	Dest.Write( szBlockEnd, i3::generic_string_size( szBlockEnd));
	//}
	//else
	//{
	//	nStart = nStart + i3::generic_string_size( szBlockStart) + 1;		// because '\n' comes after
	//	nEnd = nEnd - 1;	// because '\n' comes before

	//	// temporarily saves the text that comes after "End" tag
	//	INT32 nTempSize = nDestSize - nEnd;
	//	char* pTemp = new char[nTempSize];

	//	Dest.Seek( nEnd, CFile::begin);
	//	Dest.Read( pTemp, nTempSize);		
	//	
	//	Dest.Seek( nStart, CFile::begin);
	//	Dest.Write( pSrc, nSrcSize);
	//	Dest.Write( pTemp, nTempSize);		// writes the temp buffer back

	//	delete[] pTemp;

	//	Dest.SetLength( Dest.GetPosition());
	//}

	//Dest.Close();
	//delete[] pSrc;
	//delete[] pDest;
	
	return true;
}

void CScriptWnd::Save( void)
{
	//CWnd* pParent = GetParent();
	//I3ASSERT( pParent != NULL && pParent->IsKindOf( RUNTIME_CLASS( CScriptPane)));
	//CScriptPane* pPane = (CScriptPane*)pParent;
	//CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

	////////////////////////////////////////////
	////pMainFrm->SuspendFileWatcher();
	//pMainFrm->TerminateFileWatcher();		// 아예 죽였다 살려야할듯;;

	//if( m_bFileOpened)
	//	SaveFile();
	//else
	//	SaveBlock( m_pszBlock);
	//
	//pMainFrm->OnModifyScript( this, false);
	//pPane->OnSavingScript();

	////pMainFrm->ResumeFileWatcher();
	//pMainFrm->CreateFileWatcher();
	////////////////////////////////////////////

	//SendEditor( SCI_SETSAVEPOINT);	//http://www.scintilla.org/ScintillaDoc.html#SCI_SETSAVEPOINT
}

void CScriptWnd::_DeleteBlockFile( void)
{
	/*try
	{
		CFile::Remove(m_pszBlockPath);
	}
	catch (CFileException* pEx)
	{
	   I3TRACE( "File %20s cannot be removed\n", m_pszBlockPath);
	   pEx->Delete();
	}*/
}

BOOL CScriptWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if( GetKeyState( VK_CONTROL) & 0x8000)
			{
				switch( pMsg->wParam)
				{
				case 'C':	SendEditor( SCI_COPY);		return true;
				case 'V':	SendEditor( SCI_PASTE);		return true;
				case 'X':	SendEditor( SCI_CUT);		break;
				case 'Z':	SendEditor( SCI_UNDO);		break;
				case 'Y':	SendEditor( SCI_REDO);		break;		// 
				case 'S':	Save();						return TRUE;	// !!
				case 'G':	GoToLine();					return TRUE;
				case 'F':	Find();						return TRUE;
				case VK_F7:	
					{
						CWnd* pMain = AfxGetMainWnd();
						//((CMainFrame*)pMain)->OnCompileScript();
						return TRUE;
					}
				case 'Q':	TestFunc();		return TRUE;
				//no need for 'A'
				default:	break;
				}
			}
			else if( GetKeyState( VK_SHIFT) & 0x8000)
			{
				switch( pMsg->wParam)
				{
				case VK_F3:		OnKeyDownShiftF3();		return TRUE;
				default: break;
				}
			}
			else
			{
				switch( pMsg->wParam)
				{
				case VK_F3:		OnKeyDownF3();		return TRUE;
				default:
					break;
				}
			}
		}
		break;
	}

	// Modified Check 
// 	bool bModified = IsModified();
// 	if( m_bPreviousModifyState != bModified)
// 	{
// 		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
// 		pMainFrm->OnModifyScript( this, bModified);
// 		m_bPreviousModifyState = bModified;
// 	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CScriptWnd::RenameBlock( const char* oldname, const char* newname)
{
	//if( m_pszFullPath[0] != 0)
	//{
	//	BOOL bRes = true;

	//	// Open for read & write
	//	CFile Src;
	//	bRes = Src.Open( m_pszFullPath, CFile::modeReadWrite);
	//	if( !bRes)
	//	{
	//		I3TRACE("Failed To Open %s", m_pszFullPath);
	//		return;
	//	}
	//	
	//	ULONGLONG SrcLen = Src.GetLength();

	//	INT32 nBufSize = (INT32)SrcLen + 1;		//conversion ok?
	//	char* pBuf = new char[nBufSize];	
	//	Src.Read( pBuf, nBufSize-1);				
	//	pBuf[nBufSize-1] = 0;

	//	// Old Block Tag
	//	char szOldBlockStart[MAX_PATH];
	//	char szOldBlockEnd[MAX_PATH];

	//	i3::snprintf( szOldBlockStart, MAX_PATH, BLOCKFMT_START, oldname);	
	//	i3::snprintf( szOldBlockEnd, MAX_PATH, BLOCKFMT_END, oldname);

	//	INT32 nLenOldStart = i3::generic_string_size( szOldBlockStart);
	//	INT32 nLenOldEnd = i3::generic_string_size( szOldBlockEnd);

	//	// find
	//	INT32 nStart = (INT32)(strstr( pBuf, szOldBlockStart) - pBuf);	
	//	INT32 nEnd = (INT32)(strstr( pBuf, szOldBlockEnd) - pBuf);		

	//	if( nStart > -1 && nEnd > -1 && nEnd > nStart)
	//	{
	//		// New Block Tag
	//		char szNewBlockStart[MAX_PATH];
	//		char szNewBlockEnd[MAX_PATH];

	//		i3::snprintf( szNewBlockStart, MAX_PATH, BLOCKFMT_START, newname);	
	//		i3::snprintf( szNewBlockEnd, MAX_PATH, BLOCKFMT_END, newname);

	//		INT32 nLenNewStart = i3::generic_string_size( szNewBlockStart);
	//		INT32 nLenNewEnd = i3::generic_string_size( szNewBlockEnd);

	//		// replace
	//		INT32 nNewBufSize = nBufSize - nLenOldStart - nLenOldEnd + nLenNewStart + nLenNewEnd;
	//		char* pNewBuf = new char[nNewBufSize];
	//		pNewBuf[nNewBufSize-1] = 0;

	//		if( nNewBufSize < nBufSize)
	//		{
	//			// replace first
	//		//	bool bResult = i3String::NReplace( pBuf, nStart, nStart+nLenOldStart-1, nBufSize, szNewBlockStart, nLenNewStart);
	//		//	I3ASSERT( bResult );
	//			i3::generic_nreplace_range(pBuf, nBufSize-1, pBuf + nStart, pBuf + nStart+nLenOldStart-1, szNewBlockStart, szNewBlockStart+nLenNewStart);

	//			nEnd = nEnd - nLenOldStart + nLenNewStart;
	//		//	bResult = i3String::NReplace( pBuf, nEnd, nEnd+nLenOldEnd-1, nBufSize, szNewBlockEnd, nLenNewEnd);
	//			i3::generic_nreplace_range(pBuf, nBufSize-1, pBuf + nEnd, pBuf + nEnd+nLenOldEnd-1, szNewBlockEnd, szNewBlockEnd+nLenNewEnd);

	//		//	I3ASSERT( bResult );

	//			i3::string_ncopy_nullpad( pNewBuf, pBuf, nNewBufSize);
	//		}
	//		else
	//		{
	//			// copy first				
	//			i3::string_ncopy_nullpad( pNewBuf, pBuf, nNewBufSize);

	//		//	bool bResult = i3String::NReplace( pNewBuf, nStart, nStart+nLenOldStart-1, nNewBufSize, szNewBlockStart, nLenNewStart);
	//		//	I3ASSERT( bResult );
	//			i3::generic_nreplace_range(pNewBuf, nNewBufSize-1, pNewBuf+nStart, pNewBuf+nStart+nLenOldStart-1, szNewBlockStart, szNewBlockStart+nLenNewStart);
	//		//

	//			nEnd = nEnd - nLenOldStart + nLenNewStart;
	//		//	bResult = i3String::NReplace( pNewBuf, nEnd, nEnd+nLenOldEnd-1, nNewBufSize, szNewBlockEnd, nLenNewEnd);
	//			i3::generic_nreplace_range(pNewBuf, nNewBufSize-1, pNewBuf+nEnd, pNewBuf+nEnd+nLenOldEnd-1, szNewBlockEnd, szNewBlockEnd+nLenNewEnd);
	//		//	I3ASSERT( bResult );
	//		}
	//		
	//		Src.SeekToBegin();
	//		Src.Write( pNewBuf, nNewBufSize-1);	
	//		Src.SetLength( Src.GetPosition());		// 'NUL';;

	//		Src.Close();
	//		delete[] pNewBuf;
	//	}
	//	else
	//	{
	//		Src.Close();
	//	}
	//	
	//	
	//	delete[] pBuf;

	//	//reopen
	//	if( m_bFileOpened)
	//	{
	//		OpenFile();
	//	}
	//	else
	//	{
	//		if( i3::generic_is_iequal( oldname, m_pszBlock))
	//		{
	//			OpenBlock( newname);
	//		}
	//		else
	//		{
	//			OpenBlock( m_pszBlock);
	//		}
	//	}
	//}
}

void CScriptWnd::GoToLine( void)
{
	CGoToLineDlg dlg;

	char pszStatic[64];
	char pszEdit[64];

	INT32 nLen = SendEditor( SCI_GETLINECOUNT);
	INT32 nPos = SendEditor( SCI_LINEFROMPOSITION, SendEditor(SCI_GETCURRENTPOS)) + 1;

	i3::snprintf( pszStatic, sizeof( pszStatic), "Line Number (1-%d):", nLen);
	i3::snprintf( pszEdit, sizeof( pszEdit), "%d", nPos);

	dlg.SetStaticText( pszStatic);
	dlg.SetEditText( pszEdit);

	if( dlg.DoModal() == IDOK)
	{
		nPos = i3::atoi( dlg.GetEditText()) - 1;
		SendEditor(SCI_GOTOLINE, nPos);
	}

	::SetFocus(this->m_hWnd);
}

void CScriptWnd::Find( void)
{
	CFindDlg dlg(this);
	 //Get Selected Text
	INT32 nBufSize = SendEditor( SCI_GETSELTEXT, 0, 0);
	char* pBuf = new char[nBufSize];
	SendEditor( SCI_GETSELTEXT, 0, (LPARAM)pBuf);

	dlg.SetFindWhat( (const char*)pBuf);

	if( (m_SearchFlags & SCFIND_MATCHCASE) != 0)
		dlg.SetMatchCase(true);
	else
		dlg.SetMatchCase(false);

	if( (m_SearchFlags & SCFIND_WHOLEWORD) != 0)
		dlg.SetMatchWholeWord(true);
	else
		dlg.SetMatchWholeWord(false);

	if( m_SearchDirection == SCI_SEARCHPREV)
		dlg.SetSearchUp(true);
	else
		dlg.SetSearchUp(false);


	if( dlg.DoModal() == IDCANCEL)
	{
		// Settings
		
	}

	::SetFocus(this->m_hWnd);
}

// 순환검색
// 문서의 아랫방향으로 검색
void CScriptWnd::FindForward( void)
{
 	if(strcmp( m_SearchText, "") == 0)
 		return;
 
 	INT32 nPos = SendEditor(SCI_GETCURRENTPOS);
 	INT32 nFirstLine = SendEditor( SCI_GETFIRSTVISIBLELINE);
 
 	//INT32 nStart = SendEditor( SCI_GETSELECTIONSTART);
 	INT32 nEnd = SendEditor( SCI_GETSELECTIONEND);
 	SendEditor( SCI_GOTOPOS, nEnd);
 	SendEditor( SCI_SEARCHANCHOR);
 	INT32 nFind = SendEditor( SCI_SEARCHNEXT, m_SearchFlags, (LPARAM)m_SearchText);
 
 	if( nFind == -1)
 	{
 		SendEditor( SCI_GOTOPOS, 0);
 		SendEditor( SCI_SEARCHANCHOR);
 		nFind = SendEditor( SCI_SEARCHNEXT, m_SearchFlags, (LPARAM)m_SearchText);
 
 		if( nFind == -1)
 		{
 			SendEditor( SCI_SETFIRSTVISIBLELINE, nFirstLine);
 			SendEditor( SCI_GOTOPOS, nPos);
 
 			char msg[MAX_PATH];
 			i3::snprintf( msg, sizeof( msg), "Cannot find the string:\"%s\"", m_SearchText);
 			MessageBox( msg, "i3UIEditor", MB_OK);
 			
 			::SetFocus(this->m_hWnd);
 			return;
 		}
 	}
 
 	INT32 nLine = SendEditor( SCI_LINEFROMPOSITION, nFind);
 	INT32 maxLine = SendEditor( SCI_GETLINECOUNT);
 	INT32 linesOnScreen = SendEditor( SCI_LINESONSCREEN);
 
 	INT32 newFirstLine = MINMAX( 0, nLine - linesOnScreen / 2, maxLine - linesOnScreen);
 
 	SendEditor( SCI_SETFIRSTVISIBLELINE, newFirstLine);
 
 	if( !m_bFindForward)
 	{
 		m_nInitialPos = nFind;
 		m_bFindForward = true;
 	}
 	else if( m_nInitialPos == nFind)
 	{
 		MessageBox( "Reached the starting point of the search", "i3UIEditor", MB_OK);
 		::SetFocus(this->m_hWnd);
 	}
}

// 문서의 윗방향으로 검색
void CScriptWnd::FindBackward( void)
{
	if( i3::generic_is_equal( m_SearchText, "") )
		return;

	INT32 nPos = SendEditor(SCI_GETCURRENTPOS);
	INT32 nFirstLine = SendEditor( SCI_GETFIRSTVISIBLELINE);

	SendEditor( SCI_SEARCHANCHOR);
	INT32 nFind = SendEditor( SCI_SEARCHPREV, m_SearchFlags, (LPARAM)m_SearchText);

	if( nFind == -1)
	{
		SendEditor(SCI_GOTOPOS, SendEditor(SCI_GETTEXTLENGTH));		//문서 맨 끝으로 이동
		SendEditor( SCI_SEARCHANCHOR);
		nFind = SendEditor( SCI_SEARCHPREV, m_SearchFlags, (LPARAM)m_SearchText);

		if( nFind == -1)
		{
			SendEditor( SCI_SETFIRSTVISIBLELINE, nFirstLine);
			SendEditor( SCI_GOTOPOS, nPos);

			char msg[MAX_PATH];
			i3::snprintf( msg, sizeof( msg), "Cannot find the string:\"%s\"", m_SearchText);
			MessageBox( msg, "i3UIEditor", MB_OK);
			
			::SetFocus(this->m_hWnd);
			return;
		}
	}

	INT32 nLine = SendEditor( SCI_LINEFROMPOSITION, nFind);
	INT32 maxLine = SendEditor( SCI_GETLINECOUNT);
	INT32 linesOnScreen = SendEditor( SCI_LINESONSCREEN);

	INT32 newFirstLine = MINMAX( 0, nLine - linesOnScreen / 2, maxLine - linesOnScreen);

	SendEditor( SCI_SETFIRSTVISIBLELINE, newFirstLine);

	
	if( m_bFindForward)
	{
		m_nInitialPos = nFind;
		m_bFindForward = false;
	}
	else if( m_nInitialPos == nFind)
	{
		MessageBox( "Reached the starting point of the search", "i3UIEditor", MB_OK);
		::SetFocus(this->m_hWnd);
	}
}

void CScriptWnd::OnKeyDownF3( void)
{
	if( m_SearchDirection == SCI_SEARCHNEXT)
		FindForward();
	else
		FindBackward();
}

void CScriptWnd::OnKeyDownShiftF3( void)
{
	if( m_SearchDirection == SCI_SEARCHNEXT)
		FindBackward();
	else
		FindForward();
}

void CScriptWnd::setWindowName( const char* pszName)
{
	//윈도우의 이름은 윈도우가 생성될 때 한 번만 설정됩니다.
	//I3ASSERT( m_WindowName[0] == 0);

	//i3::string_ncopy_nullpad( m_WindowName, pszName, sizeof( m_WindowName));
}

void CScriptWnd::getWindowName( char* buf, INT32 nBufSize)
{
	//I3ASSERT( m_WindowName[0] != 0);

	//i3::string_ncopy_nullpad( buf, m_WindowName, nBufSize);
}

void CScriptWnd::ReOpen( void)
{
	if( m_bFileOpened)
	{
		//I3TRACE("File Reopened!\n");
		OpenFile();
	}
	else
	{
		//I3TRACE("Block Reopened!\n");
		OpenBlock( m_pszBlock);
	}
}

int CScriptWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void CScriptWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
}


void CScriptWnd::OnContextMenu( CWnd* pWnd, CPoint pos)
{
	//CMenu Menu;
	//CMenu* pPopup = NULL;
	//UINT32 cmd = 0; 

	//Menu.LoadMenu( IDR_MENU_SCRIPT);
	//pPopup = Menu.GetSubMenu( 0);

	//if( pPopup != NULL)
	//{
	//	cmd = pPopup ->TrackPopupMenuEx(  TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
	//				pos.x, pos.y, (CWnd *) this, NULL);
	//}

	//switch( cmd)
	//{
	//case ID__INSERTEVENTHANDLER:	OnInsertEventHandler(); break;
	//case ID__FIND:					Find();	break;
	//case ID__GOTOLINE:				GoToLine();	break;
	//case ID__COPY:					SendEditor( SCI_COPY);	break;
	//case ID__CUT:					SendEditor( SCI_CUT);	break;
	//case ID__PASTE:					SendEditor( SCI_PASTE);	break;
	//default:	break;
	//}
}

void CScriptWnd::OnInsertEventHandler( void)
{
	////CWnd* pParent = GetParent();
	////I3ASSERT( pParent != NULL && pParent->IsKindOf( RUNTIME_CLASS( CScriptPane)));
	//CScriptPane* pPane = (CScriptPane*)GetParent();
	//bool bScreen = pPane->isBelongToScreen();
	//char szBlockName[MAX_PATH] = {0};

	//if( !m_bFileOpened)
	//{
	//	i3::string_ncopy_nullpad( szBlockName, m_pszBlock, MAX_PATH);
	//}
	//else
	//{
	//	INT32 nCurPos = SendEditor( SCI_GETCURRENTPOS);

	//	INT32 nLen = getTextLen();
	//	char* pBuf = new char[nLen+1];

	//	SendEditor( SCI_GETTEXT, nLen+1, (LPARAM)pBuf);

	//	ScriptManager::isInsideBlock( pBuf, nCurPos, szBlockName);

	//	delete[] pBuf;
	//}
	//
	//ScriptManager::InsertEventHandler( szBlockName, bScreen, pPane);
}


void CScriptWnd::TestFunc( void)
{

}

INT32 CScriptWnd::getCurrentPos( void)
{
	return SendEditor( SCI_GETCURRENTPOS);
}

void CScriptWnd::SetCurrentPosTo( INT32 nPos)
{
	SendEditor( SCI_GOTOPOS, nPos);
}

void CScriptWnd::AppendText( const char* pszText)
{
	//SendEditor( SCI_APPENDTEXT, i3::generic_string_size( pszText), reinterpret_cast<LPARAM>(pszText));
}

void CScriptWnd::InsertText( const char* pszText, INT32 nPos)
{
	//SendEditor( SCI_INSERTTEXT, MINMAX( 0, nPos, SendEditor( SCI_GETLENGTH)), (LPARAM)pszText);
}

INT32 CScriptWnd::getTextLen( void)
{
	return SendEditor( SCI_GETTEXTLENGTH);
}

void CScriptWnd::getTextBuf( char* pBuf, INT32 nSize)
{
	INT32 nLen = getTextLen();
	char* pText = new char[nLen+1];

	SendEditor( SCI_GETTEXT, nLen+1, (LPARAM)pText);

	INT32 i=0; 
	while( pText[i] != 0 && i < nSize-1)
	{
		pBuf[i] = pText[i];
		i++;
	}

	pBuf[i] = 0;

	delete[] pText;
}