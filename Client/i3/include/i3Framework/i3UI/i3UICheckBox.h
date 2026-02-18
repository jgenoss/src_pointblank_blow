#ifndef __I3UI_CHECKBOX_H__
#define __I3UI_CHECKBOX_H__

#include "i3UICaptionControl.h"		


class I3_EXPORT_FRAMEWORK i3UICheckBox : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UICheckBox, i3UICaptionControl);

protected:
	bool			m_bRightText = true;
	bool			m_bFreeSize = false;
	bool			m_bForceEnableInput = false;

	virtual void	_UpdateShapeState(void) override;
	void			_UpdateTextPosition( void);


public:
	i3UICheckBox();

	// Disable 이미지 상에서도 인풋을 받아야될 구 UI가 존재해서 이런 쓰잘데기 없어보이는 멤버가 추가되었다. ( 1.1버전 호칭 44개 슬롯들..)
	void			SetForceEnableInput(bool bForceEnable) { m_bForceEnableInput = bForceEnable; }
	bool			IsForceEnableInput() const { return m_bForceEnableInput; }

	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	Init( void) override;

	void			SetFreeSize(bool bEnable);

public:
	//  의도가 분명한 인터페이스 2개를 만듭니다. (11.05.31.수빈.isSelected()나 LButtonDown()이외의 것이 필요함)
	void	 setCheck(bool bCheck = true, bool bNotifyEnable = false);			// 이벤트발생을 원치 않을 경우가 있을것입니다.
	bool isChecked() const { return isSelected(); }

	// PropertyGrid
#if defined( I3_DEBUG)
	virtual void		GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void		OnTDKPropertyChanged( const char* szName) override;
#endif

	static void		RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_CHECKBOX_H__