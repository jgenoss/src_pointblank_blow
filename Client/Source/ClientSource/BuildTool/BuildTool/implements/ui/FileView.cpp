#include "stdafx.h"
#include "FileView.h"
#include "UIDefines.h"
#include "resource/ResourceController.h"
#include "SyncPlayerBar.h"
#include "MainFrm.h"
#include <boost/filesystem.hpp>
#include "resource/FolderFilesCrawler.h"
#include <boost/format.hpp>
#include "i3PackToolTree.h"
#include "PackedFileSearchBar.h"
#include "../resource/CountrySettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	//used to hold a window for splitting with a splitter
	class CHolderView : public CView
	{
		DECLARE_DYNCREATE(CHolderView)

	protected:
		CHolderView();           // protected constructor used by dynamic creation
		virtual ~CHolderView();

	private:
		CWnd* m_pWnd; //the widow that is being holded
		CView* m_pOwner; //the owner of this control

	protected:
		DECLARE_MESSAGE_MAP()
		virtual void OnDraw(CDC*) {}

	public:
		void setWnd(CWnd* pWnd);
		afx_msg void OnSize(UINT nType, int cx, int cy);

		void setOwner(CView* pOwner) { ASSERT(pOwner); m_pOwner = pOwner; }

		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	};

	IMPLEMENT_DYNCREATE(CHolderView, CView)

	CHolderView::CHolderView()
	{
		m_pWnd = NULL;
		m_pOwner = NULL;
	}

	CHolderView::~CHolderView()
	{}

	BEGIN_MESSAGE_MAP(CHolderView, CView)
		ON_WM_SIZE()
	END_MESSAGE_MAP()

	// CHolderView drawing

	// CHolderView message handlers
	void CHolderView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if (m_pWnd == NULL) return;
		if (!(::IsWindow(m_pWnd->m_hWnd))) return;

		m_pWnd->MoveWindow(0, 0, cx, cy, FALSE);
		m_pWnd->RedrawWindow();
	}

	BOOL CHolderView::PreCreateWindow(CREATESTRUCT& cs)
	{
		BOOL bRes = CView::PreCreateWindow(cs);

		cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

		return bRes;
	}

	void CHolderView::setWnd(CWnd* pWnd)
	{
		ASSERT(pWnd);
		m_pWnd = pWnd;
		if (::IsWindow(m_hWnd) && ::IsWindow(pWnd->m_hWnd)) {
			CRect rect;
			GetClientRect(rect);
			m_pWnd->MoveWindow(rect);
		}
	}

	BOOL CHolderView::OnCommand(WPARAM wParam, LPARAM lParam)
	{
		return (BOOL)m_pOwner->SendMessage(WM_COMMAND, wParam, lParam);
	}

	class CFlatFrameWnd : public CFrameWndEx {
		DECLARE_DYNAMIC(CFlatFrameWnd);
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	};
	IMPLEMENT_DYNAMIC(CFlatFrameWnd, CFrameWndEx);

	BOOL CFlatFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
	{
		CFrameWnd::PreCreateWindow(cs);
		cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
		return TRUE;
	}

#pragma region struct PackedFileSearchBar_CallbackEx : ui::IPackedFileSearchBar_Callback
	struct PackedFileSearchBar_CallbackEx : ui::IPackedFileSearchBar_Callback
	{
		virtual ~PackedFileSearchBar_CallbackEx() {}

		virtual void OnAddContents(const long row, CString strItemPath)
		{
			CMainFrame* pMainFrm = GetMainFrame();
			int countryIdx = pMainFrm->GetPackInnerFileSearchBar().GetIndexOfCurrentCountry();

			if (countryIdx < 0)
				return;

			std::vector<rsc::CountryDataPtr> countryList;
			rsc::CountrySettings::i()->GetDataList(countryList);

			CString PackTgtPath = countryList[countryIdx]->GetLiveWorkdir().c_str();
			PackTgtPath.Append(L"\\Pack");
			CString QAWorkdir = countryList[countryIdx]->GetQAWorkdir().c_str();

			// 팩파일 연결
			ui::i3PackToolDlg* i3PackToolDlg = pMainFrm->Geti3PackToolBar().Geti3PackToolDlg();
			
			i3PackToolDlg->SetBrowsePackPathText((LPCWSTR)PackTgtPath);
			i3PackToolDlg->SetBrowseSourcePathText((LPCWSTR)QAWorkdir);
			
			i3PackToolDlg->GetTreeCtrl()->Expandi3PackDetailInfoByPath(Path(strItemPath).string());
		}

	};
#pragma endregion struct PackedFileSearchBar_CallbackEx : ui::IPackedFileSearchBar_Callback

	struct ExplorerTreeCallabck : i3MFCTreeCtrlCallback
	{
		i3ShellTreeCtrl* m_owner = nullptr;

		ExplorerTreeCallabck(i3ShellTreeCtrl* owner) : m_owner(owner) {}

		virtual void OnDragEnd(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, CWnd* targetWnd, const CPoint& ptScreen) {}
		
		virtual void OnLButtonDown(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, void* ItemData) override
		{
			//static_cast<i3MFCTreeCtrl*>(ctrl)->GetItemData(hItem).
		}

		virtual void OnSelChanged(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, void* ItemData) override {}
	
		virtual void OnItemLClicked(i3MFCTreeCtrl* ctrl, HTREEITEM hItem, void* ItemData) override {}

	};

	struct ExplorerListCallback : i3MFCListCtrlCallback
	{
		virtual void OnDragEnd(i3MFCListCtrl* ctrl, const i3::vector<int>& dragIndexList, CWnd* targetWnd, const CPoint& ptScreen) override
		{
			if (targetWnd != nullptr)
			{
				// Hit Test 로 싱크플레이어 여부 판별.
				if (GetMainFrame()->GetSyncPlayerBar().HitTest(ptScreen) == TRUE)
				{
					if (rsc::ResourceController::i()->GetCurrCountryName().empty())	// 국가가 선택되어야만 뭐든 할 수 있음.
					{
						::AfxMessageBox(L"국가가 선택되지 않았습니다. 국가 로딩 후 다시 시도하여 주십시오.");
						return;
					}

					ui::SyncPlayerDlg& dlg = GetMainFrame()->GetSyncPlayerBar().GetDlg();
					
					rsc::CountryDataPtr country = rsc::CountrySettings::i()->FindData(rsc::ResourceController::i()->GetCurrCountryName());
					CString strPath;
					std::vector<Path> invalidPathList;

					for (const auto idx : dragIndexList)
					{
						static_cast<i3ShellListCtrl*>(ctrl)->GetItemPath(strPath, idx);

						const Path relativePath = rsc::ExtractRelativePath(strPath.GetString(), country->GetMediaWorkdir());
						if (relativePath.empty())
						{
							invalidPathList.push_back(strPath.GetString());
							continue;
						}

						auto data = std::make_shared<rsc::ContentsSyncData>();
						data->type = rsc::SYNCHRONIZER_TYPE_FILES;
						data->contents = std::make_shared<rsc::ContentsData>();
						data->country = country;
						data->contents->strContentsName = relativePath.string();

						if (fs::is_directory(strPath.GetString()))
						{
							rsc::FolderFilesCrawler cwl;
							cwl.SetRecursive(true);
							cwl.SetWorkdir(data->country->GetMediaWorkdir());
							cwl.SetRelativePath(relativePath);

							cwl.operator()(data->fileRelPathList);
						}
						else
						{
							data->fileRelPathList.push_back(std::make_shared<Path>(relativePath));
						}

						dlg.AddContents({ data });
					}

					if (!invalidPathList.empty())
					{
						std::wstring wstrErr = boost::io::str(boost::wformat(L"%s 폴더 내 파일들이 아니므로 동기화 할 수 없습니다.\n") % country->GetMediaWorkdir().wstring().c_str());
						for (const auto& path : invalidPathList)
						{
							wstrErr += path.wstring();	wstrErr += L"\n";
						}

						::AfxMessageBox(wstrErr.c_str());
					}
				}
				else if (GetMainFrame()->Geti3PackToolBar().HitTest(ptScreen) == TRUE)
				{
					std::vector<std::string> fileAbsPathList;
					CString strPath;
					std::string str;
					for (const auto idx : dragIndexList)
					{
						static_cast<i3ShellListCtrl*>(ctrl)->GetItemPath(strPath, idx);
						i3::utf16_to_mb(strPath.GetString(), str);
						fileAbsPathList.push_back(str);
					}

					GetMainFrame()->Geti3PackToolBar().Geti3PackToolDlg()->GetTreeCtrl()->AddFilesToCurrPack(fileAbsPathList);
				}
			}
		}

		virtual void OnClick(i3MFCListCtrl* ctrl, int index, int subItem) override
		{
			CString strItemPath;
			static_cast<i3ShellListCtrl*>(ctrl)->GetItemPath(strItemPath, index);

			if (i3::generic_is_iequal(Path(strItemPath).extension().string(), ".i3Pack"))
			{	
				if (i3::generic_is_iequal(Path(strItemPath).parent_path().filename().string(), "Pack"))
				{
					const std::wstring wstrPackTgtPath = Path(strItemPath).parent_path().wstring();
					GetMainFrame()->Geti3PackToolBar().Geti3PackToolDlg()->SetBrowsePackPathText(wstrPackTgtPath);

					// 팩 원본 경로(국가 QA 폴더) 설정.
					// 우선 빌드툴 내장 국가 세팅 정보 이용 검색.
					std::vector<rsc::CountryDataPtr> countryList;
					rsc::CountrySettings::i()->GetDataList(countryList);

					for (const auto& country : countryList)
					{
						if (i3::generic_is_iequal(country->GetLiveWorkdir().string(), Path(wstrPackTgtPath).parent_path().string()))
						{
							const std::wstring wstrQAWorkdir = country->GetQAWorkdir().wstring();
							if (wstrQAWorkdir.empty())
								continue;

							GetMainFrame()->Geti3PackToolBar().Geti3PackToolDlg()->SetBrowseSourcePathText(wstrQAWorkdir);
							break;
						}
					}
				}

				GetMainFrame()->Geti3PackToolBar().Geti3PackToolDlg()->GetTreeCtrl()->Expandi3PackDetailInfoByPath(Path(strItemPath.GetString()).string());
			}
		}
	};
	
}

namespace ui
{
#ifdef _DEBUG
	void FileView::AssertValid() const
	{
		CView::AssertValid();
	}

#ifndef _WIN32_WCE
	void FileView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}
#endif
#endif //_DEBUG

	IMPLEMENT_DYNCREATE(FileView, CView)

	BEGIN_MESSAGE_MAP(FileView, CView)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	FileView::FileView()
	{
		m_treeCtrl.SetCallback(new ExplorerTreeCallabck(&m_treeCtrl));
		m_listCtrl.SetCallback(new ExplorerListCallback);
	}

	FileView::~FileView()
	{
		delete m_listCtrl.GetCallback();
		delete m_treeCtrl.GetCallback();	
		delete GetMainFrame()->GetPackInnerFileSearchBar().GetCallback();
	}

	void FileView::CreateExplorer()
	{
		if (m_isExplorerCreated)
			return;

		CCreateContext ccc;
		ccc.m_pNewViewClass = RUNTIME_CLASS(CHolderView);
		ccc.m_pCurrentDoc = NULL;
		ccc.m_pNewDocTemplate = NULL;
		ccc.m_pLastView = NULL;
		ccc.m_pCurrentFrame = NULL;

		// Because the CFRameWnd needs a window class, we will create a new one. 
		CString strMyClass = AfxRegisterWndClass(CS_VREDRAW |
			CS_HREDRAW,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH) ::GetStockObject(WHITE_BRUSH),
			::LoadIcon(NULL, IDI_APPLICATION));

		CRect rect;	GetClientRect(rect);

		// Create the frame window with "this" as the parent
		m_frame = new CFlatFrameWnd;
		m_frame->Create(strMyClass, L"", WS_CHILD, rect, this);
		m_frame->ShowWindow(SW_SHOW);

		// and finally, create the splitter with the frame as
		// the parent
		m_splitter.CreateStatic(m_frame, 1, 2);
		m_splitter.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		m_splitter.CreateView(0, 0, RUNTIME_CLASS(CHolderView), CSize(300, 100), &ccc);
		m_splitter.CreateView(0, 1, RUNTIME_CLASS(CHolderView), CSize(100, 100), &ccc);

		CHolderView* pTreeView = (CHolderView*)m_splitter.GetPane(0, 0);
		ASSERT_VALID(pTreeView);
		pTreeView->setWnd(&m_treeCtrl);
		pTreeView->setOwner(this);
		m_treeCtrl.CreateEx(pTreeView, 1);

		::SetWindowTheme(m_treeCtrl.GetSafeHwnd(), L"explorer", 0);

		m_treeCtrl.SetExtendedStyle(
			TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER
			, //TVS_EX_AUTOHSCROLL | 
			TVS_EX_FADEINOUTEXPANDOS | TVS_EX_DOUBLEBUFFER);

		m_treeCtrl.SetBkColor(COLOR_IVORY_BLACK);
		m_treeCtrl.SetTextColor(COLOR_WHITE);

		std::wstring wstrBasePath;

		// 국가가 선택되어 있으면 선택된 국가의 미디어 경로를 기본으로.
		// 선택된 국가가 없으면 C 드라이브 또는 D 드라이브로 설정.
		auto country = rsc::CountrySettings::i()->FindData(rsc::ResourceController::i()->GetCurrCountryName());
		wstrBasePath = country->GetMediaWorkdir().wstring();
		if (wstrBasePath.empty())
			wstrBasePath = L"C:\\";
		
		m_treeCtrl.SetRootPath(wstrBasePath.c_str());
		m_treeCtrl.SelectPath(wstrBasePath.c_str());

		CHolderView* pListView = (CHolderView*)m_splitter.GetPane(0, 1);
		ASSERT_VALID(pListView);
		pListView->setWnd(&m_listCtrl);
		pListView->setOwner(this);

		m_listCtrl.Create(pListView, 1);
		m_listCtrl.SetBkColor(COLOR_IVORY_BLACK);
		m_listCtrl.SetTextColor(COLOR_WHITE);

		m_treeCtrl.SetRelatedList(&m_listCtrl);

		m_splitter.MoveWindow(0, 0, rect.Width(), rect.Height(), FALSE);
		m_splitter.ShowWindow(SW_SHOW);

		m_treeCtrl.SetFocus();

		// init packSearchbar only for making country name list and reserving container.
		GetMainFrame()->GetPackInnerFileSearchBar().Init();
		GetMainFrame()->GetPackInnerFileSearchBar().SetCallback(new PackedFileSearchBar_CallbackEx);

		m_isExplorerCreated = true;
	}

	void FileView::OnDraw(CDC* pDC)
	{}

	int FileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CView::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		CenterWindow();

		return 0;
	}

	void FileView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if (m_splitter.GetSafeHwnd())
		{
			m_frame->MoveWindow(0, 0, cx, cy);
			m_splitter.MoveWindow(0, 0, cx, cy);
		}
	}

	BOOL FileView::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}
}