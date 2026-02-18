#ifndef __I3UI_RADIO_H__
#define __I3UI_RADIO_H__

#include "i3UICaptionControl.h"		



class I3_EXPORT_FRAMEWORK i3UIRadio : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIRadio, i3UICaptionControl);
public:
	//
	//  의도가 분명한 인터페이스 2개를 만듭니다. (11.05.31.수빈.isSelected()나 LButtonDown()이외의 것이 필요함)
	//
	void			setCheck(bool bCheck = true, bool bNotifyEnable = false);			// 이벤트발생을 원치 않을 경우가 있을것입니다.
	bool			isChecked() const { return isSelected(); }

protected:
	INT32			m_nGroupIdx = 0;
	bool			m_bRightText = true;

	virtual void	_UpdateShapeState(void) override;
	void			_UpdateTextPosition( void);

	void			setGroupID( INT32 nIdx)		{	m_nGroupIdx = nIdx;	}

	
public:
	i3UIRadio();

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	Init( void) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	INT32			getGroupID(void)	{	return m_nGroupIdx;	}
	static void		RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_RADIO_H__