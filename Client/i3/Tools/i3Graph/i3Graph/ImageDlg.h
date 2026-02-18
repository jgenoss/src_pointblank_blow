#pragma once
// CImageDlg dialog
#include "resource.h"
#include "stdafx.h"
#include "afxwin.h"

enum BLTMODE	{

	BLT_NORMAL = 0,
	BLT_ALPHABLEND
};

enum SIZINGMODE	{

	SIZINGMODE_CUSTOM = 0,
	SIZINGMODE_FIXIMAGE
};

class CImageDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageDlg)

public:
	CImageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageDlg();

private:
	i3Texture*			m_pTexture;		//현재로서 보여지게되는 텍스쳐
	i3ImageFile			m_pImageFile;
	CSize				WndSize;
	CSize				m_nTotalSize;
	BLTMODE				m_bBltMode;
	INT32				m_nLevelCount;

	//읽어온 이미지용
	HBITMAP				m_hBitmap[MAX_MIPMAP_LEVEL], m_hOldBitmap[MAX_MIPMAP_LEVEL];
	HDC					m_MemDC[MAX_MIPMAP_LEVEL];

	//격자 출력용
	HBITMAP				m_hPaintBitmap, m_hOldPaintBitmap;
	HDC					m_PaintDC;

	POINT				m_posScroll;

	REAL32				m_Zoom;
	//RightDrag
	BOOL				m_RButtonDown;
	POINT				m_DragStart;
	enum { IDD = IDD_DIALOG_IMAGE };

public:
	BOOL				m_bChanged;
private:
//	void	ReSetWindowSize();

public:
	void		SetMipMapImage(i3Texture * pTexture);
//	void		SetImage(i3Texture * pTexture);
	void		SetImage(INT32	nLevel , i3Texture * pTexture);

	i3Texture*	GetImage(){	return m_pTexture;}
	void		SetTexture(i3Texture * pTexture)
	{ 
		if( pTexture != NULL)
			pTexture->AddRef();

		I3_SAFE_RELEASE( m_pTexture);
		m_pTexture = pTexture;
	}
	void		SetGridRect(HDC hdc ,int x , int y ,  INT32 Width , INT32 Height);

	void		Scroll(int nBar , int ScrollAmount , LONG& pos);
	void		ResetScroll(int nBar , LONG& pos);
	void		SetScroll();

public:
	void	DeleteImage();		//DC와 hBitmap의 Clear

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	afx_msg void OnPaint();
public:
	BOOL ImportImage( const char *szPath );
	afx_msg void OnFileApplyimg();
	afx_msg void OnFileSaveimg();
	afx_msg void OnFileImport();
	afx_msg void OnFileExitimg();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnFileImportAsExternal();
};
