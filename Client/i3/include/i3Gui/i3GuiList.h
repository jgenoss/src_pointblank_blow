#ifndef __I3GUI_LIST_H__
#define __I3GUI_LIST_H__

#include "i3GuiControl.h"
#include "i3GuiEditBox.h"
/*
#define LIST_BACKGROUND				8		// Lsit Control 배경
#define LIST_SCROLL					9		// Lsit Control Scroll 몸체
#define LIST_BAR					10		// Lsit Control Bar
#define LIST_SCROLL_BUTTON_UP		11		// Lsit Control 버튼(Bar 위로 이동)
#define LIST_SCROLL_BUTTON_DOWN		12		// Lsit Control 버튼(Bar 아래로 이동)
*/
#define LIST_BACKGROUND				0		// Lsit Control 배경
#define LIST_SCROLL					1		// Lsit Control Scroll 몸체
#define LIST_BAR					2		// Lsit Control Bar
#define LIST_SCROLL_BUTTON_UP		3		// Lsit Control 버튼(Bar 위로 이동)
#define LIST_SCROLL_BUTTON_DOWN		4		// Lsit Control 버튼(Bar 아래로 이동)

enum I3GUI_LIST_NOTIFY_TYPE
{
	I3GUI_LIST_NOTIFY_NULL = 0,
	I3GUI_LIST_NOTIFY_CLICKED,
	I3GUI_LIST_NOTIFY_ONMOUSE,
	I3GUI_LIST_NOTIFY_DRAG,
	I3GUI_LIST_NOTIFY_DOUBLECLICK,	
	I3GUI_LIST_NOTIFY_RCLICK,

	GUI_NOTIFY_ID_LIST,

	MAX_I3GUI_LIST_NOTIFY,
};

// ============================================================================
//
// i3GuiList : Scroll Bar가 있는 List Control
//
// ============================================================================
class I3_EXPORT_GUI i3GuiList : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiList, i3GuiControl );

protected:
	REAL32					m_fMinValue = 0.0f;
	REAL32					m_fMaxValue = 0.0f;
	REAL32					m_fScrollScale = 0.0f;
	REAL32					m_fCurScrollValue = 0.0f;
	REAL32					m_fScrollPos = 0.0f;
	INT32					m_nControlCount = 0;
	i3::vector<i3GuiEditBox*> m_ControlList;
	i3::vector<i3::rc_wstring>	m_ElementList;
	INT32					m_nElementCount = 0;
	INT32					m_nSelectedElement = 0;
	i3GuiTemplateElement*	m_pShape = nullptr;

	i3Sprite2D *			m_pSpriteSel = nullptr;

	bool					m_bOnceUpdate = false;
	INT32					m_nOriginalWidth = 0;
	INT32					m_nOriginalHeight = 0;

public:
	i3GuiList();
	virtual ~i3GuiList();  

	// Edit박스의 선택 셀
	void			SetSelColor(UINT8 r,UINT8 g,UINT8 b,UINT8 a);

	// Control을 생성합니다.
	void			CreateControl(void);
	void			CreateText(INT32 nDisplayCount = 5, i3Font * pFont = nullptr, INT32 nTextCount = 64, INT32 TextSize = 9);
	void			CreateTextEx(INT32 nDisplayCount, const i3TextFontSetInfo * pFontSetInfo, INT32 nTextCount = 64, INT32 TextSize = 9);
	void			CreateTextEx(INT32 nDisplayCount, const char* pszFont, UINT32 Charset = DEFAULT_CHARSET, INT32 nTextCount = 64, INT32 TextSize = 9);
	void			SetTextAlign( INT32 AnignX, INT32 AnignY);
	void			UpdateEditControl(void);

	// Slider의 현재 수치에 관련된 함수입니다.
	REAL32			getMinValue()							{ return m_fMinValue;						}
	REAL32			getMaxValue()							{ return m_fMaxValue;						}
	void			setMinMaxValue(REAL32 fMin, REAL32 fMax){ m_fMinValue = fMin; m_fMaxValue = fMax;	}

	void			SetSliderPos( REAL32 fPos);
	void			UpdatePos();

	REAL32			getCurValue()							{ return m_fCurScrollValue;					}
	void			SetCurValue( REAL32 fValue);

	void			AddElement( const i3::rc_wstring& wstr);
	void			AddHeadElement(const i3::rc_wstring& wstr);
	INT32			GetElementCount( void)					{ return (INT32)m_ElementList.size();			}
	void			UpdateElement(void);

	// List Box에서 선택되어진 최종 Element의 인덱스입니다.
	INT32			getSelectedElement(void)				{ return m_nSelectedElement;				}
	void			setSelectedElement(INT32 nIdx)			{ m_nSelectedElement = nIdx;				}

	//
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;

	virtual bool	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	// 추가합니다
	const i3::rc_wstring&	GetText(UINT32 idxElement) const;
	void			SetText(UINT32 idxElement, const i3::rc_wstring& str);
	void			SetTextColor(const COLOR* pColor);
	void			SetTextColor(INT32 idxControl,const COLOR* pColor);
	void			Clear(void);
	UINT32			GetCurrentTop(void);
	void			MovePrev(REAL32 fX, REAL32 fY, UINT32 state);
	void			MoveNext(REAL32 fX, REAL32 fY, UINT32 state);
	void			MovePrev(void);
	void			MoveNext(void);

	void			onceUpdate(void);

private:
	bool			_DragScroll(REAL32 fX,REAL32 fY,UINT32 state);	// 드래깅중이면 true, 아니면 false

protected:
	virtual bool	_ResizeControl(void) override;
	void			_ResizeList(void);
};

#endif //__I3GUI_LIST_H__
