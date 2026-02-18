
// NationEditorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "NationEditor.h"
#include "NationEditorDlg.h"

#include "OrderDialog.h"
#include "FileUtil.h"

#include "CopyPefValueDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CNationEditorDlg 대화 상자




CNationEditorDlg::CNationEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNationEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bControlKeyDown = false;
}

void CNationEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListBox_Nation);
	DDX_Control(pDX, IDC_LIST3, m_ListBox_Pef);
	DDX_Control(pDX, IDC_CHECK1, m_CheckCtrl_BakFile);
	DDX_Control(pDX, IDC_LIST1, m_OperationCtrl);
}

BEGIN_MESSAGE_MAP(CNationEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CNationEditorDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST2, &CNationEditorDlg::OnLbnSelchangeListNation)
	ON_BN_CLICKED(IDC_CHECK1, &CNationEditorDlg::OnBnClickedCheckBakFile)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CNationEditorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CNationEditorDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CNationEditorDlg 메시지 처리기

BOOL CNationEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	m_OperationCtrl.SetExtendedStyle(
		LVS_EX_FULLROWSELECT | 
		LVS_EX_GRIDLINES | 
		LVS_EX_TWOCLICKACTIVATE );

	m_OperationCtrl.NotifyHeaderColumn();

	m_bCreateBakFile = false;
	m_CheckCtrl_BakFile.SetCheck( m_bCreateBakFile );

	DragAcceptFiles( TRUE);

	if( !m_CurrentFilePath.IsEmpty() )
	{
		if( !LoadList(m_CurrentFilePath) )
		{
			AfxMessageBox( "It is not Nation List File!" );
		}
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CNationEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNationEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNationEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CNationEditorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine( cmdInfo);

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		m_CurrentFilePath = cmdInfo.m_strFileName;
	}

	return 0;
}

BOOL CNationEditorDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	ClearPefContextBank();

	return CDialog::DestroyWindow();
}

void CNationEditorDlg::ClearPefContextBank()
{
	size_t size = m_PefContextBank.size();
	for(size_t i=0; i<size; ++i)
	{
		m_PefContextBank[i]->pRegistrySet->Release();
		delete m_PefContextBank[i];
	}
	m_PefContextBank.clear();
}



static CNationEditorDlg *pParent = NULL;

void OnStartProc()
{
	CHListBox* pListCtrl = pParent->GetPefCtrl();
	pListCtrl->ResetContent();
}

void OnEndProc()
{
	CHListBox* pListCtrl = pParent->GetPefCtrl();

	size_t size = pListCtrl->GetCount();
	CString CountStr;
	CountStr.Format("Pef File List (%d)", size);
	pParent->SetDlgItemText( IDC_ST_PEF, CountStr);
}

void OnFileProc(const WIN32_FIND_DATA& Data, const String & FileName)
{
	size_t Index = FileName.rfind( "pef" );
	if( Index == String::npos )
		return;

	if( FileName.substr( Index ) != "pef" )
		return;

	i3RegistrySet_Impl  *pRegistrySet = i3RegistrySet_Impl::new_object();
	pRegistrySet->SetListCtrlPtr( pParent );

	if( pRegistrySet->LoadRegistrySet( FileName.c_str() )  == STREAM_ERR )
		return;

	if( pParent->IsMultiPef( pRegistrySet->getRoot() ) )
	{
		pParent->AddPefContext(Data.cFileName,
			FileName, pRegistrySet);
	}
	else
	{
		pRegistrySet->Release();
	}
}

bool CNationEditorDlg::IsMultiPef(i3RegKey * pRoot) const
{
	if( pRoot ->getNodeStyle() == I3REG_STYLE_ROOT )
	{
		if( pRoot->getChildCount() == 0 )
			return false;

		i3RegKey* RegKey = (i3RegKey*)pRoot ->getChild( 0 );
		if( RegKey == NULL)
			return false;

		if( RegKey->GetDataCount() == 0 )
			return false;

		i3RegData* pRegData = RegKey->GetData( 0 );
		if( pRegData == NULL)
			return false;

		return true;
	}

	return false;
}

void CNationEditorDlg::AddPefContext( const String & Name, const String & Path, 
									 i3RegistrySet_Impl * pRegistrySet)
{
	PefContext * Context = new PefContext;
	
	Context->Name = Name;
	Context->Path = Path;
	Context->pRegistrySet = pRegistrySet;

	m_PefContextBank.push_back( Context );

	m_ListBox_Pef.AppendString( Name.c_str() );
}

size_t CNationEditorDlg::GetNationIndex(const String & Nation) const
{
	size_t size = m_NationBank.size();

	for(size_t i = 0; i <size; ++i)
	{
		if( Nation == m_NationBank[i] )
			return i;
	}

	return (size_t)-1;
}

void CNationEditorDlg::AddListNationString(const CString & Nation)
{
	m_NationBank.push_back( Nation.GetString() );
	m_ListBox_Nation.AppendString( Nation );
}

void CNationEditorDlg::DelListNationString(const CString & Nation)
{
	size_t index = GetNationIndex( Nation.GetString() );

	if( index == (size_t)-1 )
		return;

	m_NationBank.erase( m_NationBank.begin() + index );
	m_ListBox_Nation.DeleteString( index );
}

bool CNationEditorDlg::LoadList(const CString & FullPath)
{
	int Index = FullPath.Find( "Nation.nlf" );

	if( Index == -1 )
		return false;

	ClearPefContextBank();
	m_NationBank.clear();

	m_CurrentFilePath = FullPath;
	m_CurrentPath = FullPath.Mid( 0, Index-1);

	LoadNationList();
	LoadPefList();

	m_OperationCtrl.NotifyHeaderColumn();

	return true;
}

void CNationEditorDlg::LoadPefList()
{
	pParent = this;

	FileUtil::SetProc( OnStartProc, OnEndProc, OnFileProc);

	FileUtil::Spread( m_CurrentPath.GetString() );
}

void CNationEditorDlg::SavePef(PefContext* pPefContex)
{
	String PefPath, XmlPath, PefBakPath, XmlBakPath;

	//Pef 저장
	PefPath = pPefContex->Path;
	XmlPath = PefPath.substr( 0, PefPath.rfind(".") );
	XmlPath.append(".i3RegXML");

	if( m_bCreateBakFile )
	{
		PefBakPath = PefPath;
		PefBakPath.append(".bak" );

		FileUtil::CopyFile(PefPath, PefBakPath);

		XmlBakPath = XmlPath;
		XmlBakPath.append(".bak" );

		FileUtil::CopyFile(XmlPath, XmlBakPath);
	}

	pPefContex->pRegistrySet->SaveRegistrySet(
		PefPath.c_str(), REGISTRY_FILE_BINARY );

	pPefContex->pRegistrySet->SaveRegistrySet(
		XmlPath.c_str(), REGISTRY_FILE_XML_BY_UTF8 );
}

void CNationEditorDlg::SavePefList()
{
	size_t size = m_PefContextBank.size();
	for(size_t i=0;i<size;++i)
	{
		SavePef( m_PefContextBank[i] );
	}
}


void CNationEditorDlg::LoadNationList()
{
	CStdioFile FileStream;
	if( !FileStream.Open(m_CurrentFilePath, CFile::modeRead) )
	{
		I3PRINTLOG(I3LOG_FATAL, "Nation.nlf File - Stream IO Error! %s", m_CurrentFilePath.GetString() );
	}

	m_ListBox_Nation.ResetContent();

	CString Buffer;
	std::string NationString;
	while( FileStream.ReadString(Buffer) )
	{
		NationString = Buffer;

		size_t Index;
		//주석 제거
		if( !NationString.empty() )
		{
			Index = NationString.rfind(";");
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;
		//앞쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_first_not_of(" \t");
			if( Index != std::string::npos )
				NationString.erase(0, Index );
		}
		else
			continue;

		//뒷쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_last_not_of(" \t") +1;
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;

		if( !NationString.empty() )
		{
			AddListNationString( NationString.c_str() );
		}
	}

	size_t size = m_NationBank.size();
	CString CountStr;
	CountStr.Format("Nation List (%d)", size);
	SetDlgItemTextA(IDC_ST_NATION, CountStr);

	FileStream.Close();
}

void CNationEditorDlg::SaveNationList()
{
	if( m_bCreateBakFile )
	{
		CString BakPath = m_CurrentFilePath;
		BakPath.Append(".bak" );

		FileUtil::CopyFile(m_CurrentFilePath.GetString(), BakPath.GetString());
	}

	CStdioFile FileStream;
	if( !FileStream.Open(m_CurrentFilePath, CFile::modeCreate | CFile::modeWrite) )
	{
		I3PRINTLOG(I3LOG_FATAL, "Nation.nlf File - Stream IO Error! %s", m_CurrentFilePath.GetString() );
	}

	FileStream.WriteString( ";통합 Pef에 사용되는 국가 리스트 입니다.\n" );
	FileStream.WriteString( ";대소문자 구분없습니다.\n" );
	FileStream.WriteString( ";한 라인으로 나라를 구분합니다.\n" );

	size_t size = m_NationBank.size();
	for(size_t i=0;i<size;++i)
	{
		FileStream.WriteString( m_NationBank[i].c_str() );
		FileStream.WriteString( "\n" );
	}

	FileStream.Close();
}

void CNationEditorDlg::OnBnClickedOk()
{
	CString AddNationString, DelNationString;

	GetDlgItemText(IDC_EDIT1, AddNationString);
	GetDlgItemText(IDC_EDIT2, DelNationString);

	CString MessageString;
	if( !AddNationString.IsEmpty() )
	{
		if( IsDuplicateString(m_ListBox_Nation, AddNationString) )
		{
			MessageString.Format( "[%s] country has already been registered.", AddNationString  );
			AfxMessageBox(MessageString);
			return;
		}

		MessageString.Format( "Would you like to add [%s] country?", AddNationString  );
		if( AfxMessageBox(MessageString, MB_YESNO | MB_ICONQUESTION) != IDYES )
		{
			return;
		}
		else
		{
			AddRegNation(AddNationString);
			AddListNationString( AddNationString );

			SavePefList();
			SaveNationList();

			MessageString.Format( "[%s] country registered.", AddNationString  );
			AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);

			return;
		}
	}

	if( !DelNationString.IsEmpty() )
	{
		if( !IsDuplicateString(m_ListBox_Nation, DelNationString) )
		{
			MessageString.Format( "[%s] country is not registered.", DelNationString  );
			AfxMessageBox(MessageString);
			return;
		}

		MessageString.Format( "Would you like to del [%s] nation?", DelNationString  );
		if( AfxMessageBox(MessageString, MB_YESNO | MB_ICONQUESTION) != IDYES )
		{
			return;
		}
		else
		{
			DelRegNation(DelNationString);
			DelListNationString( DelNationString );

			SavePefList();
			SaveNationList();

			MessageString.Format( "[%s] country unregistered.", DelNationString  );
			AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);
			return;
		}
	}

	MessageString.Format( "No Action! Input Action!" );
	AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);
}

bool CNationEditorDlg::IsDuplicateString(CHListBox & ListCtrl, const CString & String)
{
	int Count = ListCtrl.GetCount();

	CString ListString;
	for(int i=0;i<Count;++i)
	{
		ListCtrl.GetText(i, ListString);
		
		if( String == ListString )
			return true;
	}
	return false;
}

size_t CNationEditorDlg::GetOperationNationIndex(size_t Index)
{
	String Nation = m_OperationCtrl.GetNationName( Index );

	if( Nation == "_Blank_" )
		return (size_t)-1;

	size_t size = m_NationBank.size();
	for(size_t i=0;i<size;++i)
	{
		if( m_NationBank[i] == Nation )
			return i;
	}
	
	return (size_t)-1;
}


void CNationEditorDlg::AddRegNation(const CString & AddNationName)
{
	//등록된 모든 Pef에 추가 실행 한다.
	size_t size = m_PefContextBank.size();
	for(size_t i=0;i<size;++i)
	{
		size_t FromIndex = GetOperationNationIndex( i );

		i3RegKey * pRoot = m_PefContextBank[i]->pRegistrySet->getRoot();
		Spread::AddReg( pRoot, FromIndex, 0);
	}
}

void CNationEditorDlg::DelRegNation(const CString & DelNationName)
{
	size_t max = m_NationBank.size();
	size_t Index = GetNationIndex( DelNationName.GetString() );
	
	size_t size = m_PefContextBank.size();
	for(size_t i=0;i<size;++i)
	{
		i3RegKey * pRoot = m_PefContextBank[i]->pRegistrySet->getRoot();
		Spread::RemoveReg( pRoot, Index, max);
	}
}

void CNationEditorDlg::CloneRegNation(size_t pefInx, StringVector* pList, int srcInx, int destInx)
{
	i3RegKey * pRoot = m_PefContextBank[pefInx]->pRegistrySet->getRoot();
	Spread::CloneReg( pRoot, (void*)pList, srcInx, destInx);

	SavePef( m_PefContextBank[pefInx] );

	CString MessageString;
	MessageString.Format( "[%s] pef saved.", m_PefContextBank[pefInx]->Name.c_str()  );
	AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);
}

void CNationEditorDlg::_SwapPef(const OrderList* pOrderList)
{
	//등록된 모든 Pef에 추가 실행 한다.
	size_t size = m_PefContextBank.size();
	for(size_t i=0;i<size;++i)
	{
		i3RegKey * pRoot = m_PefContextBank[i]->pRegistrySet->getRoot();
		Spread::SwapReg( pRoot, (void*)pOrderList);
	}
}

void CNationEditorDlg::OnLbnSelchangeListNation()
{
	if( m_bControlKeyDown )
	{
		CString Name = m_ListBox_Nation.GetCurrentString();
		SetDlgItemText(IDC_EDIT2, Name );
	}
}

BOOL CNationEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_CONTROL:
			m_bControlKeyDown = true;
			break;
		}
		break;
	case WM_KEYUP:
		switch( pMsg->wParam )
		{
		case VK_CONTROL:
			m_bControlKeyDown = false;
			break;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CNationEditorDlg::OnBnClickedCheckBakFile()
{
	if( m_bCreateBakFile )
		m_bCreateBakFile = false;
	else
		m_bCreateBakFile = true;
}

void CNationEditorDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	char szName[MAX_PATH];
	int Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);
	for( int i = 0 ; i < Count ; i++)
	{
		DragQueryFile( hDropInfo, i, szName, sizeof(szName) - 1);
	}

	if( !LoadList( szName ) )
	{
		AfxMessageBox( "It is not Nation List File!" );
	}

	DragFinish(hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

void CNationEditorDlg::OnBnClickedButton1()
{
	COrderDialog dlg(this);
	if( dlg.DoModal() == IDOK )
	{
		CString MessageString;

		MessageString.Format( "Would you like to swap nation?");
		if( AfxMessageBox(MessageString, MB_YESNO | MB_ICONQUESTION) == IDYES )
		{
			const OrderList* pOrderList = dlg.GetOrderList();

			//NationList를 변경한다.
			_SwapNationList( pOrderList );
			_SwapPef(pOrderList);

			SaveNationList();
			SavePefList();

			MessageString.Format( "Country swapped.");
			AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);
		}
	}
}

void CNationEditorDlg::_SwapNationList(const OrderList* pOrderList)
{
	StringVector nationList;

	size_t size = pOrderList->size();
	for(size_t i = 0; i < size; ++i)
	{
		nationList.push_back( m_NationBank[ (*pOrderList)[i] ] );
	}

	m_NationBank.clear();
	m_ListBox_Nation.ResetContent();

	size = nationList.size();
	for(size_t i = 0; i < size; ++i)
	{
		AddListNationString( nationList[i].c_str() );
	}
}

void CNationEditorDlg::OnBnClickedButton2()
{
	if( m_PefContextBank.empty() || m_NationBank.empty() )
	{
		CString MessageString;
		MessageString.Format( "First! Load [Nation.nlf] File !");
		AfxMessageBox(MessageString , IDOK | MB_DEFBUTTON1);
		return;
	}

	CClonePefValueDialog dlg(this);

	dlg.SetPefList( &m_PefContextBank );
	dlg.SetNationList( &m_NationBank );

	if( dlg.DoModal()  == IDOK )
	{
		CloneRegNation(m_ClonePefIndex,
			&m_CloneList,
			m_CloneSrcIndex,
			m_CloneDestIndex);
	}
}

void CNationEditorDlg::SetCloneData(int pefInx, int srcInx, int destInx, const StringVector& List)
{
	m_ClonePefIndex = pefInx;
	m_CloneSrcIndex = srcInx;
	m_CloneDestIndex = destInx;
	m_CloneList = List;
}
