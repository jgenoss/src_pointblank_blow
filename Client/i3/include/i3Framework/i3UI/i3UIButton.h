#ifndef __I3UI_BUTTON_H__
#define __I3UI_BUTTON_H__

#include "i3UICaptionControl.h"

class I3_EXPORT_FRAMEWORK i3UIButton : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIButton, i3UICaptionControl);
	friend class i3UIListView_Header;

public:
	enum I3UIBUTTON_COLOR_TYPE
	{
		I3UIBUTTON_CR_IDLE		= 0,
		I3UIBUTTON_CR_SELECTED,
		I3UIBUTTON_CR_ONMOUSE,
		I3UIBUTTON_CR_DISABLE,
		I3UIBUTTON_CR_SELECTEDNORMAL,

		MAX_I3UIBUTTON_COLOR,
	};

private:
	bool			m_bSelectedNormalState = false;

protected:
	
	REAL32			m_PassedTime = 0.0f;
	UINT32			m_CurShape;				// [initialize at constructor]
	bool			m_bUIControl = false;
	bool			m_bSkipUnselect = false;
	bool			m_bSkipUnselect_PushedCurrSel = false;

	COLOR m_ButtonTextColor[ MAX_I3UIBUTTON_COLOR ];

	virtual void	_UpdateShapeState(void);

public:
	void			SetUIControl(bool UIC){ m_bUIControl = UIC;};
	bool			GetUIControl(){ return m_bUIControl;};
	void			SetButtonShape(UINT32 Button_Shape) { m_CurShape = Button_Shape; };
	void			SkipUnselect(bool bEnable) { m_bSkipUnselect = bEnable; }
	bool			IsSkipUnselect() const { return m_bSkipUnselect;    }

public:
	UINT32		GetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type );
	void		SetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type, UINT32 uColor );
	void		SetButtonTextColor( I3UIBUTTON_COLOR_TYPE _type, const COLOR& cr );

	// 상태 구분없이 모두 동일한 텍스트 색상을 처리할때 별도 이름 함수로 처리.
	void		SetButtonTextColorAll(const COLOR& cr);
	void		SetButtonTextColorAll(UINT32 uColor);

	// 오버로딩 가림 처리..( SetTextColor의 기본함수에 근접 못하게 해야한다..)
	void	SetTextColor( I3UIBUTTON_COLOR_TYPE _type, UINT32 uColor);
	void	SetTextColor( I3UIBUTTON_COLOR_TYPE _type, COLOR* pCol);

	void			UnSelectedNormalState();
	bool			IsSelectedNormalState() const { return m_bSelectedNormalState; }

	UINT32			getCurShape()	{return m_CurShape;}

	void RemoveOnMouse();

	void set_disable_text_color(const COLOR & color);
	void disable_btn(); //입력 비활성화 및 텍스트 색상 변경(set_disable_text_color 함수에 등록된 색상)
	void enable_btn(); //입력 활성화 및 텍스트 색상 변경(원래 색상)

public:
	i3UIButton();
	virtual ~i3UIButton(); 
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr) override;
	virtual bool	OnSetFocus() override;

	virtual void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual i3UIControl*	OnSelect( void) override;
	virtual bool	GetTextRect( i3::pack::RECT* pRect) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	Init( void) override;

	virtual INT32	OnCommand(LuaState * pState) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif
	static void i3UIButton::RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_BUTTON_H__
