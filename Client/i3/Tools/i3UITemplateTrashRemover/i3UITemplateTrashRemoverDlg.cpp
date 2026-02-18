
// i3UITemplateTrashRemoverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3UITemplateTrashRemover.h"
#include "i3UITemplateTrashRemoverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ci3UITemplateTrashRemoverDlg dialog




Ci3UITemplateTrashRemoverDlg::Ci3UITemplateTrashRemoverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3UITemplateTrashRemoverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

Ci3UITemplateTrashRemoverDlg::~Ci3UITemplateTrashRemoverDlg()
{
	I3_SAFE_RELEASE( m_pFramework);
	I3_SAFE_RELEASE( m_pViewer);
}


void Ci3UITemplateTrashRemoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_WORKDIR, m_edWorkDir);
	DDX_Control(pDX, IDC_STATIC_RENDER, m_stRender);
	DDX_Control(pDX, IDC_ED_LOG, m_edLog);
}

BEGIN_MESSAGE_MAP(Ci3UITemplateTrashRemoverDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CLEAR, &Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_WORKDIR, &Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnWorkdir)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SAVE, &Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// Ci3UITemplateTrashRemoverDlg message handlers

BOOL Ci3UITemplateTrashRemoverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CreateRenderer();

	{
		CString str;

		str = theApp.GetProfileString( LPCTSTR("Settings"), LPCTSTR("Working Path"));

		i3ResourceFile::SetWorkingFolder( (char*)str.GetString());

		m_edWorkDir.SetWindowText( str.GetString());
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Ci3UITemplateTrashRemoverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Ci3UITemplateTrashRemoverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Ci3UITemplateTrashRemoverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static BOOL _FindProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	i3StringList *	pList = (i3StringList *) pUserData;
	bool bIgnore;
	
	bIgnore =  (pFileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if( bIgnore == false)
	{
		char conv[MAX_PATH];

		sprintf( conv, "%s/%s", pszPath, pFileInfo->cFileName);

		pList->Add( conv);
	}
	
	return TRUE;
}

void Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnClear()
{
	INT32 i, j, count = 0;

	// TODO: Add your control notification handler code here
	i3StringList * pFileList = i3StringList::NewObject();

	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd ) == FALSE)
	{
		return;
	}

	i3System::LookForFiles( dlg.GetSelectedFolderPath(), "*.i3UIe", (FINDFILEPROC) _FindProc, pFileList, 0);

	i3UIManager * pUI = i3UI::getManager();
	I3ASSERT( pUI != NULL);
	

	// 대표로 하나의 Scene을 읽어서 Library를 읽어오자.
	if( pFileList->GetCount() > 0)
	{
		const char * pszName = (const char*)pFileList->GetItem( 0);

		i3UIScene * pScene = pUI->LoadUIScene( pszName);

		i3UILibrary * pLib = i3UI::getUILibrary();

		INT32 totalCount = pLib->getElementCount();

		UINT32 * pElementMap = (UINT32*) i3MemAlloc( sizeof(UINT32) * totalCount);
		i3mem::FillZero( pElementMap, sizeof( UINT32) * totalCount);

		for( i = 0;i < pFileList->GetCount(); ++i)
		{
			const char * pszName = (const char*)pFileList->GetItem( i);

			i3UIScene * pScene = pUI->LoadUIScene( pszName);
			if( pScene != NULL)
			{
				I3NOTICE( "Loaded Scene : %s\n", pszName);
				pScene->EnableScene( false);
				
				INT32 count = 0;
				i3UITemplate * pTemplate;
				
				for( j = 0; j < totalCount; ++j)
				{
					pTemplate = (i3UITemplate*)pLib->getElement(j);

					I3ASSERT( pTemplate != NULL);

					// templibrary tree와 uilibrary의 list에 존재.
					if( pTemplate->GetRefCount() > 2)
					{	// 사용된 Template의 카운트를 증가시켜주자.
						(pElementMap[ j])++;
					}
				}

				//// 무결성 검사
				//i3UIControl * pChild = (i3UIControl*) pScene->getFirstChild();
				//while( pChild != NULL)
				//{
				//	I3ASSERT( pChild->IsTypeOf( i3UIControl::GetClassMeta()));

				//	bool bCheck = false;
				//	for( j = 0; j < totalCount; j++)
				//	{
				//		pTemplate = (i3UITemplate*)pLib->getElement(j);
				//		if( pChild->getTemplate() == pTemplate)
				//		{
				//			(pElementMap[ j])++;
				//			bCheck = true;
				//			break;
				//		}
				//	}

				//	if( bCheck == false)
				//	{	// fail
				//		I3WARN( "no template!!!");
				//	}

				//	pChild = (i3UIControl*)pChild->getNext();
				//}

				pUI->RemoveScene( pScene);
			}
			else
			{
				I3NOTICE( "can't Load Scene : %s\n", pszName);
			}
		}

		I3NOTICE("=============== Start Remove Template ===============\n");
		for( i=0,j=0; i < totalCount; ++i,++j)
		{
			i3UITemplate * pTemplate = (i3UITemplate*)pLib->getElement(i);

			if( pElementMap[ i] == 0)
			{
				
				I3ASSERT( pTemplate != NULL);

				I3NOTICE( "- %s \n", pTemplate->GetName());

				//pTemplate->Release();

				//pLib->RemoveElement( pTemplate);
				//j--;
				count++;
			}
			else
			{
				I3NOTICE( "Remain - %s \n", pTemplate->GetName());
			}
		}

		I3NOTICE("=============== End Remove Template ===============\n");
		I3NOTICE("\n\n");

		I3NOTICE("=============== Results ===========================\n");
		I3NOTICE("total			: %d\n", totalCount);
		I3NOTICE("removed Count : %d\n", count);
		I3NOTICE("remain Count	: %d\n", totalCount - count);
		I3NOTICE("===================================================\n");

		pUI->RemoveAllScene();

		I3MEM_SAFE_FREE( pElementMap);
	}
}

bool Ci3UITemplateTrashRemoverDlg::SaveTemplateLibrary()
{
	return true;

	i3UILibrary * pUILib = i3UI::getUILibrary();
	if( pUILib == NULL)
	{
		MessageBox("설정된 라이브러리가 없습니다.", "알림", MB_OK);
		return false;
	}

	if( pUILib->hasName())
	{
		i3XMLFile File;
		char conv[MAX_PATH], szFull[MAX_PATH];

		i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), pUILib->GetName(), conv, sizeof( conv));
		i3String::Format( szFull, MAX_PATH -1, "%s/%s", i3ResourceFile::GetWorkingFolder(), conv);

		File.setKeyObject( pUILib);
		if( File.Save(szFull))
		{
			I3NOTICE("Template Library Saved OK \n");

			pUILib->setEdited( false);

			return true;
		}
		else
		{
			I3WARN("Couldn't save Template Library to %s\n", szFull);
		}
	}

	return false;
}

void Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnWorkdir()
{
	// TODO: Add your control notification handler code here
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd ))
	{
		i3ResourceFile::SetWorkingFolder(dlg.GetSelectedFolderPath());

		m_edWorkDir.SetWindowText( dlg.GetSelectedFolderPath());

		theApp.WriteProfileString( LPCTSTR("Settings"), LPCTSTR("Working Path"), LPCTSTR( dlg.GetSelectedFolderPath()));
	}
}

bool Ci3UITemplateTrashRemoverDlg::CreateRenderer(void)
{
	INT32 width, height;

	width	= 512;
	height	= 256;

	i3VideoInfo video;

	video.m_Width					= width;
	video.m_Height					= height;
	video.m_bFullScreen				= FALSE;
	video.m_bVSync					= TRUE;
	video.m_MultiSampling			= 0;
	video.m_MultiSamplingQuality	= 0;

	video.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;
	
	m_pViewer = i3Viewer::NewObject();
	m_pViewer->setFixedResolution( true);
	m_pViewer->Create( m_stRender.m_hWnd, &video, I3I_DEVICE_MASK_KEYBOARD | I3I_DEVICE_MASK_MOUSE);

	m_pFramework = i3ViewerFramework::NewObject();
	m_pFramework->Create( m_pViewer);

	return true;
}
int Ci3UITemplateTrashRemoverDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


//// output string
void Ci3UITemplateTrashRemoverDlg::OutputNotice( const char* pszString, bool bConcat)
{
	char szString[4096];

	i3String::Format( szString, sizeof( szString), "%s", pszString);

	// SetWindowText
	if( ::IsWindow( m_edLog.m_hWnd))
	{
		if( bConcat)
		{
			//m_msg.append( szString);
			INT32 iTotalTextLength = m_edLog.GetWindowTextLength();
			m_edLog.SetSel( iTotalTextLength, iTotalTextLength);
			m_edLog.ReplaceSel( pszString);
			m_edLog.LineScroll(1);
		}
		else
		{
			m_edLog.SetWindowText( pszString);
		}
	}
}

void Ci3UITemplateTrashRemoverDlg::OnBnClickedBtnSave()
{
	// TODO: Add your control notification handler code here
	SaveTemplateLibrary();
}
