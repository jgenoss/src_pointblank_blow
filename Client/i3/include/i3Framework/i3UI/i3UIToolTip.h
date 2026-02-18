#ifndef __I3UI_TOOLTIP_H__
#define __I3UI_TOOLTIP_H__

#include "i3UICaptionControl.h"

class I3_EXPORT_FRAMEWORK i3UIToolTip : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIToolTip, i3UICaptionControl);

protected:
	INT32			m_nPosOffsetX = 0;
	INT32			m_nPosOffsetY = 0;
	INT32			m_nMaxWidth = 300;

	virtual void			_UpdateShapeState( void) override;

	void			setPosOffsetX( INT32 x)		{	m_nPosOffsetX = x;	}
	void			setPosOffsetY( INT32 y)		{	m_nPosOffsetY = y;	}
	void			setMaxWidth( INT32 w)		{	m_nMaxWidth = w;	}
public:
	i3UIToolTip();

	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual void	EnableCtrl( bool bTrue, bool bRecursive = false) override;

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	void			ChangePos( POINT cursorPos);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;
	
#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
#endif
	
	virtual void	SetText( const wchar_t* pText) override;
	virtual void	SetText( const i3::wliteral_range& wTextRng) override;
	virtual void	SetText( const i3::rc_wstring& strText) override;
	virtual void	SetText( const i3::wstring& strText) override;
	virtual void	SetText( const i3::stack_wstring& wstrText) override;

private:
	template<class StringType>									// cpp에 두고 멤버특화시킬것..
	void			SetTextImpl( const StringType& str);
};

#endif //__I3UI_TOOLTIP_H__
