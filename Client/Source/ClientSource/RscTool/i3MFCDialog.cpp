#include "stdafx.h"
#include "i3MFCDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	i3MFCDialogCallback	g_dialogcallback_null;
}


IMPLEMENT_DYNAMIC(i3MFCDialog, CDialog)

i3MFCDialog::i3MFCDialog() : CDialog(), m_callback(&g_dialogcallback_null), m_rect(0,0,640,480)
{
	
}

i3MFCDialog::~i3MFCDialog()
{

}


BOOL i3MFCDialog::CreateEx(CWnd* pParent, const CRect& rc)
{
	return i3MFCDialog::CreateImpl(pParent, rc, FALSE);
}

int i3MFCDialog::CreateImpl(CWnd* pParent, const CRect& rc, BOOL modal)
{
	
	CFont* pParentFont = (pParent) ? pParent->GetFont() : AfxGetMainWnd()->GetFont();

	LOGFONT lf;
	if (pParentFont)
	{
		pParentFont->GetLogFont(&lf);
	}
	else
	{
		::GetObjectW(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
	}

	int fontLen = wcslen(lf.lfFaceName) + 1;

	int raw_data_size = sizeof(DLGTEMPLATE) + (2*sizeof(WORD)) +
		sizeof(wchar_t) + sizeof(WORD) + fontLen;
	raw_data_size = (raw_data_size + 3) & ~3;

	BYTE* raw_data = (BYTE*)_malloca( raw_data_size );

	DLGTEMPLATE* raw_template = reinterpret_cast<DLGTEMPLATE*>(raw_data);

	if (pParent)
		raw_template->style = WS_VISIBLE | WS_CHILD | WS_CAPTION | DS_SETFONT | WS_SYSMENU;
	else
		raw_template->style = WS_VISIBLE | WS_CAPTION | DS_SETFONT | WS_SYSMENU;

	raw_template->dwExtendedStyle = WS_EX_DLGMODALFRAME;
	raw_template->cdit = 0;
	raw_template->x = (short)0; 
	raw_template->y = (short)0; 
	raw_template->cx = (short)((rc.right - rc.left) * 8 / lf.lfHeight );
	raw_template->cy = (short)((rc.bottom - rc.top) * 8 / lf.lfHeight );

	raw_data += sizeof(DLGTEMPLATE);
	*((WORD*)raw_data) = 0; // no menu -- WORD to say it is 0 bytes 
	raw_data += sizeof(WORD);

	*((WORD*)raw_data) = 0; // use default window class -- WORD to say it is 0 bytes 
	raw_data += sizeof(WORD);

	*((wchar_t*)raw_data) = 0;		// 캡션..(일단 비우기 위한 널값)
	raw_data += sizeof(wchar_t);

	*((WORD*) raw_data) = (WORD)lf.lfHeight;	//폰트 크기..
	raw_data += sizeof(WORD);

	memcpy(raw_data, lf.lfFaceName, fontLen);
	raw_data += fontLen;


	int res = -1;
	
	if (modal)	
	{
		this->InitModalIndirect(raw_template, pParent);
		res = CDialog::DoModal();
	}
	else
	{
		res = this->CreateIndirect(raw_template, pParent);
		this->MoveWindow(rc, TRUE);
	}
	_freea(raw_template);

	m_rect = rc;

	return res;
}


BEGIN_MESSAGE_MAP(i3MFCDialog, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void i3MFCDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	m_callback->OnSize(this, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

int i3MFCDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( m_callback->OnCreate(this, lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

INT_PTR i3MFCDialog::DoModal()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	return i3MFCDialog::CreateImpl(NULL, m_rect, TRUE);   
}

void i3MFCDialog::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( m_callback->OnClose() )
		CDialog::OnClose();
}
