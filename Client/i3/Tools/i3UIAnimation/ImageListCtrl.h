#pragma once

class CImageListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CImageListCtrl)

	CImageList m_ImageList;
protected:
	DECLARE_MESSAGE_MAP()

	HBITMAP				m_hBitmap, m_hOldBitmap;
	HDC					m_MemDC;
	i3Texture*			m_pTexture;

	POINT				m_ImageSize;
	bool				m_bImageSet;
public:
	CImageListCtrl();
	~CImageListCtrl();

	INT32			AddImage(UVITEM* pUVItem);
	HBITMAP			CreateImage(UVITEM* pUVItem);
	void			UpdateImage(UVITEM* pUVItem, INT32 nIndex);

	void			Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, POINT ImageSize);
	virtual int		InsertItem(UVITEM* pUVItem, LVITEM* pControlItem);
	virtual BOOL	DeleteAllItems();
	void			SetImageSize(POINT pt);

	afx_msg void OnSize(UINT nType, int cx, int cy);

};

