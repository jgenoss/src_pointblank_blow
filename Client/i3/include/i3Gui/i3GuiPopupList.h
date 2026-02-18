#ifndef __I3GUI_POPUP_LIST_H__
#define __I3GUI_POPUP_LIST_H__

#include "i3GuiControl.h"

enum I3GUI_POPUPLIST_NOTIFY_TYPE
{
	I3GUI_POPUPLIST_NOTIFY_CLICKED,	
	I3GUI_POPUPLIST_NOTIFY_ONMOUSE,	
	I3GUI_POPUPLIST_NOTIFY_DOUBLECLICK,
	I3GUI_POPUPLIST_NOTIFY_RCLICK,

	MAX_I3GUI_POPUPLIST_NOTIFY
};

// ============================================================================
//
// i3GuiPopupList : 팝업 리스트
//
// ============================================================================
class I3_EXPORT_GUI i3GuiPopupList : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiPopupList, i3GuiControl );

protected:
	i3::vector<i3::rc_wstring>	m_ElementList;
	INT32						m_nElementCount = 0;

	i3::vector<class i3GuiEditBox*> m_ControlList;
	INT32					m_nControlCount = 0;

	INT32					m_nSelectedElement = 0;

	i3GuiTemplateElement*	m_pShape = nullptr;
	i3Sprite2D *			m_pSpriteSel = nullptr;

	i3TextFontSetInfo		m_FontSetInfo;
	i3Font *				m_pFont = nullptr;
	INT32					m_nTextCount = 0;
	INT32					m_nTextSize = -1;
	COLOR					m_SelColor = { 96, 196, 213, 255 };

	INT32					m_nOriginalWidth = 0;
	INT32					m_nOriginalHeight = 0;
	bool					m_bOnceUpdate = false;

	bool					m_bFixedElementCount = false;// 항목수에 따라 가변하지 않는 고정폭 팝업리스트를 위한 플래그

public:
	i3GuiPopupList();
	virtual ~i3GuiPopupList();  

	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;

	virtual bool	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	void			AddElement( const i3::rc_wstring& wstrTemp);
	
	INT32			GetElementCount( void)					{ return (INT32)m_ElementList.size();			}
	void			UpdateElement(void);

	// Edit박스의 선택 셀
	void			SetSelColor(UINT8 r,UINT8 g,UINT8 b,UINT8 a);

	// Control을 생성합니다.
	void			CreateControl(void);
	void			CreateText( INT32 nDisplayCount = 3, i3Font * pFont = nullptr, INT32 nTextCount = 64, INT32 nTextSize = -1);
	void			CreateTextEx( INT32 nDisplayCount, const char* pszFont, UINT32 CharSet = DEFAULT_CHARSET, INT32 nTextCount = 64, INT32 nTextSize = -1);
	void			CreateTextEx( INT32 nDisplayCount, const i3TextFontSetInfo * pFontSet, INT32 nTextCount = 64, INT32 nTextSize = -1);
	void			SetDisplayCount(INT32 nDisplayCount);
	void			SetTextAlign( INT32 AnignX, INT32 AnignY);
	void			UpdateEditControl(void);
	INT32			GetTextHeight(void);
	INT32			GetTextWidth(void);

	// List Box에서 선택되어진 최종 Element의 인덱스입니다.
	INT32			getSelectedElement(void)				{ return m_nSelectedElement;				}
	void			SetSelectedElement(INT32 nIdx, bool bOnEvent = true);


	// Element 관련 추가
	const i3::rc_wstring&		GetText(UINT32 idxElement) const;
	void			SetText(UINT32 idxElement, const i3::rc_wstring& wstr);
	void			SetTextColor(const COLOR* pColor);
	void			SetTextColor(INT32 idxElement,const COLOR* pColor);

	void			Clear(void);

	void			SetFixedElementCount(bool bFixed);
	bool			GetFixedElementCount(void) const;

protected:
	virtual bool	_ResizeControl(void) override;
	void			_ResizePopUp(void);
};

#endif //__I3GUI_POPUP_LIST_H__
