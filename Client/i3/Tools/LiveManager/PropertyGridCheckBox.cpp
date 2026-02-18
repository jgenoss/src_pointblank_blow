#include "StdAfx.h"
#include "PropertyGridCheckBox.h"


#define IDC_CHECKBOX_ID 1


CPropertyGridCheckBox::CPropertyGridCheckBox(CPropertyGridCtrlEx* pPropertyList, const CString& strName, _variant_t& varValue, const CString strParant, 
											 DWORD dwFontDialogFlags, LPCTSTR lpszDescr, DWORD_PTR dwData, COLORREF color) 
: CMFCPropertyGridProperty(strName, COleVariant(varValue), lpszDescr, dwData)
{
	m_bCheckState = false;
	m_pProperty = NULL;
	m_pwndPropList = pPropertyList;
	m_strParantName = L"[";
	if( strParant == "BaseItem" )
	{
		m_strParantName += "NewItem";
	}
	else
	{
		m_strParantName += strParant;
	}
	m_strParantName += L"] ";
	m_bStaticCheckState = false;
}

CPropertyGridCheckBox::~CPropertyGridCheckBox(void)
{

}

void CPropertyGridCheckBox::OnDestroyWindow()
{
	CMFCPropertyGridProperty::OnDestroyWindow();
}

BOOL CPropertyGridCheckBox::OnEdit(LPPOINT lptClick)
{
	CMFCPropertyGridProperty* pGroup = m_pwndPropList->FindItemByData(ID_PROPERTY_NEWINFO);

	if( !m_bCheckState )
	{
		if( pGroup )
		{
			m_pProperty = new CMFCPropertyGridProperty(m_strParantName + GetName(), GetValue(), _T(""));
			pGroup->AddSubItem(m_pProperty);
			m_pwndPropList->ExpandAll();
		}
		
		m_bCheckState = true;

		if( !lptClick )
		{
			m_bStaticCheckState = true;
		}
	}
	else
	{
		if( pGroup && !m_bStaticCheckState )
		{
			m_bCheckState = false;

			pGroup->RemoveSubItem(m_pProperty);
			m_pProperty = NULL;
			m_pwndPropList->ExpandAll();
		}
	}
	
	return TRUE;
}



BOOL CPropertyGridCheckBox::PushChar(UINT nChar)
{
	return FALSE;
}

 void CPropertyGridCheckBox::OnDrawValue(CDC* pDC, CRect rect)
 {
	CMFCPropertyGridProperty::OnDrawValue(pDC, rect);

	 if( m_bCheckState )
	 {
		 CRect rectColor = rect;


		 rectColor.left = rect.right - 10;
		 rectColor.top = rect.top + 3;
		 rectColor.bottom = rect.bottom - 3;
		 

		 CBrush br(RGB(0, 255, 0));
		 pDC->FillRect(rectColor, &br);
		 pDC->Draw3dRect(rectColor, 0, 0);
	 }
 }

 IMPLEMENT_DYNAMIC(CPropertyGridCtrlEx, CMFCPropertyGridCtrl)
 BEGIN_MESSAGE_MAP(CPropertyGridCtrlEx, CMFCPropertyGridCtrl)

	 ON_WM_VSCROLL()
END_MESSAGE_MAP()

 CPropertyGridCtrlEx::CPropertyGridCtrlEx() : CMFCPropertyGridCtrl()
 {
 }
 void CPropertyGridCtrlEx::AdjustLayout()
 {
	CMFCPropertyGridCtrl::AdjustLayout();
 }

 void CPropertyGridCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
 {
	 CMFCPropertyGridCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
	
 }

 CPropertyGridPropertyWeapon::CPropertyGridPropertyWeapon(const CString& strName, _variant_t& varValue, bool bPauseState, LPCTSTR lpszDescr, DWORD_PTR dwData) 
	 : CMFCPropertyGridProperty(strName, COleVariant(varValue), lpszDescr, dwData)
 {
	 m_bPauseEdit = bPauseState;
 }

 CPropertyGridPropertyWeapon::~CPropertyGridPropertyWeapon(void)
 {

 }

 BOOL CPropertyGridPropertyWeapon::OnEdit(LPPOINT lptClick)
 {
	 if( m_bPauseEdit ) return FALSE;

	 CMFCPropertyGridProperty::OnEdit(lptClick);

	 return FALSE;
 }

 BOOL CPropertyGridPropertyWeapon::PushChar(UINT nChar)
 {
	 if( m_bPauseEdit ) return FALSE;

	CMFCPropertyGridProperty::PushChar(nChar);
	
	return FALSE;
 }

 void CPropertyGridPropertyWeapon::OnDrawButton(CDC* pDC, CRect rect)
 {
	 CMFCPropertyGridProperty::OnDrawButton(pDC, rect);
 }