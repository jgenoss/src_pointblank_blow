
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "BuildTool.h"

#include "MainFrm.h"
#include "ui/UIDefines.h"
#include "ui/ContentsView.h"
#include "ui/SyncPlayerBar.h"
#include "ui/SyncPlayerDlg.h"
#include <functional>
#include <boost/signals2/signal.hpp>
#include "ui/ContentsDescriptionBar.h"
#include "ui/MQContentsStatusBar.h"
#include "ui/QLContentsStatusBar.h"
#include "resource/ContentsSynchronizer.h"
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	struct ContentsViewCallback : ui::IContentsViewCallback
	{
		ui::ContentsView& view;

		boost::signals2::signal<void(const rsc::ContentsDataPtr&, const std::wstring&)> sigContentsClick;
		
		using AddContentsFuncType = std::function<void(const std::vector<rsc::ContentsSyncDataPtr>&)>;
		AddContentsFuncType f_AddContents;

		ContentsViewCallback(ui::ContentsView& v, AddContentsFuncType addContentsFunc)
			: view(v), f_AddContents(addContentsFunc) 
		{}

		virtual ~ContentsViewCallback() 
		{
			sigContentsClick.disconnect_all_slots();
		}

		virtual void OnContentsClick(const long row) override
		{
			const rsc::ContentsDataPtr& contentsData = view.GetContents(row);
			if (contentsData->strContentsName.empty())
				return;

			const std::wstring& wstrCountryName = view.GetCountryName();

			if (!sigContentsClick.empty())
				sigContentsClick(contentsData, wstrCountryName);
		}

		virtual void OnAddContents(const long row) override
		{
			rsc::ContentsSyncDataPtr syncData = std::make_shared<rsc::ContentsSyncData>();

			const rsc::ContentsDataPtr contents = view.GetContents(row);
			syncData->contents = contents;

			const rsc::CountryDataPtr country = rsc::CountrySettings::i()->FindData(rsc::ResourceController::i()->GetCurrCountryName());
			syncData->country = country;

			syncData->type = i3::generic_is_iequal(contents->strCategoryName, "Shader") ? rsc::SYNCHRONIZER_TYPE_SHADER : rsc::SYNCHRONIZER_TYPE_CONTENTS;

			f_AddContents({ syncData });
		}
	};
}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_MESSAGE(ui::WMU_SET_CONTENTS_NON_SYNC_COLOR, &CMainFrame::OnViewListCtrlLClicked)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	//if (!m_wndMenuBar.Create(this))
	//{
	//	TRACE0("메뉴 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 메뉴 모음을 활성화해도 포커스가 이동하지 않게 합니다.
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	/*CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);*/

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	//m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 사용자 정의 도구 모음 작업을 허용합니다.
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	CRect rect;
	GetClientRect(&rect);

	const int cx = rect.Width();
	const int cy = rect.Height();

	const int r_taskBar = static_cast<int>(cx * 0.1f);
	if (!m_taskBar.Create(L"Tasks", this, CRect(0, 0, r_taskBar, cy), TRUE, ui::ID_TASK_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create item tasks bar \n");
		return -1;
	}

	const int l_syncPlayerBar = cx - static_cast<int>(cx * 0.18f);
	if (!m_syncPlayerBar.Create(L"Sync Player", this, CRect(l_syncPlayerBar, 0, cx, cy), TRUE, ui::ID_SYNC_PLAYER_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create item tasks bar \n");
		return -1;
	}

	//if (!m_outputBar.Create(L"Output", this, CRect (0, 0, 150, 150), TRUE, ui::ID_OUTPUT_BAR,
	//	WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("Failed to create output bar \n");
	//	return -1;
	//}

	const int cx_center = l_syncPlayerBar - r_taskBar;

	const int cx_descBar = static_cast<int>(cx_center * 0.2f);
	const int cx_MQStatusBar = static_cast<int>(cx_center * 0.4f);
	const int cx_QLStatusBar = cx_center - (cx_descBar + cx_MQStatusBar);

	const int r_descBar = r_taskBar + cx_descBar;
	if (!m_contentsDescBar.Create(L"Description", this, CRect(r_taskBar, 0, r_descBar, 200), TRUE, ui::ID_CONTENTS_DESCRIPTION_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create contents description bar \n");
		return -1;
	}

	const int r_MQStatusBar = r_descBar + cx_MQStatusBar;
	if (!m_MQContentsStatusBar.Create(L"MEDIA <-> QA", this, CRect(r_descBar, 0, r_MQStatusBar, 200), TRUE, ui::ID_MQ_CONTENTS_STATUS_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create MEDIA <-> QA contents status bar\n");
		return -1;
	}

	const int r_QLStatusBar = r_MQStatusBar + cx_QLStatusBar;
	if (!m_QLContentsStatusBar.Create(L"QA <-> LIVE", this, CRect(r_MQStatusBar, 0, r_QLStatusBar, 200), TRUE, ui::ID_QL_CONTENTS_STATUS_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create QA <-> LIVE contents status bar \n");
		return -1;
	}
	m_QLContentsStatusBar.SetOwner(&m_MQContentsStatusBar);

	m_MQContentsStatusBar.SetQLContentsStatusBar(&m_QLContentsStatusBar);	// m_QLContentsStatusBar 의 출력 행위는 m_MQContentsStatusBar 계산 결과에 종속되므로.


	if (!m_packInnerFileSearchBar.Create(L"Packed File Search Bar", this, CRect(0, 0, r_QLStatusBar, 200), TRUE, ui::ID_I3PACK_DESCRIPTION_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create packedFileSearchBar \n");
		return -1;
	}
	m_packInnerFileSearchBar.ShowPane(FALSE, FALSE, TRUE);

	
	if (!m_i3PackToolBar.Create(L"i3Pack Tool", this, CRect(r_taskBar, 0, r_descBar, 200), TRUE, ui::ID_I3PACK_DESCRIPTION_BAR,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create i3PackDescriptionBar \n");
		return -1;
	}
	m_i3PackToolBar.ShowPane(FALSE, FALSE, TRUE);
	
	// TODO: 도구 모음 및 메뉴 모음을 도킹할 수 없게 하려면 이 다섯 줄을 삭제하십시오.
	//m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_outputBar.EnableDocking(CBRS_ALIGN_ANY);
	m_taskBar.EnableDocking(CBRS_ALIGN_ANY);
	m_contentsDescBar.EnableDocking(CBRS_ALIGN_ANY);
	m_MQContentsStatusBar.EnableDocking(CBRS_ALIGN_ANY);
	m_QLContentsStatusBar.EnableDocking(CBRS_ALIGN_ANY);
	m_syncPlayerBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);

	//DockPane(&m_wndMenuBar);
	//DockPane(&m_wndToolBar);

	DockPane(&m_taskBar);	// left.
	DockPane(&m_syncPlayerBar);	// right.
	DockPane(&m_contentsDescBar);	// bottom left.

	DockPane(&m_MQContentsStatusBar);	// bottom right.
	m_MQContentsStatusBar.DockToWindow(&m_contentsDescBar, CBRS_RIGHT);
	
	DockPane(&m_QLContentsStatusBar);	// bottom right.
	m_QLContentsStatusBar.DockToWindow(&m_MQContentsStatusBar, CBRS_RIGHT);

	DockPane(&m_packInnerFileSearchBar);

	m_duplicatedPackFilesReport.Create(IDD_DIALOG_DUPLICATED_PACKFILES, this);
	m_duplicatedPackFilesReport.ShowWindow(SW_HIDE);



	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 도구 모음 및 도킹 창 메뉴 바꾸기를 활성화합니다.
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 빠른(<Alt> 키를 누른 채 끌기) 도구 모음 사용자 지정을 활성화합니다.
	//CMFCToolBar::EnableQuickCustomization();

	//if (CMFCToolBar::GetUserImages() == NULL)
	//{
	//	// 사용자 정의 도구 모음 이미지를 로드합니다.
	//	if (m_UserImages.Load(_T(".\\UserImages.bmp")))
	//	{
	//		m_UserImages.SetImageSize(CSize(16, 16), FALSE);
	//		CMFCToolBar::SetUserImages(&m_UserImages);
	//	}
	//}

	// 메뉴 개인 설정을 활성화합니다(가장 최근에 사용한 명령).
	// TODO: 사용자의 기본 명령을 정의하여 각 풀다운 메뉴에 하나 이상의 기본 명령을 포함시킵니다.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	if (cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);
		cs.hMenu = NULL;
	}

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	/*switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}*/

	
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnCountryTaskClicked()
{
	// TasksBar::OnClickTask 에서 처리됨.
}

LRESULT CMainFrame::OnViewListCtrlLClicked(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 기본 클래스가 실제 작업을 수행합니다.

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::SetViews(ui::ContentsView* view1, ui::FileView* view2)
{
	m_contentsView = view1;
	if (m_contentsView)
	{
		std::unique_ptr<ContentsViewCallback> callbackPtr = 
			std::make_unique<ContentsViewCallback>(*m_contentsView, 
				std::bind(&ui::SyncPlayerDlg::AddContents, &m_syncPlayerBar.GetDlg(), std::placeholders::_1));

		callbackPtr->sigContentsClick.disconnect_all_slots();

		callbackPtr->sigContentsClick.connect(
			std::bind(&ui::ContentsDescriptionBar::ShowContentsDesc, &m_contentsDescBar, std::placeholders::_1, std::placeholders::_2));

		callbackPtr->sigContentsClick.connect(
			std::bind(&ui::MQContentsStatusBar::ShowContentsStatusTree, &m_MQContentsStatusBar, std::placeholders::_1, std::placeholders::_2));

		m_contentsView->SetCallback(std::move(callbackPtr));
	}

	m_fileView = view2;
}

void CMainFrame::DestroyAllPopupWnds()
{
}

void CMainFrame::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame::DestroyAllPopupWnds();

	CFrameWndEx::PostNcDestroy();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnDestroy()
{
	CFrameWndEx::OnDestroy();
}

void CMainFrame::OnClose()
{
	// 종료 시 반드시 스레드풀 join 처리를 해야 함.
	rsc::ResourceController::i()->OnClose();
	m_MQContentsStatusBar.OnClose();
	m_i3PackToolBar.Geti3PackToolDlg()->OnClose();

	CFrameWndEx::OnClose(); 
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CFrameWnd::WindowProc(message, wParam, lParam);
}