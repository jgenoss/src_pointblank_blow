#pragma once

#include "i3UICaptionControl.h"

typedef UINT32 I3UI_COMBO_STYLE;
#define I3UI_COMBO_STYLE_EDITREADONLY	0x00000001		// EditBox의 readonly flag
#define I3UI_COMBO_STYLE_LISTBOXDIRUP	0x00000002		// ListBox의 위치(1이면 위에 붙는다.)


class i3UIListBox;

class I3_EXPORT_FRAMEWORK i3UIComboBase : public i3UICaptionControl
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3UIComboBase, i3UICaptionControl);

public:
	i3UIComboBase();
	virtual ~i3UIComboBase();

	void			addComboStyle( I3UI_COMBO_STYLE style)		{ m_ComboStyle |= style; }
	void			removeComboStyle( I3UI_COMBO_STYLE style)	{ m_ComboStyle &= ~style; }
	bool			isComboStyle( I3UI_COMBO_STYLE style)		{ return (m_ComboStyle & style) == style; }

	virtual void	SetTextColor( COLOR * pCol) override;
	virtual void	SetCaption( const WCHAR16* pszCaption) override;
	virtual void	SetTextAlign( I3UI_TEXT_ALIGN_X AlignX, I3UI_TEXT_ALIGN_Y AlignY) override;
	virtual void	SetTextSpaceofLetters( INT32 nX, INT32 nY) override;
	virtual void	SetTextMargin( INT32 rMarginX,INT32 rMarginY) override;

	void			SetComboBoxTextColor( COLOR * pCol);
	void			BackupTextColor();
	void			RestoreTextColor();

	void			Clear( void);

	void			SetItems( const char* pszItems);
	void			GetItems( char* pBuf, INT32 nBufSize);

	void			SetCurSel( INT32 nIdx);
	INT32			GetCurSel( void);
	void			GetCurSelString( WCHAR16 * pOut);

	i3UIListBox*		getListBox( void) const	{	return m_pListBox;	}
	i3UICaptionControl*	getDerivedTextControl() const { return m_DerivedTextCtrl; }

	bool			isListBoxUnfolded(void);		// 콤보가열렸는지 질의함수
	bool			FoldListBox(void);				// 실제로 액션이 일어나면 true, 이미 처리됬거나 처리불가능할때 false
	bool			UnfoldListBox(void);			// 실제로 액션이 일어나면 true, 이미 처리됬거나 처리불가능할때 false

	void			SetButtonhHeight(REAL32 rHeight)	{ m_fDefHeightButton = rHeight; }
	REAL32			GetButtonhHeight(void)				{ return m_fDefHeightButton; }

	// ================= Inherited members =================
	virtual void	OnInitAfterLoad() override;
	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnBindTemplate(void) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr) override;
	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	virtual bool	isOnMouse(  I3UI_OVERLAP_INFO * pInfo) override;

	REAL32			getMinimumHeight( void);

	static void		RegisterLuaFunction(LuaState * pState);

private:

	virtual void	_UpdateComponentSize_DerivedCombo(REAL32 cx, REAL32 cy) = 0;
	virtual void	_RemoveListBox_DerivedCombo() = 0;
	virtual void	_SetFocusEditBoxIfExists_DerivedCombo() = 0;
	virtual void	_OnInitAfterLoad_DerivedCombo() = 0;
	virtual void	_OnEnabled_DerivedCombo(bool bEnable) = 0;
	virtual void	_OnBindTemplate_DerivedCombo() = 0;
	virtual bool	_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const = 0;

	virtual UINT32	_OnLoadMore( i3ResourceFile * pResFile, UINT32 parentFileID) = 0;

#if defined( I3_DEBUG)	// Tool에서만 사용
	virtual void	_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List) = 0;
	virtual bool	_OnTDKPropertyChanged_DerivedCombo(const char* szName) = 0;
	virtual void	_OnTDKPropertyChanged_DerivedColor(COLOR pColor) {}
#endif
	

protected:
	virtual void	_UpdateShapeState(void) override;
	void			_UpdateComponentSize(REAL32 cx, REAL32 cy);
	void			_AddListBox( void);
	void			_RemoveListBox( void);
	
	// persistant members
	I3UI_COMBO_STYLE	m_ComboStyle = 0;
	i3UIListBox*		m_pListBox;						// [initialize at constructor]

	REAL32				m_fDefHeightButton = 22.0f;		// Button Height
	REAL32				m_fListBoxHeight = 100.0f;
	INT32				m_nCurSel = -1;
	UINT32				m_BackupTextColor = 0;

	/*파생클래스에 이미 존재할것인 텍스트콘트롤 포인터를 받아둔다..
	(4바이트 낭비이지만, 가상함수를 그만큼 제낄수 있다.) */
	i3UICaptionControl*	m_DerivedTextCtrl = nullptr;		

	// Test Automation 관련
public:
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl);
};
