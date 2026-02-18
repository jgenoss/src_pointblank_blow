#pragma once


// CDlg_VTexView dialog

class CDlg_VTexView : public CDialog
{
	DECLARE_DYNAMIC(CDlg_VTexView)

protected:
	i3VirtualTexture *		m_pTex;

	HDC				m_hDC, m_hWndDC;
	HBITMAP			m_hBitmap, m_hOldBitmap;
	INT32			m_BitmapCX, m_BitmapCY;
	INT32			m_WndCX, m_WndCY;
	char *			m_pPixelData;

	INT32			m_idxShow, m_idxOld;

public:
	void			setVTex( i3VirtualTexture * pTex);

public:
	CDlg_VTexView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_VTexView();

// Dialog Data
	enum { IDD = IDD_DLG_VTEXVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
