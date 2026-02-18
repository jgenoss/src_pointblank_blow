#ifndef __I3GUI_POPUP_LIST_WITH_SCROLL_H__
#define __I3GUI_POPUP_LIST_WITH_SCROLL_H__

#include "i3GuiControl.h"
#include "i3GuiPopupList.h"

enum I3GUI_LISTPOPUP_SCROLL_SHAPE
{	
	I3GUI_CTRL_LISTPOPUP_SCROLL_BG = 9,
	I3GUI_CTRL_LISTPOPUP_SCROLL_DOWN,
	I3GUI_CTRL_LISTPOPUP_SCROLL_UP,
	I3GUI_CTRL_LISTPOPUP_SCROLL_BAR,
};


// ============================================================================
//
// i3GuiPopupListWithScroll : 팝업 리스트 with scroll
//
// ============================================================================
class I3_EXPORT_GUI i3GuiPopupListWithScroll : public i3GuiPopupList
{
	I3_EXPORT_CLASS_DEFINE( i3GuiPopupListWithScroll, i3GuiPopupList );

	//
	i3::vector<COLOR>	m_ElementColorList;

	//팝업 리스트 사이즈 에서 가질수 있는 최대 Edit Control 수. ( == total item count)
	INT32 m_nMaxControlCount = 0;
	
	//scroll variable
	bool m_bEnableScroll = false;
	REAL32 m_fCurScrollPos = 0.0f;		// 0 ~ (m_fMaxArea - m_fMinArea)
	REAL32 m_fMinArea = 0.0f;			// Bar가 갈 수 있는 최소 값.
	REAL32 m_fMaxArea = 0.0f;			// Bar가 갈 수 있는 최대 값.
	REAL32 m_fMoveSpeed = 1.0f;		// Bar가 한칸 움직이는 거리.
	
	bool							m_bBarDrag = false;				//
	bool							m_bClickedButton = false;		//
	
	REAL32							m_fFirstUpdateTime = 0.0f;		//
	REAL32							m_fUpdateTime = 0.0f;			//
	REAL32							m_fUpdatePeriod = 0.1f;		//
	
	I3GUI_LISTPOPUP_SCROLL_SHAPE	m_eCurSelectedShape = I3GUI_CTRL_LISTPOPUP_SCROLL_BG;	// Clicked된 Shape
	I3GUI_LISTPOPUP_SCROLL_SHAPE	_GetScrollCtrlByMousePos( REAL32 fX, REAL32 fY);
	INT32 _GetScrollBarIdx();

	bool PtInRect( RECT* rc, POINT pt);
	
public:
	virtual ~i3GuiPopupListWithScroll();  

	//
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	//
	virtual void	UserInput(REAL32 fX, REAL32 fY, REAL32 fDeltaZ, UINT32 state);

	// Create Edit Control
	virtual void	CreateControl(void);

	// Total Element Count
	virtual void	SetDisplayCount( INT32 nDisplayCount);

	//
	virtual void	AddElement( const i3::rc_wstring& wstrTemp);
	virtual void	UpdateElement(void);

	//
	virtual void	UpdateEditControl(void);
	virtual void	SetTextColor(const COLOR* pColor);
	virtual void	SetTextColor(INT32 idxElement,const COLOR* pColor);
	virtual void	SetSelectedElement(INT32 nIdx, bool bOnEvent = true);

	//
	virtual void	Clear(void);

	//
	virtual INT32	GetEditControlCount() const { return m_nMaxControlCount;}


	//.........................................//
	//WithScroll member//

	// Up, Down버튼을 계속 누르고 있을 경우 스크롤 시간.
	void SetScrollUpdateTime( REAL32 fTime = 0.1f) { m_fUpdatePeriod = fTime;}

protected:
	virtual void	_ResizePopUp(void);

};

#endif //__I3GUI_POPUP_LIST_WITH_SCROLL_H__
