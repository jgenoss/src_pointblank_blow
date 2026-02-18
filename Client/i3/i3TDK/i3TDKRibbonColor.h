#if !defined( __I3_TDK_RIBBON_COLOR_H)
#define __I3_TDK_RIBBON_COLOR_H

class I3_EXPORT_TDK i3TDKRibbonColor : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(i3TDKRibbonColor)

protected:
	COLORREAL		m_Color;
	CSize			m_ImageSz = { 24, 24 };
	
	HBRUSH			m_hBrush = nullptr;

protected:
	void			reset(void);

public:
	i3TDKRibbonColor() {}
	i3TDKRibbonColor(UINT nID, LPCTSTR lpszText, int nSmallImageIndex = -1, int nLargeImageIndex = -1, BOOL bAlwaysShowDescription = FALSE) : CMFCRibbonButton(nID, lpszText, nSmallImageIndex, nLargeImageIndex, bAlwaysShowDescription) {}
	i3TDKRibbonColor(UINT nID, LPCTSTR lpszText, HICON hIcon, BOOL bAlwaysShowDescription = FALSE, HICON hIconSmall = nullptr, BOOL bAutoDestroyIcon = FALSE, BOOL bAlphaBlendIcon = FALSE) : CMFCRibbonButton(nID, lpszText, hIcon, bAlwaysShowDescription, hIconSmall, bAutoDestroyIcon, bAlphaBlendIcon) {}

	virtual ~i3TDKRibbonColor(void);

	COLORREAL *		getColor(void)				{ return &m_Color; }
	void			setColor( COLORREAL * pCol);
	void			setColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a);

	virtual void DrawImage( CDC* pDC, RibbonImageType type, CRect rectImage);
	virtual CSize GetImageSize(RibbonImageType type) const;
	virtual void OnClick(CPoint point);
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
};


#endif
