#pragma once
#include "PaneBase.h"
#include "i3UIAnimation2DSprite.h"
class CAnimationViewPane : public CPaneBase
{
	DECLARE_DYNAMIC(CAnimationViewPane)
	DECLARE_MESSAGE_MAP()
	HBITMAP				m_hBitmap, m_hOldBitmap;
	HDC					m_MemDC;
	i3Texture*			m_pTexture;
	i3UIAnimation2DSprite*	m_pAnimaton;
	I3_UVINFO*			m_pItem;
	INT32				m_nAnimationIndex;

public:
	CAnimationViewPane();
	virtual ~CAnimationViewPane();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void OnDraw(HDC hdc);
	void AdjustLayout();
	void CopyToAni(i3UIAnimation2DSprite* pData);
	void SelectFrame(UVITEM* pItem);
};

