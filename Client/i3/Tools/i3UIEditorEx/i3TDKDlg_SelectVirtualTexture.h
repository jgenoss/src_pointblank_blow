#pragma once

#include "i3Base/string/compare/generic_is_iless.h"

#pragma pack( push, 4)

namespace i3tool
{
	namespace pack
	{
		struct VTEX_THUMB
		{
			INT32	m_Code[4] = { 0 };			// MD5로 generating합니다.
			char	m_szName[256] = { 0 };
			//	INT32	m_Size;
			//	INT64	m_Offset;

			HBITMAP	m_hBitmap = nullptr;
			INT32	m_Width = 0;
			INT32	m_Height = 0;
		};
	}
}

#pragma pack( pop)

struct I3TDK_VTEX_VISIBLE_TEX
{
	INT32	m_idxThumb = 0;
	INT32	m_idxTexInfo = 0;
};

#define		MAX_THUMB_POOL		64

// i3TDKDlg_SelectVirtualTexture
template<> struct i3::less<i3tool::pack::VTEX_THUMB*>
{
	bool comp( const INT32 * pCode1, const INT32 * pCode2) const 
	{
		if( pCode1[0] > pCode2[0]) return false;
		else if( pCode1[0] < pCode2[0])	return true;
		if( pCode1[1] > pCode2[1]) return false;
		else if( pCode1[1] < pCode2[1]) return true;
		if( pCode1[2] > pCode2[2]) return false;
		else if( pCode1[2] < pCode2[2]) return true;
		if( pCode1[3] > pCode2[3]) return false;
		else if( pCode1[3] < pCode2[3])	return true;
		return false;
	}
	bool operator()( const i3tool::pack::VTEX_THUMB * p1, const i3tool::pack::VTEX_THUMB * p2 ) const {	return comp( p1->m_Code, p2->m_Code); }
	bool operator()( const i3tool::pack::VTEX_THUMB * p1, const INT32 * pCode2) const { return comp(p1->m_Code, pCode2); }
	bool operator()( const INT32 * pCode1, const i3tool::pack::VTEX_THUMB * p2) const { return comp(pCode1, p2->m_Code); }
};

struct I3VT_TEXINFO_Comp
{
	bool	operator()( const I3VT_TEXINFO * pInfo1, const I3VT_TEXINFO * pInfo2) const
	{
		return i3::generic_is_iless( pInfo1->m_strName, pInfo2->m_strName);		// , MAX_PATH -1)
	}
};

class i3TDKDlg_SelectVirtualTexture : public CWnd
{
	DECLARE_DYNAMIC(i3TDKDlg_SelectVirtualTexture)
protected:
	i3VirtualTexture *		m_pVTex;

	i3::vector_set< i3tool::pack::VTEX_THUMB* >		m_TexList;						// 유일값으로 우선 판단하고 교체..
	CSize					m_WndSz;

	I3TDK_VTEX_VISIBLE_TEX	m_VisibleTex[MAX_THUMB_POOL];
	INT32					m_cntVisible;

	INT32					m_ShowY;
	INT32					m_ExtentY;

	INT32					m_ViewUnit;
	INT32					m_idxSelected;

	HFONT					m_hFont, m_hOldFont;

	HDC						m_hDC, m_hCopyDC;
	HBITMAP					m_hBitmap, m_hOldBitmap;
	HBRUSH					m_hbrBack, m_hbrSelect;

	i3::vector<I3VT_TEXINFO*>	m_RefList;

protected:

	INT32				FindThumb( const char * pszName);
	INT32				PrepareThumb( I3VT_TEXINFO * pTexInfo);
	void				RemoveAllThumbs(void);
	i3tool::pack::VTEX_THUMB *	getThumb( INT32 idx)				{ return m_TexList.get_vector()[idx]; }

	void				_GenThumbImage( i3tool::pack::VTEX_THUMB * pInfo, I3VT_TEXINFO * pTexInfo);

protected:
	INT32				getTexCount(void)				{ return (INT32)m_TexList.size(); }
	i3tool::pack::VTEX_THUMB *	getTex( INT32 idx)				{ return m_TexList.get_vector()[idx]; }
	INT32				FindTexByPos( POINT pt);

	void				GetImageRect( INT32 idx, CRect * pRect);
	void				PrepareVisibleTexture(void);
	void				DrawImage( HDC dc, CRect * pRect, I3VT_TEXINFO * pTexInfo, i3tool::pack::VTEX_THUMB * pThumb, bool bSelected);
	void				Draw( HDC dc);

	void				SetupScroll(void);
	void				UpdateScroll(void);

	
	I3VT_TEXINFO *		getRefTex( INT32 idx)				{ return m_RefList[idx]; }
	INT32				getRefTexCount(void) { return (INT32)m_RefList.size(); }

public:
	i3TDKDlg_SelectVirtualTexture();
	virtual ~i3TDKDlg_SelectVirtualTexture();

	

	bool			Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);
	void			SetVirtualTexture( i3VirtualTexture * pVTex);

	void			ModifyThumb(CProgressCtrl* pProgress);

	void			SetRefTextureList(const i3::vector_multiset<I3VT_TEXINFO*, struct I3VT_TEXINFO_Comp>& List);

	I3VT_TEXINFO *			getSelectedRefTexture(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


