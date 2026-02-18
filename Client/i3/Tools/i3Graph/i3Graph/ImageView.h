#pragma once

#include "stdafx.h"


class CImageView : public CWnd
{
	DECLARE_DYNAMIC(CImageView)

public:
	CImageView();
	CImageView(i3Texture * pTexture);
	virtual ~CImageView();
	

private:
	i3Texture*			m_pTexture;		//현재로서 보여지게되는 텍스쳐
	i3ImageFile			m_pImageFile;

	BOOL				m_bAlphaMode;

	//읽어온 이미지용
	HBITMAP				m_hBitmap, m_hOldBitmap;
	HDC					m_MemDC;
	//격자 출력용
	HBITMAP				m_hPaintBitmap, m_hOldPaintBitmap;
	HDC					m_PaintDC;

	COLORREF			m_AlphaValue;

	POINT				m_posScroll;
	REAL32				m_Zoom;
	//RightDrag
	BOOL				m_RButtonDown;
	POINT				m_DragStart;

	//생성
public:
	BOOL		Create( DWORD	dwStyle , CWnd* pParentWnd,UINT	nID	);

	//이미지 load
public:
	void		SetImage(i3Texture * pTexture);			//이미지 텍스쳐 설정
	i3Texture*	GetImage(){	return m_pTexture;}

	void		OpenImageFile();						//파일 오픈	   (fileopen 대화상자 열림)
	void		LoadImageFile(char* lpstrFile);			//파일 읽어오기(filename 주어져야함)
	BOOL		CheckFileName(char * FileName);

private:
	void		SetGridRect(HDC hdc ,int x , int y ,  INT32 Width , INT32 Height);
	void		MakeDCforDraw();

	void		Scroll(int nBar , int ScrollAmount , LONG& pos);
	void		ResetScroll(int nBar , LONG& pos);
	void		SetScroll();

public:
	void		DeleteImage();		//DC와 hBitmap의 Clear

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
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};