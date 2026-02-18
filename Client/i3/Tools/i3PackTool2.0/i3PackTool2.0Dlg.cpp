
// i3PackTool2.0Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3PackTool2.0.h"
#include "i3PackTool2.0Dlg.h"
#include "PTTreeworkQA.h"
#include "PTTreeworkLive.h"
#include "PTTreeworkMediator.h"

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


// Ci3PackTool20Dlg 대화 상자
Ci3PackTool20Dlg::Ci3PackTool20Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3PackTool20Dlg::IDD, pParent), m_enableSizeModifyTreeCtrl(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ci3PackTool20Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LIVE_FOLDERPATH, m_editLiveFolderpath);
	DDX_Control(pDX, IDC_EDIT_QA_FOLDERPATH, m_editQAFolderpath);
	DDX_Control(pDX, IDC_EDIT_MEDIA_FOLDERPATH, m_editMediaFolderpath);
}

BEGIN_MESSAGE_MAP(Ci3PackTool20Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OPEN_LIVE_FOLDER, &Ci3PackTool20Dlg::OnOpenLiveFolder)
	ON_COMMAND(ID_OPEN_QA_FOLDER, &Ci3PackTool20Dlg::OnOpenQAFolder)
	ON_COMMAND(ID_OPEN_MEDIA_FOLDER, &Ci3PackTool20Dlg::OnOpenMediaFolder)
	ON_COMMAND(ID_32774, &Ci3PackTool20Dlg::OnExit)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// Ci3PackTool20Dlg 메시지 처리기

BOOL Ci3PackTool20Dlg::OnInitDialog()
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
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//this->ModifyStyle(NULL, WS_THICKFRAME); // 사이즈 조절 가능

	// 이미지 리스트 생성
	if (!m_imageList.Create(16, 16, 0, 1, 0))
	{
		AfxMessageBox("Problem in creating ImageList Ctrl");
	}
	else
	{
		CBitmap bmpImage;
		for (UINT bid = IDB_BITMAP1, i = 0; bid <= IDB_BITMAP2; bid++,i++) 
		{
			bmpImage.LoadBitmap(bid);
			m_imageList.Add(&bmpImage, (COLORREF)NULL);
			bmpImage.DeleteObject();
		}	
	}


	// LIVE 트리
	RECT rect = {15, 40, 380, 600};

	m_liveTreeCtrl.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASLINES|TVS_LINESATROOT
		|TVS_HASBUTTONS|TVS_TRACKSELECT|TVS_SHOWSELALWAYS, rect, this, 1001);

	m_liveTreeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	// QA 트리
	rect.left = rect.right + 15;
	rect.right = rect.left + 380;

	m_qaTreeCtrl.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASLINES|TVS_LINESATROOT
		|TVS_HASBUTTONS|TVS_TRACKSELECT|TVS_SHOWSELALWAYS, rect, this, 1002);

	m_qaTreeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	// MEDIA 트리
	rect.left = rect.right + 15;
	rect.right = rect.left + 380;

	m_mediaTreeCtrl.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASLINES|TVS_LINESATROOT
		|TVS_HASBUTTONS|TVS_TRACKSELECT|TVS_SHOWSELALWAYS, rect, this, 1003);

	m_mediaTreeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	// 컨트롤들 비율 미리 계산.
	RECT rt;
	GetClientRect(&rt);

	RECT rtCtrl;

	m_liveTreeCtrl.GetClientRect(&rtCtrl);
	m_liveTreeSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_liveTreeSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);

	m_qaTreeCtrl.GetClientRect(&rtCtrl);
	m_qaTreeCtrlSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_qaTreeCtrlSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);

	m_mediaTreeCtrl.GetClientRect(&rtCtrl);
	m_mediaTreeCtrlSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_mediaTreeCtrlSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);

	m_editLiveFolderpath.GetClientRect(&rtCtrl);
	m_liveEditSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_liveEditSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);

	m_editQAFolderpath.GetClientRect(&rtCtrl);
	m_qaEditSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_qaEditSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);

	m_editMediaFolderpath.GetClientRect(&rtCtrl);
	m_mediaEditSizeRatio.w = static_cast<REAL32>(rtCtrl.right) / static_cast<REAL32>(rt.right);
	m_mediaEditSizeRatio.h = static_cast<REAL32>(rtCtrl.bottom) / static_cast<REAL32>(rt.bottom);
	
	m_enableSizeModifyTreeCtrl = true;

	// collegue 생성
	TheMediator().CreateCollegues();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Ci3PackTool20Dlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void Ci3PackTool20Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3PackTool20Dlg::OnPaint()
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
		int x = static_cast<int>((rect.Width() - cxIcon + 1) * 0.5f);
		int y = static_cast<int>((rect.Height() - cyIcon + 1) * 0.5f);

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
HCURSOR Ci3PackTool20Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3PackTool20Dlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CDialog::OnContextMenu(pWnd, point);
}

void Ci3PackTool20Dlg::OnExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	EndDialog(-1);
}

void Ci3PackTool20Dlg::OnOpenLiveFolder()
{
	TheMediator().GetTreeworkLive()->OpenWorkingFolder(&m_liveTreeCtrl);
}

void Ci3PackTool20Dlg::OnOpenQAFolder()
{
	TheMediator().GetTreeworkQA()->OpenWorkingFolder(&m_qaTreeCtrl);
}

void Ci3PackTool20Dlg::OnOpenMediaFolder()
{
}

void Ci3PackTool20Dlg::OnSize(UINT nType, int cx, int cy)
{
	//CDialog::OnSize(nType, cx, cy);

	//// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//if (m_enableSizeModifyTreeCtrl == false) return;

	//// tree ctrl
	//RECT rtLive;
	//m_liveTreeCtrl.GetWindowRect(&rtLive);
	//ScreenToClient(&rtLive);
	//rtLive.right = rtLive.left + static_cast<LONG>((cx*m_liveTreeSizeRatio.w));
	//rtLive.bottom = rtLive.top + static_cast<LONG>((cy*m_liveTreeSizeRatio.h));
	//m_liveTreeCtrl.MoveWindow(&rtLive, FALSE);
	//m_liveTreeCtrl.Invalidate(FALSE);

	//RECT rtQA;
	//m_qaTreeCtrl.GetWindowRect(&rtQA);
	//ScreenToClient(&rtQA);
	//rtQA.left = rtLive.right + 15;
	//rtQA.right = rtQA.left + static_cast<LONG>((cx*m_qaTreeCtrlSizeRatio.w));
	//rtQA.bottom = rtQA.top + static_cast<LONG>((cy*m_qaTreeCtrlSizeRatio.h));
	//m_qaTreeCtrl.MoveWindow(&rtQA, FALSE);
	//m_qaTreeCtrl.Invalidate(FALSE);

	//RECT rtMedia;
	//m_mediaTreeCtrl.GetWindowRect(&rtMedia);
	//ScreenToClient(&rtMedia);
	//rtMedia.left = rtQA.right + 15;
	//rtMedia.right = rtMedia.left + static_cast<LONG>((cx*m_mediaTreeCtrlSizeRatio.w));
	//rtMedia.bottom = rtMedia.top + static_cast<LONG>((cy*m_mediaTreeCtrlSizeRatio.h));
	//m_mediaTreeCtrl.MoveWindow(&rtMedia, FALSE);
	//m_mediaTreeCtrl.Invalidate(FALSE);

	//// edit box
	//m_editLiveFolderpath.GetWindowRect(&rtLive);
	//ScreenToClient(&rtLive);
	//rtLive.right = rtLive.left + static_cast<LONG>((cx*m_liveEditSizeRatio.w));
	//rtLive.bottom = rtLive.top + static_cast<LONG>((cy*m_liveEditSizeRatio.h));
	//m_editLiveFolderpath.MoveWindow(&rtLive, FALSE);
	//m_editLiveFolderpath.Invalidate(FALSE);

	//m_editQAFolderpath.GetWindowRect(&rtQA);
	//ScreenToClient(&rtQA);
	//rtQA.left = rtLive.right + 15;
	//rtQA.right = rtQA.left + static_cast<LONG>((cx*m_qaEditSizeRatio.w));
	//rtQA.bottom = rtQA.top + static_cast<LONG>((cy*m_qaEditSizeRatio.h));
	//m_editQAFolderpath.MoveWindow(&rtQA, FALSE);
	//m_editQAFolderpath.Invalidate(FALSE);

	//m_editMediaFolderpath.GetWindowRect(&rtMedia);
	//ScreenToClient(&rtMedia);
	//rtMedia.left = rtQA.right + 15;
	//rtMedia.right = rtMedia.left + static_cast<LONG>((cx*m_mediaEditSizeRatio.w));
	//rtMedia.bottom = rtMedia.top + static_cast<LONG>((cy*m_mediaEditSizeRatio.h));
	//m_editMediaFolderpath.MoveWindow(&rtMedia, FALSE);
	//m_editMediaFolderpath.Invalidate(FALSE);
}
