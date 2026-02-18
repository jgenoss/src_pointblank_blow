#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_EMPTY_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_EMPTY_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"


// ImageGroup Property 아래에 추가되는 Empty Property. 동작을 막을 함수들만 재정의 한다. 
class I3_EXPORT_TDK i3TDKPropertyGridProperty_Empty : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Empty)

	i3TDKPropertyGridProperty_Empty( void);
	
	virtual void UpdateFromProperty( void* pData = nullptr);

public:
	//virtual void OnDrawName(CDC* pDC, CRect rect);
	//virtual void OnDrawValue(CDC* pDC, CRect rect);
	//virtual void OnDrawExpandBox(CDC* pDC, CRect rectExpand);
	//virtual void OnDrawButton(CDC* pDC, CRect rectButton);
	//virtual void OnDrawDescription(CDC* pDC, CRect rect);

	//virtual CString FormatProperty();

	virtual BOOL OnUpdateValue() {	return TRUE;}
	virtual BOOL OnEdit(LPPOINT lptClick) {	return TRUE;}
	//virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	//virtual CSpinButtonCtrl* CreateSpinControl(CRect rectSpin);

	virtual BOOL OnEndEdit() {	return TRUE;}

	virtual void OnClickButton(CPoint point) {}
	virtual BOOL OnClickValue(UINT uiMsg, CPoint point) {	return TRUE;}
	virtual BOOL OnDblClk(CPoint point) {	return TRUE;}

	virtual void OnSelectCombo() {}
	virtual void OnCloseCombo() {}

	virtual BOOL OnSetCursor() const {	return TRUE;}
	//virtual BOOL PushChar(UINT nChar) {}

	//virtual CString GetNameTooltip();
	//virtual CString GetValueTooltip();

	virtual void OnClickName(CPoint /*point*/) {}
	virtual void OnRClickName(CPoint /*point*/) {}
	virtual void OnRClickValue(CPoint /*point*/, BOOL /*bSelChanged*/) {}

	virtual void OnPosSizeChanged(CRect /*rectOld*/) {}

	virtual void OnSetSelection(CMFCPropertyGridProperty* /*pOldSel*/) {}
	virtual void OnKillSelection(CMFCPropertyGridProperty* /*pNewSel*/) {}

	//virtual void AdjustButtonRect();
	//virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);

protected:
	//virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	//virtual CComboBox* CreateCombo(CWnd* pWndParent, CRect rect);
	//virtual void OnDestroyWindow();

	//virtual BOOL OnKillFocus(CWnd* /*pNewWnd*/) { return TRUE; }
	//virtual BOOL OnEditKillFocus() { return TRUE; }

	//virtual BOOL HasButton() const;

	//virtual BOOL IsProcessFirstClick() const { return TRUE; }
	//virtual BOOL HasValueField() const { return TRUE; }

	//virtual BOOL TextToVar(const CString& strText);
	//virtual BOOL IsValueChanged() const;

	//virtual BOOL OnActivateByTab();
	//virtual BOOL OnRotateListValue(BOOL bForward);

};

#endif

#endif