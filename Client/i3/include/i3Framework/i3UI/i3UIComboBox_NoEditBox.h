#pragma once


//
// 기존 콤보박스와는 달리 에디트 박스를 제거하고, 셀렉션된 내용은 기존 가로로 연장된 Button 왼쪽에 표시해두도록 처리한다.
// UI작업자는 화살표버튼 대신에 가로로 연장된 버튼을 사용하도록 한다..
// ListBox는 Button을 누를 때마다 동적으로 생성/소멸되어야 한다. 
//

#include "i3UIComboBase.h"

class i3UIButtonComposed3;


class I3_EXPORT_FRAMEWORK i3UIComboBox_NoEditBox : public i3UIComboBase
{
	I3_EXPORT_CLASS_DEFINE( i3UIComboBox_NoEditBox, i3UIComboBase);

public:
	i3UIComboBox_NoEditBox();
	virtual ~i3UIComboBox_NoEditBox();

	i3UIButtonComposed3* getButtonCtrl() const { return m_pButton; }

protected:

	i3UIButtonComposed3*	m_pButton;		// [initialize at constructor]

private:
	virtual void	_UpdateComponentSize_DerivedCombo(REAL32 cx, REAL32 cy) override;
	virtual void	_RemoveListBox_DerivedCombo() override;
	virtual void	_SetFocusEditBoxIfExists_DerivedCombo()  override {}
	virtual void	_OnInitAfterLoad_DerivedCombo()  override {}
	virtual void	_OnEnabled_DerivedCombo(bool bEnable) override;
	virtual void	_OnBindTemplate_DerivedCombo() override;
	virtual bool	_IsListBoxActionNeededWhenClick_DerivedCombo(i3UIControl* pClickedCtrl) const;
	virtual UINT32	_OnLoadMore( i3ResourceFile * pResFile, UINT32 parentFileID) override;

#if defined( I3_DEBUG)	// Tool에서만 사용
	virtual void	_GetTDKGridProperties_DerivedCombo(i3::vector<PROPERTYINFO*>& List) override;
	virtual bool	_OnTDKPropertyChanged_DerivedCombo(const char* szName) override;
	virtual void	_OnTDKPropertyChanged_DerivedColor(COLOR pColor) override;
#endif

	// 기존 기본클래스 가상함수들 처리..(버튼내부 텍스트색상에 대해서 별도 저장한다.)
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

};
