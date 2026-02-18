#if !defined( __I3UI_DOUBLE_TEXT_H__)
#define __I3UI_DOUBLE_TEXT_H__

/** \brief 2개의 Static Text를 설정하는 Control 입니다. */

#include "i3UIControl.h"

class i3UIStaticText;

class I3_EXPORT_FRAMEWORK i3UIDoubleText : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIDoubleText, i3UIControl);
private:
	i3UIStaticText *	m_pText1 = nullptr;
	i3UIStaticText *	m_pText2 = nullptr;

public:
	virtual ~i3UIDoubleText();

	virtual void	OnBindTemplate(void) override;

	const WCHAR16 *	GetText( INT32 idx);

	void			SetText( const WCHAR16 * pText1, const char * pText2);
	void			SetText( const char* pText1, const char * pText2);

	static void		RegisterLuaFunction( LuaState * pState);
};

#endif
