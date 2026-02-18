#include "stdafx.h"
#include "i3Export.h"
#include "resource.h"
#include "OptionDialog.h"
#include "HelpDialog.h"
#include "Util.h"
#include "i3EXPLogDialog.h"
#include "MultiExportDialog.h"
#include "AdjustAllMaterialDialog.h"
#include "DlgProperty.h"
#include "DlgConvertTGA.h"

#include "OptiExpPlugService.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/contain_string.h"

#include "NXU_cooking.h"


i3Export *		g_pExport = NULL;

i3Export::i3Export(void)
{
	g_pExport = this;

	m_pPhysixCtx  = NULL;

	m_pPosData = new i3ExportPosData( this);
	m_pMsgHandler = new i3EXPFrameMsgHandler( this);

	// Default Setting
	//memset( &m_Option, 0, sizeof(m_Option));

	//m_Option.m_bExportI3S = TRUE;
	//m_Option.m_bExportSelectedOnly = false;
	//m_Option.m_bReportError = true;
	//m_Option.m_bExportAnim = false;
	//m_Option.m_bExportMesh = true;
	//m_Option.m_bExportCamera = true;
	//m_Option.m_bExportLight = true;
	//m_Option.m_bExportPhysicsGeometry = false;
	//m_Option.m_KeySampleType = i3tool::exporter::KEYSAMPLE_PICKEDONLY;
	//m_Option.m_SamplingInterval = 10;
	//m_Option.m_SelectBoneWndX = -1;
	//m_Option.m_SelectBoneWndY = -1;
	//m_Option.m_SelectBoneWndCX = -1;
	//m_Option.m_SelectBoneWndCY = -1;
	//m_Option.m_szExcludeNodeName[0] = 0;
	//m_Option.m_SkinWidth = 0.01f;
	//m_Option.m_bRotateAxis = TRUE;
	//m_Option.m_bExportShapeAsConvex = FALSE;
	//m_Option.m_bCreateRappingConvex = FALSE;
	//m_Option.m_bCreateForSimulate = FALSE;

	//cj 13.09.05 위의내용은 다음함수로 이관
	I3EXP_OPTION_INFO_SetDefault(&m_Option);
	m_enabledOptiExpPlug = false;

	m_pRoot = NULL;
//	m_GeomList.SetOnceMode( TRUE);
//	m_TexList.SetOnceMode( TRUE);			// Add함수 호출 부분이 항상 고유해서 스킵 가능..

	{
		LOGFONT log;

		memset( &log, 0, sizeof(LOGFONT));

		log.lfHeight = 12;
		log.lfCharSet = DEFAULT_CHARSET;
		log.lfOutPrecision = OUT_DEFAULT_PRECIS;
		log.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		log.lfQuality  = DEFAULT_QUALITY;
		log.lfPitchAndFamily = DEFAULT_PITCH;
		log.lfWeight = 400;
		strcpy( log.lfFaceName, "Tahoma");

		m_hFont = CreateFontIndirect( &log);
		I3ASSERT( m_hFont);
	}

	m_hwndLog = NULL;
	m_bUpdateOption = true;
	m_bResetAnimExcludeList = true;
	m_bNoAddProcessInfo = false;
	m_bNoCheckHide	= false;

	m_pAnim = NULL;
	m_pSkinBoneRoot = NULL;
	m_physExportLogFlag = EPHYSEXPLOG_CANBECONVEX_SKIP | EPHYSEXPLOG_CANBECONVEX_FAIL;
	m_loggingToi3Export = false;

	m_pCurProcessInfo = NULL;

	if(OptiExpPlug_Init(this))
		m_enabledOptiExpPlug = true;
	else
		m_enabledOptiExpPlug = false;
}

i3Export::~i3Export(void)
{
	OptiExpPlug_Finalize();

	if( m_pPosData != NULL)
	{
		delete m_pPosData ;
		m_pPosData = NULL;
	}

	if( m_pMsgHandler != NULL)
	{
		delete m_pMsgHandler;
		m_pMsgHandler = NULL;
	}

	if( m_hFont != NULL)
	{
		::DeleteObject( m_hFont);
		m_hFont = NULL;
	}

	I3_SAFE_RELEASE( m_pAnim);
}

void i3Export::UpdateOption( HWND hwnd, bool bToCtrl)
{
	HWND hwndCtrl;

	if( bToCtrl)
	{
		// Option의 내용을 Toolbar에 적용한다.

		// Path
		hwndCtrl = GetDlgItem( hwnd, IDC_ED_PATH);
		::SetWindowText( hwndCtrl, m_Option.m_szLastPath);

		// Name
		hwndCtrl = GetDlgItem( hwnd, IDC_ED_NAME);
		::SetWindowText( hwndCtrl, m_Option.m_szLastName);
	}
	else
	{
		// Toolbar의 내용을 Option Data에 반영한다.

		if( m_bUpdateOption)
		{
			// Path
			hwndCtrl = GetDlgItem( hwnd, IDC_ED_PATH);
			::GetWindowText( hwndCtrl, m_Option.m_szLastPath, sizeof(m_Option.m_szLastPath) - 1);

			// Name
			hwndCtrl = GetDlgItem( hwnd, IDC_ED_NAME);
			::GetWindowText( hwndCtrl, m_Option.m_szLastName, sizeof(m_Option.m_szLastName) - 1);
		}
	}
}

void	i3Export::_OnFileSelect(void)
{
	OPENFILENAME ofn;
	char szName[MAX_PATH];

	memset( &ofn, 0, sizeof(ofn));

	strcpy( szName, m_Option.m_szLastName);

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = getDialogHWND();
	ofn.hInstance = hInstance;
	ofn.lpstrFilter = "i3Engine Scene File (*.I3S)\x0*.I3S\x0\x0";
	ofn.lpstrFile = szName;
	ofn.lpstrInitialDir = m_Option.m_szLastPath;
	ofn.nMaxFile = sizeof(szName) - 1;
	ofn.lpstrTitle = "저장할 파일을 선택하세요.";
	ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_FORCESHOWHIDDEN | OFN_LONGNAMES | 
				OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = "I3S";

	//CDERR_DIALOGFAILURE
	if( GetOpenFileName( &ofn) == FALSE)
	{
		I3TRACE( "ERROR : %08X\n", CommDlgExtendedError());
		return;
	}

	strcpy( m_Option.m_szLastPath, szName);
	i3::extract_filename( szName, m_Option.m_szLastName);
	i3::remove_slash_filename( m_Option.m_szLastPath);
//	i3String::NormalizePath( m_Option.m_szLastPath);

	UpdateOption( getDialogHWND(), true);
}

void	i3Export::_OnFileExport(void)
{
	UpdateOption( getDialogHWND(), false);
	DoExport( m_Option.m_szLastPath, m_Option.m_szLastName);
}

void	i3Export::_OnOption(void)
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_OPTION), getDialogHWND(), _OptionDialogProc, (LPARAM)this);
}

void	i3Export::_OnHelp(void)
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_DLG_HELP), getDialogHWND(), _HelpDialogProc, (LPARAM)this);
}

void	i3Export::_OnOptiExport(void)
{
	if(!m_enabledOptiExpPlug)
		return;

	OptiExpPlug_GetHdr()->ShowExportDlg(getDialogHWND());
}

void	i3Export::_OnLogToggle(void)
{
	if( m_hwndLog == NULL)
		return;

	if( ::GetWindowLong( m_hwndLog, GWL_STYLE) & WS_VISIBLE)
	{
		::ShowWindow( m_hwndLog, SW_HIDE);
	}
	else
	{
		::ShowWindow( m_hwndLog, SW_SHOW);
	}
}

void i3Export::_OnMultiExport(void)
{
	DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_MULTI_EXPORT), getDialogHWND(), _MultiExportDialogProc, (LPARAM)NULL);
}

void i3Export::_OnPopupMenu(void)
{
	POINT pt;
	HMENU hMenu, hPopupMenu;
	UINT32 id;

	::GetCursorPos( &pt);

	hMenu = LoadMenu( hInstance, MAKEINTRESOURCE( IDR_POPUP_TOOLS));

	hPopupMenu = ::GetSubMenu( hMenu, 0);

	id = ::TrackPopupMenuEx( hPopupMenu, TPM_RETURNCMD | TPM_HORPOSANIMATION | TPM_VERPOSANIMATION | TPM_VERTICAL,
			pt.x, pt.y, m_hwndDialog, NULL);
	
	switch( id)
	{
		case ID_TOOLS_ADJUSTALLMATERIALS :
			DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_DLG_ADJUSTMATERIAL), getDialogHWND(), 
				_AdjustAllMaterialDialogProc, (LPARAM)NULL);
			break;

		case ID_TOOLS_CONVERTTEXTURETOTGAFORMAT :
			DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_DLG_CONVERT_TGA), getDialogHWND(),
				_DlgConvertTGAProc, (LPARAM) NULL);
			break;

		case ID_TOOLS_MATCHMATERIALNAMESWITHTEXTURES :
			MatchMaterialNamesWithTexture();
			break;

		case ID_TOOLS_CHECKCOLLISIONMESHES :
			CheckCollisionMeshes();
			break;

	}
}

void i3Export::OnCommand( UINT32 id)
{
	switch( id)
	{
		case IDC_BTN_PATH :			_OnFileSelect();	break;
		case IDC_BTN_EXPORT :		_OnFileExport();	break;
		case IDC_BTN_SEL_EXPORT :	_OnFileExport();	break;
		case IDC_BTN_OPTION :		_OnOption();		break;
		case IDC_BTN_LOG :			_OnLogToggle();		break;
		case IDC_BTN_MULTIEXPORT :	_OnMultiExport();	break;
		case IDC_BTN_POPUP :		_OnPopupMenu();		break;
		case IDC_BTN_HELP :			_OnHelp();			break;
		case IDC_BTN_OPTIEXPORT :   _OnOptiExport();	break;
	}
}

INT_PTR CALLBACK _ToolbarWndProc( HWND hwndDlg, UINT msg,  WPARAM wParam, LPARAM lParam)
{
	i3Export *pExport = (i3Export *)GetWindowLongPtr( hwndDlg, GWLP_USERDATA);

	if (!pExport && msg != WM_INITDIALOG ) 
		return FALSE;

	switch (msg) 
	{
		case WM_INITDIALOG: 
			{
				pExport = (i3Export *)lParam;
				SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LONG)pExport);

				pExport->UpdateOption( hwndDlg, true);
		 		return TRUE;
			}

		case WM_SHOWWINDOW :
			pExport->UpdateOption( hwndDlg, true);
			break;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			break;

		case WM_COMMAND: 
			pExport->OnCommand( wParam );
			return TRUE;

		default:
			break;
	}

	return FALSE;
}

//
//	_ToolbarNotifyCallBack
//
static void _ToolbarNotifyCallBack( void *param, NotifyInfo *info) 
{
	if( !info || !param ) return;

	i3Export *pExport = (i3Export*)param;

	switch( info->intcode ) 
	{
		case NOTIFY_SYSTEM_POST_RESET:
		case NOTIFY_SYSTEM_POST_NEW:
		case NOTIFY_FILE_POST_OPEN :
		case NOTIFY_FILE_POST_MERGE :
		case NOTIFY_POST_IMPORT :
			pExport->Reset();
			break;

		default:
			break;
	}

	switch( info->intcode)
	{
		case NOTIFY_FILE_POST_OPEN :
		case NOTIFY_FILE_POST_SAVE :
			pExport->RefreshExportName();
			break;
	}

}

DWORD i3Export::Start(void)
{
	ICUIFrame* iFrame = NULL;
	bool bNewWindow = false;

	_LoadOption();

	// Check to see if the CUI Frame Mgr already created window from .cui file
	TCHAR * pszFrameName = GetString(IDS_FRAME_NAME);

	iFrame = GetCUIFrameMgr()->GetICUIFrame( pszFrameName);
	if( iFrame != NULL)
	{
		m_hwnd = iFrame->GetHwnd();
	}
	else 
	{
		// New Window - First create the frame
		m_hwnd = CreateCUIFrameWindow( Max()->GetMAXHWnd(), GetString(IDS_FRAME_NAME), 0, 0,
							m_pPosData->GetWidth(0, 0), m_pPosData->GetHeight(0,0));
		iFrame = ::GetICUIFrame(m_hwnd);
		bNewWindow = true;
	}

	// Set the content type to a user supplied HWND
	iFrame->SetContentType( CUI_HWND);
	iFrame->SetPosType( CUI_HORIZ_DOCK | CUI_FLOATABLE | CUI_SM_HANDLES);
	iFrame->SetMenuDisplay(CUI_MENU_SHOW_ENABLED);

	// This forces the CUI system to create the window from the .cui.  The frame will
	// remain empty until this method is called.
	iFrame->SetSystemWindow(TRUE);

	// Now create our toolbar/dialog window
	m_hwndDialog = CreateDialogParam( hInstance, MAKEINTRESOURCE( IDD_TOOLBAR), 
											m_hwnd, _ToolbarWndProc, (LPARAM)this);

	// Calc width and height
	{
		RECT rect;
		GetClientRect( m_hwndDialog, &rect);

		m_pPosData->SetSize( rect.right-rect.left, rect.bottom-rect.top);
	}

	// If this is a new window, and not in the CUI file, we need to provide some intial window locations.
	if( bNewWindow ) 
	{
		RECT rectMax, rect;

		// Use the max window as a starting point for our offset
		GetWindowRect(MaxWnd(), &rectMax);

		rect.left = rectMax.left + 20;
		rect.top = rectMax.top + 90;
		rect.right = rect.left + m_pPosData->GetWidth( 0, 0);
		rect.bottom = rect.top + m_pPosData->GetHeight( 0, 0);
		GetCUIFrameMgr()->FloatCUIWindow( m_hwnd, &rect, FALSE );
	}

	// Hook up our message handler
	iFrame->InstallMsgHandler( m_pMsgHandler);

	// Move our window to make room for the dockable handle
	MoveWindow( m_hwndDialog, 10, 0, m_pPosData->getWidth(), m_pPosData->getHeight(), TRUE );

	if( iFrame->GetCurPosition() == CUI_FLOATING ) 
	{
		// Calc the diff between the client window and the window and apply our dialog size to that.
		RECT rectW, rectC;
		int iWidthOffset, iHeightOffset;

		GetWindowRect(m_hwnd, &rectW);
		GetClientRect(m_hwnd, &rectC);

		iWidthOffset = (rectW.right-rectW.left)-(rectC.right-rectC.left);
		iHeightOffset = (rectW.bottom-rectW.top)-(rectC.bottom-rectC.top);

		SetWindowPos( m_hwnd, NULL, 10, 0, 
					m_pPosData->GetWidth(0,0)+iWidthOffset, 
					m_pPosData->GetHeight(0,0)+iHeightOffset, 
					SWP_NOMOVE|SWP_NOZORDER);
	}

	// Redraw UI
	GetCUIFrameMgr()->RecalcLayout( TRUE);

	RegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_SYSTEM_POST_RESET);
	RegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_SYSTEM_POST_NEW);
	RegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_FILE_POST_OPEN);
	RegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_FILE_POST_MERGE);
	RegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_POST_IMPORT);
	

	// We are done, release the toolbar and frame handles
	ReleaseICUIFrame(iFrame);

	{
		m_hwndLog = CreateDialog( hInstance, MAKEINTRESOURCE( IDD_DIALOG_LOG), m_hwnd, _LogDialogProc);

		ShowWindow( m_hwndLog, SW_HIDE);
	}
	
	// Let our function published class know about us
	//fpBookmark.SetGUPInterface( (GUP*)this );

	{
		m_pPhysixCtx = i3PhysixContext::new_object();
		m_pPhysixCtx->Create();
	}

	// Rollup Panel
	{
		IRollupWindow * pRollup = GetCOREInterface()->GetCommandPanelRollup();

		pRollup->AppendRollup( hInstance, MAKEINTRESOURCE( IDD_ROLLUP_PROPERTIES), _DlgPropertiesProc,
						"i3Engine Properties",
						0, 0,
						ROLLUP_CAT_STANDARD);
	}

	return GUPRESULT_KEEP;
}

void i3Export::Stop(void)
{
	ICUIFrame* iFrame = GetCUIFrameMgr()->GetICUIFrame(GetString(IDS_FRAME_NAME));

	if( m_hwndLog != NULL)
	{
		::DestroyWindow( m_hwndLog);
		m_hwndLog = NULL;
	}

    if (iFrame) 
	{
        iFrame->Hide(TRUE);

        // If the toolbar was docked we need to recalc the layout
        // so the toolbar will be removed.
        if (!iFrame->IsFloating())
            GetCUIFrameMgr()->RecalcLayout(TRUE);

		ReleaseICUIFrame(iFrame);
    }

	UnRegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_SYSTEM_POST_RESET);
	UnRegisterNotification( _ToolbarNotifyCallBack, this, NOTIFY_SYSTEM_POST_NEW);

	_SaveOption();

	I3_SAFE_RELEASE( m_pPhysixCtx);
}

void i3Export::Log( INode * pNode, char * pszMsg)
{
	if(m_loggingToi3Export)
	{
		I3PRINTLOG(I3LOG_NOTICE,pszMsg);
	}
	
	SendMessage( m_hwndLog, WM_EXP_LOG, (WPARAM) pNode, (LPARAM) pszMsg);
}

void i3Export::LogEx( INode * pNode, char* fmt, ...)
{
	static char tempstr[1024];

	va_list marker;
	va_start(marker, fmt);

	vsprintf(tempstr, fmt, marker);
	va_end(marker);

	Log(pNode, tempstr);
}

void i3Export::ClearLog(void)
{
	SendMessage( m_hwndLog, WM_EXP_CLEAR, 0, 0);
}

void i3Export::_LoadOption(void)
{
	HANDLE hFile;
	Interface * ip = GetCOREInterface();
	INT32 i;
	DWORD ReadBytes;
	char szPath[MAX_PATH];

	for( i = 0; i < ip->GetPlugInEntryCount(); i++)
	{
		sprintf( szPath, "%s\\i3Export2.option", ip->GetPlugInDir( i));

		hFile = CreateFile( szPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile != INVALID_HANDLE_VALUE)
		{
			ReadFile( hFile, (LPVOID) &m_Option, sizeof(m_Option), &ReadBytes, NULL);

			CloseHandle( hFile);
			return;
		}
	}
}

void i3Export::_SaveOption(void)
{
	HANDLE hFile;
	DWORD WriteBytes;
	Interface * ip = GetCOREInterface();
	char szPath[MAX_PATH];

	if( ip->GetPlugInEntryCount() <= 0)
		return;

	sprintf( szPath, "%s\\i3Export2.option", ip->GetPlugInDir( 0));

	hFile = CreateFile( szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile( hFile, (LPCVOID) &m_Option, sizeof(m_Option), &WriteBytes, NULL);
		CloseHandle( hFile);
	}
}

#define IOSIG_I3EXPORT					0x001
#define IOSIG_I3EXPORT_VERSION			0x002
#define IOSIG_I3EXPORT_OPTIONDATA		0x003

#define I3EXP_VERSION					1

IOResult i3Export::Save( ISave *pISave )
{
	ULONG uWritten;

	pISave->BeginChunk( IOSIG_I3EXPORT );

	// Version.
	pISave->BeginChunk( IOSIG_I3EXPORT_VERSION );
	int iVer = I3EXP_VERSION;
	pISave->Write( (void*)&iVer, sizeof(int), &uWritten );
	pISave->EndChunk();

	// Option Data
	pISave->BeginChunk( IOSIG_I3EXPORT_OPTIONDATA );
	pISave->Write( (void*)&m_Option, sizeof(m_Option), &uWritten );
	pISave->EndChunk();

	pISave->EndChunk();

	return IO_OK;
}

//
//	Load
//
IOResult i3Export::Load( ILoad *pILoad )
{
	//ClearAll();

	IOResult res = IO_OK;

	pILoad->OpenChunk();

	if( pILoad->CurChunkID() == IOSIG_I3EXPORT ) 
	{
		ULONG uRead;
		int iVer = 0;
		int iNumBME = 0;

		while( IO_OK == ( res=pILoad->OpenChunk() ) ) 
		{
			switch( pILoad->CurChunkID() )  
			{
				case IOSIG_I3EXPORT_VERSION:
					pILoad->Read( (void*)&iVer, sizeof(int), &uRead );
					break;

				case IOSIG_I3EXPORT_OPTIONDATA:
					//pILoad->Read( (void*)&m_Option, sizeof(m_Option), &uRead );
					break;

			}
			pILoad->CloseChunk();
		}
	}
	pILoad->CloseChunk();

	UpdateOption( getDialogHWND());

	return IO_OK;
}

void i3Export::Reset(void)
{
	m_bNoAddProcessInfo = false;
	m_bNoCheckHide = false;

	m_ExcludeNodeList.clear();

	if( m_bResetAnimExcludeList)
	{
		m_ExcludeAnimList.clear();
	}

	removeAllProcessInfo();

	I3_SAFE_RELEASE( m_pAnim);

	m_pSkinBoneRoot = NULL;

	I3TRACE( "Resetted\n");
}

bool i3Export::_IsValidNode( INode * pNode)
{
	if( m_Option.m_szExcludeNodeName[0] != 0)
	{
		if( i3::contain_string( (const char *) pNode->GetName(), (const char *)m_Option.m_szExcludeNodeName) != -1)
		{
			return false;
		}
	}

	return true;
}

bool i3Export::_IsValidTransform( INode * pNode)
{
	if( IsBiped( pNode))
	{
		if( strstr( pNode->GetName(), "Nub") != NULL)
		{
			// Biped 끝에 붙는 Nub이라고 간주.
			return false;
		}
	}
	else if( IsFootPrint( pNode))
	{
		// Footprint Node는 무시
		return false;
	}

	return true;
}


//
// implements OptiExpPlugCallBack
//

HINSTANCE i3Export::OEPC_GetOwnerModuleInst()
{
	return hInstance;
}

void i3Export::OEPC_ClearLog()
{
	g_pExport->ClearLog();
}

bool i3Export::OEPC_DoExport(const char* path, const char* filename, i3Node** rstSceneNode)
{
	if(!filename)
		return false;

	return g_pExport->DoExport((char*)path, (char*)filename, rstSceneNode, false);
}

void* i3Export::OEPC_GetExporter()
{ 
	return (void*)this; 
}

bool i3Export::OEPC_DoExporterOptDialog()
{
	g_pExport->_OnOption();
	return true;
}

void i3Export::OEPC_GetExporterFilePathName(char* path, char* name)
{
	if(path)
		strcpy(path, g_pExport->m_Option.m_szLastPath);

	if(name)
		strcpy(name, g_pExport->m_Option.m_szLastName);
}

void i3Export::OEPC_SetExporterFilePathName(const char* path, const char* name)
{
	if(path)
		strcpy(g_pExport->m_Option.m_szLastPath, path);

	if(name)
		strcpy(g_pExport->m_Option.m_szLastName, name);
}

i3tool::exporter::pack::OPTION_INFO* i3Export::OEPC_GetExporterOpt()
{
	return &g_pExport->m_Option;
}

void i3Export::OEPC_Log(char* msg)
{
	g_pExport->Log(NULL, msg);
}

INT32 i3Export::OEPC_SetPhysXSettingToNode(INT32 dynType, bool includeOnly,
		const char* includeNode, const char* excludeNode)
{	
	Interface* ip = GetCOREInterface();

	class ExportableINodeChecker : public OptiExpPlugSer::ExportableINodeChecker
	{
	public:
		ExportableINodeChecker(i3Export* exporter) 
		{ 
			m_exporter = exporter; 

			SetNodeStr(NULL, true);
			SetNodeStr(NULL, false);
		}

		virtual bool isExportableINode(INode* node)
		{
			if(m_includeNodeOnly)
			{
				if(!OptiExpPlugSer::FindNameInText(node->GetName(), m_includeNode))
					return false;
			}
			else
			{
				if(m_lenExcludeNodeStr)
				{
					if(OptiExpPlugSer::FindNameInText(node->GetName(), m_excludeNode))
						return false;
				}
			}

			return SgBuilder::isTargetToExport(node, m_exporter);
		}

		void SetNodeStr(const char* nodeStr, bool includeStr=true)
		{
			char* dstNode = NULL;
			INT32* dstLen = NULL;

			if(includeStr)
			{
				dstNode = m_includeNode;
				dstLen = &m_lenIncludeNodeStr;
			}
			else
			{
				dstNode = m_excludeNode;
				dstLen = &m_lenExcludeNodeStr;
			}

			if(!nodeStr)
			{
				dstNode[0] = NULL;
			}
			else
				strcpy_s(dstNode, 1024, nodeStr);

			*dstLen = strlen(dstNode);
		}

		i3Export* m_exporter;

		INT32 m_lenExcludeNodeStr;
		char m_excludeNode[1024];

		INT32 m_lenIncludeNodeStr;
		char m_includeNode[1024];

		bool m_includeNodeOnly;
	};

	ExportableINodeChecker checker(g_pExport);

	checker.SetNodeStr(includeNode, true);
	checker.SetNodeStr(excludeNode, false);
	checker.m_includeNodeOnly = includeOnly;

	return OptiExpPlugSer::SetPhysXOption(ip->GetRootNode(), dynType, &checker);
}

void i3Export::OEPC_GetAniTimeRange(INT32& startTime, INT32& endTime)
{
	Interface* ip = GetCOREInterface();
	Interval intv;
	intv = ip->GetAnimRange();

	startTime = intv.Start();
	endTime = intv.End();
}

void i3Export::OEPC_SetAniTimeRange(INT32 startTime, INT32 endTime)
{
	Interface* ip = GetCOREInterface();
	Interval intv;

	intv.Set(startTime, endTime);
	ip->SetAnimRange(intv);
}


void i3Export::OEPC_Changei3DxResourceClassMode(bool toMem)
{
	if(toMem)
	{
		// i3Texture가 i3TextureMem을 생성하게
		i3TextureMem::static_meta()->set_class_meta_style_concrete(i3Texture::static_meta());
		i3TextureCubeMem::static_meta()->set_class_meta_style_concrete(i3TextureCube::static_meta());
		i3VertexArrayMem::static_meta()->set_class_meta_style_concrete(i3VertexArray::static_meta());
		i3IndexArrayMem::static_meta()->set_class_meta_style_concrete(i3IndexArray::static_meta());
	}
	else
	{
		// i3Texture가 i3TextureDX을 생성하게
		i3TextureDX::static_meta()->set_class_meta_style_concrete(i3Texture::static_meta());
		i3TextureCubeDX::static_meta()->set_class_meta_style_concrete(i3TextureCube::static_meta());
		i3VertexArrayDX::static_meta()->set_class_meta_style_concrete(i3VertexArray::static_meta());
		i3IndexArray::static_meta()->set_class_meta_style_concrete(i3IndexArray::static_meta());
	}
}

bool i3Export::OEPC_CheckDupplicatedName(void** rst)
{
	Interface* ip = GetCOREInterface();

	OptiExpPlugSer::DupplicatedNamedNodeChecker* checker 
		= OptiExpPlugSer::DupplicatedNamedNodeChecker::GetDefault();

	checker->SetExporter(g_pExport);

	if(rst)
		*rst = checker;

	return OptiExpPlugSer::CheckDupplictedOrEmptyNamedINode(ip->GetRootNode(), *checker);
}

bool i3Export::OEPC_CheckCollsionMesh(DWORD logFlag)
{
	if(!NXU::hasCookingLibrary())
	{
		LogEx(NULL, "[CheckCollisionMesh] ERR! PhysX cooking library is not avaiable.."
			"physX SDKVER=[%d]", NX_PHYSICS_SDK_VERSION);
		return false;
	}

	bool rst = false;

	TimeValue oldStart = m_timeStart;
	TimeValue oldEnd = m_timeEnd;

	m_timeStart = m_timeEnd = 0;

	DWORD oldLogFlag = m_physExportLogFlag;
	
	m_physExportLogFlag &= ~EPHYSEXPLOG_CANBECONVEX_ALLFLAG;
	m_physExportLogFlag |= logFlag;

	rst = CheckCollisionMeshes(false);

	m_timeStart = oldStart;
	m_timeEnd = oldEnd;

	m_physExportLogFlag = oldLogFlag;

	return rst;
}

void i3Export::OEPC_SetLoggingToFile(bool logging, bool* prevStatus)
{
	if(prevStatus)
		*prevStatus = m_loggingToi3Export;

	m_loggingToi3Export = logging;
}

bool i3Export::OEPC_GetPhysXCookingLibInfo(DWORD* ver)
{
	if(ver)
		*ver = 0;

	if(NXU::hasCookingLibrary())
	{
		if(ver)
			*ver = (DWORD)NX_PHYSICS_SDK_VERSION;

		return true;
	}

	return false;
}

