#ifndef __I3UI_BUTTONCOMBOBOX_H__
#define __I3UI_BUTTONCOMBOBOX_H__

#include "i3UIComboBase.h"

class i3UIButtonComposed3;
class i3UIButton;	

class I3_EXPORT_FRAMEWORK i3UIButtonComboBox : public i3UIComboBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIButtonComboBox, i3UIComboBase);

public:
	i3UIButtonComboBox();
	virtual ~i3UIButtonComboBox();

	i3UIButton*				getButtonCtrl() const { return m_pButton; }
	i3UIButtonComposed3*	getEditBox( void) const	{	return m_pEditBox;	}

protected:
	i3UIButtonComposed3*	m_pEditBox;		// [initialize at constructor]
	i3UIButton*				m_pButton;		// [initialize at constructor]
	REAL32					m_fDefWidthButton = 22.0f;		// Button Width
	
private:

	virtual void	_UpdateComponentSize_DerivedCombo(REAL32 cx, REAL32 cy) override;
	virtual void	_RemoveListBox_DerivedCombo() override {}
	virtual void	_SetFocusEditBoxIfExists_DerivedCombo() override {}
	virtual void	_OnInitAfterLoad_DerivedCombo() override {}
	virtual void	_OnEnabled_DerivedCombo(bool bEnable) override;
	virtual void	_OnBindTemplate_DerivedCombo() override;
	virtual bool	_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const;
	virtual UINT32	_OnLoadMore( i3ResourceFile * pResFile, UINT32 parentFileID) override { return 0; }

#if defined( I3_DEBUG)	// Tool에서만 사용
	virtual void	_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List) override;
	virtual bool	_OnTDKPropertyChanged_DerivedCombo(const char* szName) override;
	virtual void	_OnTDKPropertyChanged_DerivedColor(COLOR pColor) override;
#endif

	virtual void	CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method) override;

};

#endif //__I3UI_COMBOBOX_H__