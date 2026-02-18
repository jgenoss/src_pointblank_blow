#ifndef __I3UI_COMBOBOX_H__
#define __I3UI_COMBOBOX_H__



#include "i3UIComboBase.h"

class i3UIEditBox;
class i3UIButton;	

// Combo는 EditBox, Button, ListBox 3개의 Child Control을 갖는다.
// ListBox는 Button을 누를 때마다 동적으로 생성/소멸되어야 한다. 
// 그래야 Control들의 Add순서와 관계없이 항상 맨 위에 보인다. 
class I3_EXPORT_FRAMEWORK i3UIComboBox : public i3UIComboBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIComboBox, i3UIComboBase);
public:
	i3UIComboBox();
	virtual ~i3UIComboBox();

	i3UIButton*		getButtonCtrl() const { return m_pButton; }
	i3UIEditBox*	getEditBox( void) const	{	return m_pEditBox;	}
	void			SetEditBoxReadOnly( bool bReadOnly);

protected:
	i3UIEditBox*	m_pEditBox;						// [initialize at constructor]
	i3UIButton*		m_pButton;						// [initialize at constructor]
	REAL32			m_fDefWidthButton = 22.0f;		// Button Width

private:
	virtual void	_UpdateComponentSize_DerivedCombo(REAL32 cx, REAL32 cy) override;
	virtual void	_RemoveListBox_DerivedCombo()  override {}
	virtual void	_SetFocusEditBoxIfExists_DerivedCombo() override;
	virtual void	_OnInitAfterLoad_DerivedCombo() override;
	virtual void	_OnEnabled_DerivedCombo(bool bEnable) override;
	virtual void	_OnBindTemplate_DerivedCombo() override;
	virtual bool	_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const;
	virtual UINT32	_OnLoadMore( i3ResourceFile * pResFile, UINT32 parentFileID)  override { return 0; }

#if defined( I3_DEBUG)	// Tool에서만 사용
	virtual void	_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List) override;
	virtual bool	_OnTDKPropertyChanged_DerivedCombo(const char* szName) override;
#endif

	virtual void	CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method) override;
};

#endif //__I3UI_COMBOBOX_H__