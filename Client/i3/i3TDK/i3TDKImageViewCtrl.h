#pragma once

#include "stdafx.h"
#include "i3Gfx.h"

class I3_EXPORT_TDK i3TDKImageViewCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKImageViewCtrl)

public:
	enum VIEW_MODE
	{
		VIEW_RGB = 0,
		VIEW_BLEND,
		VIEW_R,
		VIEW_G,
		VIEW_B,
		VIEW_A
	};

	i3TDKImageViewCtrl() {}
	i3TDKImageViewCtrl(i3Texture * pTexture);
	virtual ~i3TDKImageViewCtrl();

protected:
	i3Texture*			m_pTexture = nullptr;		//현재로서 보여지게되는 텍스쳐
	i3ImageFile			m_pImageFile;

	//읽어온 이미지용
	HBITMAP				m_hBitmap = nullptr, m_hOldBitmap = nullptr;
	HDC					m_MemDC = nullptr;
	
	POINT				m_posScroll = { 0, 0 };
	REAL32				m_Zoom = 1.0f;
	CSize				m_WndSize = { 0, 0 };

	//RightDrag
	BOOL				m_RButtonDown = FALSE;
	POINT				m_DragStart = { 0, 0 };

	VIEW_MODE			m_ViewMode = VIEW_BLEND;

	//생성
public:
	BOOL		Create( DWORD	dwStyle , CWnd* pParentWnd,UINT	nID	);

	//이미지 load
public:
	void		SetImage(i3Texture * pTexture);			//이미지 텍스쳐 설정
	void		SetImage();
	i3Texture*	GetImage(){	return m_pTexture;}

	void		OpenImageFile();						//파일 오픈	   (fileopen 대화상자 열림)
	void		LoadImageFile(const TCHAR* lpstrFile);			//파일 읽어오기(filename 주어져야함)
	BOOL		CheckFileName(const TCHAR* FileName) const;

	VIEW_MODE	GetViewMode(void)								{ return m_ViewMode; }
	void		SetViewMode( VIEW_MODE mode);
protected:
	void		SetGridRect(HDC hdc ,int x , int y ,  INT32 Width , INT32 Height);

	void		Scroll(int nBar , int ScrollAmount , LONG& pos);
	void		ResetScroll(int nBar , LONG& pos);
	void		SetScroll();

public:
	void		DeleteImage();		//DC와 hBitmap의 Clear

	virtual void	OnDraw( HDC dc);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};