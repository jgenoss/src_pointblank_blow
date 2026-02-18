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
	I3_CLASS_DEFINE( i3GuiPopupList );

protected:
	i3List					m_ElementList;
	INT32					m_nElementCount;

	i3List					m_ControlList;
	INT32					m_nControlCount;

	INT32					m_nSelectedElement;

	i3GuiTemplateElement*	m_pShape;
	i3Sprite2D *			m_pSpriteSel;

	char					m_szFont[256];
	i3Font *				m_pFont;
	INT32					m_nTextCount;
	COLOR					m_SelColor;

	INT32					m_nOriginalWidth;
	INT32					m_nOriginalHeight;
	BOOL					m_bOnceUpdate;

	BOOL					m_bFixedElementCount;

public:
	i3GuiPopupList();
	virtual ~i3GuiPopupList();  

	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape);
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );

	virtual BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	void			AddElement( char *pszTemp);
	INT32			GetElementCount( void)					{ return m_ElementList.GetCount();			}
	void			UpdateElement(void);

	// Edit박스의 선택 셀
	void			SetSelColor(UINT8 r,UINT8 g,UINT8 b,UINT8 a);

	// Control을 생성합니다.
	void			CreateControl(void);
	void			CreateText( INT32 nDisplayCount = 3, i3Font * pFont = NULL, INT32 nTextCount = 64);
	void			CreateTextEx( INT32 nDisplayCount, const char* pszFont, INT32 nTextCount = 64);
	void			SetDisplayCount(INT32 nDisplayCount);
	void			SetTextAlign( INT32 AnignX, INT32 AnignY);
	void			UpdateEditControl(void);

	// List Box에서 선택되어진 최종 Element의 인덱스입니다.
	INT32			getSelectedElement(void)				{ return m_nSelectedElement;				}
	void			SetSelectedElement(INT32 nIdx, BOOL bOnEvent = TRUE);


	// Element 관련 추가
	const char*		GetText(UINT32 idxElement);
	void			SetText(UINT32 idxElement,const char* strText);
	void			SetTextColor(const COLOR* pColor);
	void			SetTextColor(INT32 idxElement,const COLOR* pColor);

	void			Clear(void);

	void			SetFixedElementCount(BOOL bFixed);
	BOOL			GetFixedElementCount(void) const;

protected:
	virtual BOOL	_ResizeControl(void);
	void			_ResizePopUp(void);
};

#endif //__I3GUI_POPUP_LIST_H__
