#if !defined( __CUI_SHAPE_GRIP_MANAGER_H_)
#define __CUI_SHAPE_GRIP_MANAGER_H_

#include "UIGripObjHandler.h"
#include "UIGripCtrlRect.h"

const int DEFAULT_SNAP_SIZE = 1;		// == snap size

#define GRIP_CTRL_PRESSED	1
#define GRIP_SHIFT_PRESSED	2

enum GRIP_CONTROL_MODE
{
	GCM_NONE = 0,
	GCM_MOVING,
	GCM_SIZING,
	GCM_SELECT,

	GCM_COUNT,
};

struct	SNAP_INFO
{
	POINT	m_Pos = { 0, 0 };
	INT32	m_SnapSize = 0;
};

// template의 edit를 위한 grip manager.
class CUIShapeGripManager : public i3NamedElement
{
	I3_CLASS_DEFINE(CUIShapeGripManager, i3NamedElement);

protected:
	i3ClassMeta			*	m_pControllerMeta;
	i3::vector<CUIGripCtrl*> m_SelectedList;			// CUIGripCtrl*

	bool					m_bLBDown;
	CPoint					m_ptDragStart;
	CPoint					m_ptOldPos;

	CUIGripCtrlPoint	*	m_pFocusedPoint;
	CUIGripCtrlRect		*	m_pFocusedShape;
	GRIP_CONTROL_MODE		m_mode;

	POINT					m_ScrollPos;
	POINT					m_OffsetPos;
	REAL32					m_ZoomRate;
	CUIGripCtrl*			m_pCurFocusShape;
public:
	CUIShapeGripManager(void);
	virtual ~CUIShapeGripManager(void);

	void	Reset(void);
	void	RecalcLayout(void);

protected:
	CUIGripCtrlPoint *	_hasFocusedCtrl(CPoint point);
	CUIGripCtrlRect *	_hasFocusedShape(CPoint point);

	bool				_isResizablePoint(CUIGripCtrlPoint* pPoint);
	bool				_isMovableRect(CUIGripCtrlRect* pRect);

public:
	void	Create(i3ClassMeta * pMeta);
	INT32	AddSelect(void * pElement, i3ClassMeta * pMeta = NULL, bool bReverse = false);
	INT32	RemoveSelect(void * pElement);
	void	RemoveAllSelect();

	INT32		getSelectedItemCount(void) { return (INT32)m_SelectedList.size(); }
	const i3::vector<CUIGripCtrl*>& getSelectedItemList(void) const { return m_SelectedList; }
	void		getSelectedObjList(i3::vector<i3UIControl*>& Out);

	bool	isSelected(CPoint point);
	CUIGripCtrlRect* FindElement(void* pElement);
	bool	isOnGrip(CPoint point);

	bool	OnLButtonDown(CPoint point);
	bool	OnLButtonUp(CPoint point);
	bool	OnMouseMove(CPoint point, UINT32 nFlags, CPoint* ptDelta = 0);

	void	OnKeyDown(UINT32 nKey, UINT32 nFlags);

	void	SetZoomRate(REAL32 zoom);
	void	SetScrollPos(POINT pt);
	void	SetOffsetPos(POINT pt);

	HCURSOR	getCursor(CPoint point);

	void	Draw(HDC hdc);

	//////////////////////////////////////
	// Snap
protected:
	INT32					m_SnapSize;
	i3::vector<SNAP_INFO*>	m_SnapPoints;

protected:
	POINT			GetSnappedPoint(POINT pt);

public:
	void			AddSnapPoint(POINT pt, INT32 sz);
	void			RemoveAllSnapPoint(void);

	void			SetSnapSize(INT32 size);
	INT32			GetSnapSize(void) { return m_SnapSize; }

	CPoint GetDelta() const { return m_ptDelta; }
	void SetDelta(CPoint val) { m_ptDelta = val; }
public:
	CUIGripCtrlPoint*	getFocusedPoint(void) { return m_pFocusedPoint; }

protected:
	POINT			m_ptAnchor;
	POINT			m_szAnchor;
	UINT32			m_nAnchorStyle;

	void			_FixedRatioTransform(CUIGripCtrlPoint* pPoint, CPoint pt);

	//Undo/Redo
protected:
	bool			m_bTransforming;

	void			_BeginTransform(void);
	void			_EndTransform(void);

	CPoint m_ptDelta;

	////////////////////////
	// Banding Rectangle (영역 선택할 때 마우스 Dragging에 의해 나타나는 사각형)
protected:
	CRect			m_BandRect;
	bool			m_bBanding;

	HBRUSH			m_hbrBand;

public:
	void			StartBanding(const CPoint & pt);
	void			Banding(const CPoint & pt);
	void			EndBanding(void);
	CUIGripCtrlRect*	GetFocusedShape(){return m_pFocusedShape;}
	CUIGripCtrl*		GetCurFocusShape() {return m_pCurFocusShape;}
	void			AllPenColorNormal();
	void SetFocusdItem(void* pElement);
};

#endif	//__CUI_SHAPE_GRIP_MANAGER_H_