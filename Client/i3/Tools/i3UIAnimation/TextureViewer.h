#pragma once
#include "i3TDKImageViewCtrl.h"
#include "i3TDKUpdateSystem.h"
#include "Control/UIShapeGripManager.h"

class CUIGripCtrl;
enum RECT_EXPANDTYPE {
	RECTEXPAND_LEFT = 0,
	RECTEXPAND_RIGHT,
	RECTEXPAND_TOP,
	RECTEXPAND_BOTTOM,
};
class CTextureViewer : public i3TDKImageViewCtrl
{
	DECLARE_DYNAMIC(CTextureViewer)

	i3ClassMeta	*			m_pControllerMeta;
	POINT					m_ptOldPos;
	bool					m_bLBDown;
	CUIShapeGripManager		m_GripManager;
	i3::vector<RECT*>		m_listRect;
	COLORREAL				m_ColorKey;
	POINT					m_LDragStart;

protected:
	DECLARE_MESSAGE_MAP()
public:
	CTextureViewer();
	virtual ~CTextureViewer();

	virtual void OnDraw(HDC dc) override;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void	DisplayTex(i3::string sItemPath);
	void	AddItem(UVITEM* pItem);
	void	RemoveAllItem();
	CPoint	_AbsToRel(CPoint ptAbs);
	void	UpdateScroll(bool bVert, UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void	AdjustShowXY();
	void	SetFocusdItem(i3UIControl* pControl);
	void	AnalysisPickImage();
	void	AnalysisImage();
	bool	isSameColor(COLORREAL * pCol1, COLORREAL * pKey);
	INT32	ExpandDown(INT32 idx);
	INT32	ExpandRight(INT32 idx);
	INT32	FindBefore(POINT *pt);
	RECT*	MakeNewRect(POINT pos);
	void	SaveColorKey();
	void	LoadColorKey();
	void	UpdateInvaliDate();
	void	RectExpend(RECT *rt, RECT_EXPANDTYPE type);
	bool	CalculRect(i3Texture * pTexture, RECT *rt);

};

