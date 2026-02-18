#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_BUTTON_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_BUTTON_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

// 버튼을 누르면 연결된 함수가 호출된다.
class I3_EXPORT_TDK i3TDKPropertyGridProperty_Button : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Button)

private:

#ifdef _UNICODE
	i3::wstring				m_strBtnName;			// 버튼에 표시될 텍스트
#else
	i3::string				m_strBtnName;			// 버튼에 표시될 텍스트
#endif

	PROC_BUTTONPROPERTY		m_pProc;				// 버튼을 눌렀을 때 실행될 함수	[initialize at constructor]
	
protected:
	virtual BOOL HasButton() const { return TRUE; }
	virtual void AdjustButtonRect();
	virtual void OnClickButton(CPoint point);
	virtual void OnDrawButton(CDC* pDC, CRect rectButton);

public:
	i3TDKPropertyGridProperty_Button( PROPERTYINFO* pInfo);

	virtual void	UpdateFromProperty( void* pData = nullptr);
};

#endif

#endif