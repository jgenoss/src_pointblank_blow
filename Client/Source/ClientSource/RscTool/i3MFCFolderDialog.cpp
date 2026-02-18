#include "stdafx.h"
#include "i3MFCFolderDialog.h"

#include "i3Base/itl/function/function.h"
#include "i3Base/itl/bind/bind.h"

#include "i3Base/string/ext/make_unix_path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////

// splitterwnd의 wnd구성을 위해 holderview 를 넣음..

namespace detail_folder_dialog
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
	CDialog* m_pDlgOwner; //the owner of this control
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnDraw(CDC*) {}

public:
	void setWnd(CWnd* pWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnChangeFolder(WPARAM,LPARAM);

	void setOwner(CDialog* pDlg) {ASSERT(pDlg); m_pDlgOwner = pDlg; }

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

IMPLEMENT_DYNCREATE(CHolderView, CView)

CHolderView::CHolderView()
{
	m_pWnd=NULL;
	m_pDlgOwner = NULL;
}

CHolderView::~CHolderView()
{
}

BEGIN_MESSAGE_MAP(CHolderView, CView)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(I3_WM_CHANGE_CURRENT_FOLDER, OnChangeFolder)
END_MESSAGE_MAP()


// CHolderView drawing


// CHolderView message handlers

void CHolderView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_pWnd==NULL) return;
	if(!(::IsWindow(m_pWnd->m_hWnd))) return;

	m_pWnd->MoveWindow(0,0,cx,cy, FALSE);
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
	if(::IsWindow(m_hWnd) && ::IsWindow(pWnd->m_hWnd)) {
		CRect rect;
		GetClientRect(rect);
		m_pWnd->MoveWindow(rect);
	}
}

BOOL CHolderView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return (BOOL) m_pDlgOwner->SendMessage(WM_COMMAND, wParam, lParam);
}

LRESULT CHolderView::OnChangeFolder(WPARAM,LPARAM)
{
	((i3MFCFolderDialog*)m_pDlgOwner)->UpdateFolderComboText();
	return 0;
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

struct ButtonCallback : i3MFCButtonCallback
{
	ButtonCallback(const i3::function<>& f) : m_f(f) {}
	virtual void OnClick(i3MFCButton* ctrl) 
	{
		m_f();
	}
	i3::function<>	m_f;
};

}

using namespace detail_folder_dialog;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(i3MFCFolderDialog, i3MFCDialog)

i3MFCFolderDialog::i3MFCFolderDialog() : i3MFCDialog(), m_pFrame(NULL)
{
	i3MFCDialog::SetRect(CRect(0,0,1024,768));
	m_buttonOK.SetCallback( new ButtonCallback(i3::bind( &i3MFCFolderDialog::OnOKButton, this) ) );
	m_buttonCancel.SetCallback(new ButtonCallback(i3::bind( &i3MFCFolderDialog::OnCancelButton, this) ) );
}

i3MFCFolderDialog::~i3MFCFolderDialog()
{
	delete m_buttonOK.GetCallback();
	delete m_buttonCancel.GetCallback();
}


BOOL i3MFCFolderDialog::CreateEx(CWnd* pParent, const CRect& rc)
{
	return i3MFCDialog::CreateEx(pParent, rc);


}

BEGIN_MESSAGE_MAP(i3MFCFolderDialog, i3MFCDialog)
	ON_WM_CREATE()

END_MESSAGE_MAP()



int i3MFCFolderDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3MFCDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
		
	return 0;
}

BOOL i3MFCFolderDialog::OnInitDialog()
{
	i3MFCDialog::OnInitDialog();

	SetWindowText(L"Select Folder");

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CCreateContext ccc;
	ccc.m_pNewViewClass   = RUNTIME_CLASS(CHolderView);
	ccc.m_pCurrentDoc     = NULL;
	ccc.m_pNewDocTemplate = NULL;
	ccc.m_pLastView       = NULL;
	ccc.m_pCurrentFrame   = NULL;

	// Because the CFRameWnd needs a window class, we will create a new one. 
	CString strMyClass = AfxRegisterWndClass(CS_VREDRAW |
		CS_HREDRAW,
		::LoadCursor(NULL, IDC_ARROW),
		(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		::LoadIcon(NULL, IDI_APPLICATION));

	CRect rect;	GetClientRect(rect);
	rect.DeflateRect(50, 50);
	
	// Create the frame window with "this" as the parent
	m_pFrame = new CFlatFrameWnd;
	m_pFrame->Create(strMyClass,L"", WS_CHILD, rect, this);
	m_pFrame->ShowWindow(SW_SHOW);

	// and finally, create the splitter with the frame as
	// the parent
	m_cSplitter.CreateStatic(m_pFrame,1, 2);
	m_cSplitter.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	m_cSplitter.CreateView(0,0, RUNTIME_CLASS(CHolderView),	CSize(300,100), &ccc);
	m_cSplitter.CreateView(0,1, RUNTIME_CLASS(CHolderView),CSize(100,100), &ccc);

	CHolderView* pView = (CHolderView*)m_cSplitter.GetPane(0,0);
	ASSERT_VALID(pView);
	pView->setWnd(&m_treectrl);
	pView->setOwner(this);
	m_treectrl.CreateEx(pView, 1); //LBS_NOTIFY|WS_CHILD|WS_VISIBLE|WS_VSCROLL,CRect(0,0,1,1),pView,0);

	pView = (CHolderView*)m_cSplitter.GetPane(0,1);
	ASSERT_VALID(pView);
	pView->setWnd(&m_listctrl);
	pView->setOwner(this);

	m_listctrl.SetShowFolderOnly(true);
	m_listctrl.Create(pView, 1);		// LBS_NOTIFY|WS_CHILD|WS_VISIBLE|WS_VSCROLL,CRect(0,0,1,1),pView,IDC_SLAVE);
	
	// #define CSIDL_DRIVES                    0x0011        // My Computer
	BOOL select_path_res = FALSE;

	if (!m_strResult.empty() )
	{
		i3::wstring mspath = m_strResult;
		i3::range::replace(mspath, L'/', L'\\');
		select_path_res = m_treectrl.SelectPath(mspath.c_str());
	}

	if (select_path_res == FALSE)
	{
		LPITEMIDLIST pidl_Drives;
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl_Drives)))
		{
			m_treectrl.SelectPath(pidl_Drives);		
		}
	}

	m_treectrl.SetRelatedList(&m_listctrl);

	m_cSplitter.MoveWindow(0,0, rect.Width(), rect.Height(), FALSE);	
	m_cSplitter.ShowWindow(SW_SHOW);
	
	rect.top -= 24;
	m_combo_fullpath.Create(WS_VISIBLE|CBS_DROPDOWN|WS_CHILD, CRect(rect.left,rect.top,rect.right,200), this, 1212);
		
	m_combo_fullpath.GetEditCtrl()->SetReadOnly(TRUE);

	int leftButtonOffset = rect.Width() / 2 - 20 - 80;
	int rightButtonOffset = rect.Width() / 2 + 20;
	
	CRect rcButtonLeft;
	rcButtonLeft.left = rect.left + leftButtonOffset;
	rcButtonLeft.right = rcButtonLeft.left + 80;
	rcButtonLeft.top = rect.bottom + 5;
	rcButtonLeft.bottom = rcButtonLeft.top + 40;

	CRect rcButtonRight;
	rcButtonRight.left = rect.left + rightButtonOffset;
	rcButtonRight.right = rcButtonRight.left + 80;
	rcButtonRight.top = rect.bottom + 5;
	rcButtonRight.bottom = rcButtonRight.top + 40;

	m_buttonOK.Create(L"OK", WS_VISIBLE|WS_CHILD, rcButtonLeft, this, 1);
	m_buttonCancel.Create(L"Cancel", WS_VISIBLE|WS_CHILD, rcButtonRight, this, 1);

	i3MFCFolderDialog::UpdateFolderComboText();

//	m_treectrl.EnsureVisible( m_treectrl.GetSelectedItem());

	m_treectrl.SetFocus();

	HTREEITEM sel_item = m_treectrl.GetSelectedItem();
	if (sel_item != NULL)
	{
		m_treectrl.SelectSetFirstVisible(sel_item);
		
		int vis_count = m_treectrl.GetVisibleCount();

		int cur_pos = m_treectrl.GetScrollPos(SB_VERT);
			
		cur_pos = (cur_pos > vis_count / 2) ? cur_pos - vis_count / 2 : cur_pos;

		m_treectrl.SetScrollPos(SB_VERT, cur_pos );

	}
	

	return FALSE; //TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void i3MFCFolderDialog::UpdateFolderComboText()
{
	if (m_combo_fullpath.GetSafeHwnd() == NULL)
		return;

	CString strPath;
	if (!m_listctrl.GetCurrentFolder (strPath) &&
		!m_listctrl.GetCurrentFolderName (strPath))
	{
		strPath = _T("????");
	}

	m_strResult.assign(LPCWSTR(strPath), strPath.GetLength() );
	i3::make_unix_path(m_strResult);

	m_combo_fullpath.GetEditCtrl()->SetWindowText (m_strResult.c_str());	
}


void i3MFCFolderDialog::OnOKButton()
{
	EndDialog(IDOK);
}

void i3MFCFolderDialog::OnCancelButton()
{
	EndDialog(IDCANCEL);
}

void i3MFCFolderDialog::SelectFolder( const i3::wstring& wstrPath)
{
	m_strResult = wstrPath;
	
	i3::range::replace(m_strResult, L'\\', L'/');

	if ( this->GetSafeHwnd() )
	{
		BOOL select_path_res = FALSE;

		if (!m_strResult.empty() )
		{
			i3::wstring mspath = m_strResult;
			i3::range::replace(mspath, L'/', L'\\');

			select_path_res = m_treectrl.SelectPath(mspath.c_str());
		}

		if (select_path_res == FALSE)
		{
			LPITEMIDLIST pidl_Drives;
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl_Drives)))
			{
				m_treectrl.SelectPath(pidl_Drives);		
			}
		}

		i3MFCFolderDialog::UpdateFolderComboText();
	}
}

