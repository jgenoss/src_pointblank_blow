#ifndef		__I3_TDK_PROPERTY_GRID_PROPERTY_H__
#define		__I3_TDK_PROPERTY_GRID_PROPERTY_H__

#if defined( I3_WINDOWS)

class i3TDKPropertyGridCtrl;

//Abstract Class
class I3_EXPORT_TDK i3TDKPropertyGridProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty)

public:
	
public:
	// constructor 
	i3TDKPropertyGridProperty( const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE )
		:	CMFCPropertyGridProperty( strGroupName, dwData, bIsValueList)
	{
	}

	// constructor 
	i3TDKPropertyGridProperty( const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0,
							LPCTSTR lpszEditMask = nullptr, LPCTSTR lpszEditTemplate = nullptr, LPCTSTR lpszValidChars = nullptr )
		:	CMFCPropertyGridProperty( strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
	{
	}
	
	// Property의 값이 변경되었을 때 그 값과 연결되어 있는 실제 변수의 값을 Update 해주는 함수
	virtual void UpdateFromProperty( void* pData = nullptr) = 0;		// From property to variable	// pData != nullptr 이면 pData의 값을 update
	virtual void UpdateToProperty( void)	{}						// From variable to property
	
	// PropertyCtrl내에 이미지를 그려주어야 하는 Property의 경우 Override
	virtual void DrawImage( void) {}

		
};

#endif

#endif