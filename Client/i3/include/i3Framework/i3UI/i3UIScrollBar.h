#ifndef __I3UI_SCROLLBAR_H__
#define __I3UI_SCROLLBAR_H__

#include "i3UIControl.h"
#include "i3UITemplate_ScrollBar.h"
#include "i3UIButton.h"

class I3_EXPORT_FRAMEWORK i3UIScrollBar : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIScrollBar, i3UIControl );
protected:
	// Persist
	I3UI_SCROLLBAR_STYLE		m_ScrollType = I3UI_SCROLLBAR_STYLE_VERT;

	INT32			m_ValueMin = 0;
	INT32			m_ValueMax = 100;
	INT32			m_ValuePage = 30;	// Page 단위로 이동할 단위( => 막대기의 크기 결정)
	INT32			m_ValueTick = 5;	// Prev, Next 버튼을 눌렀을 때 이동할 단위

	// Volatile
	INT32			m_CurValue = 0;
	REAL32			m_PageSizeInPx = 0.0f;

	i3UIButton *	m_pBtnPrev = nullptr;
	i3UIButton *	m_pBtnNext = nullptr;
	i3UIButton *	m_pBtnTrack = nullptr;

	INT32			m_nDragStartPos = 0;
	INT32			m_nDragStartX = 0;
	INT32			m_nDragStartY = 0;

	bool			m_bDisablePrevNextButton = false;

protected:
	virtual void	_UpdateShapeState(void) override;

	void			_UpdateScrollBar(bool bForce = false);
	void			_OnPrevPushed( void);
	void			_OnNextPushed( void);
	void			_OnTrackDragStart( INT32 x, INT32 y);
	void			_OnTrackDragging( INT32 x, INT32 y);
public:
	virtual ~i3UIScrollBar(); 
	
	void			setDisablePrevNextButton(bool disable)		{ m_bDisablePrevNextButton = disable; }
	bool			getDisablePrevNextButton()					{ return m_bDisablePrevNextButton;    }

	I3UI_SCROLLBAR_STYLE		getScrollType(void)				{ return m_ScrollType; }
	void						setScrollType( I3UI_SCROLLBAR_STYLE typE);

	void			getScrollRange( INT32 * pMin, INT32 * pMax)
	{
		* pMin = m_ValueMin;
		* pMax = m_ValueMax;
	}
	void			setScrollRange( INT32 minValue, INT32 maxValue);

	INT32			getScrollLimit(void)						{ return m_ValueMax; }
	
	INT32			getScrollPos(void)							{ return m_CurValue; }
	void			setScrollPos( INT32 pos);

	INT32			getPageSize(void)							{ return m_ValuePage; }
	void			setPageSize( INT32 val);

	INT32			getTickSize( void)							{ return m_ValueTick;	}
	void			setTickSize( INT32 val);

	void			GetScrollRect( i3::pack::RECT* pRt);

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;
	
	virtual void	OnEnabled( bool bEnable) override;

	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual void	OnBindTemplate(void) override;

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;

	virtual void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;

	void SetAlpha(UINT8 alpha);
	void SetAlpha(REAL32 alpha); //0.f ~ 1.f 값
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	static	void	RegisterLuaFunction(LuaState* pState);		// 11.05.19.수빈 루아함수 몇개가 필요해 급히 만듭니다.

};

#endif //__I3UI_BUTTON_H__
