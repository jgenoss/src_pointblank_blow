#pragma once

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_ColorPicker : public i3TDKPropertyGridProperty
{
	friend class CMFCPropertyGridCtrl;

	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_ColorPicker)

private:
	COLOR*			m_pData;			// Color [initialize at constructor]
	HBRUSH			m_hBrush = nullptr;
	
protected:
	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);

	void	UpdateColor(void);

public:
	i3TDKPropertyGridProperty_ColorPicker( PROPERTYINFO* pInfo);
	virtual ~i3TDKPropertyGridProperty_ColorPicker(void);

	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual void OnClickButton(CPoint point);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL OnUpdateValue(void);

	virtual CString FormatProperty();

	virtual void	UpdateFromProperty( void* pData = nullptr);
};

#endif
