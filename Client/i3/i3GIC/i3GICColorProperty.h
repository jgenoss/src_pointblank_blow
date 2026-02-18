#if !defined( __I3GIC_COLOR_PROPERTY_H__)
#define __I3GIC_COLOR_PROPERTY_H__

#if defined( I3_DEBUG)

class i3GICShape;

enum GIC_PROPERTY_UPDATE_COLOR
{
	GIC_PROPERTY_UPDATE_COLOR_BK = 0,
	GIC_PROPERTY_UPDATE_COLOR_BORDER,
	GIC_PROPERTY_UPDATE_COLOR_LINE,
	GIC_PROPERTY_UDPATE_COLOR_FONT,
	GIC_PROPERTY_UDPATE_COLOR_BAR,
};

class I3_EXPORT_GIC i3GICColorProperty : public CMFCPropertyGridColorProperty
{
	DECLARE_DYNAMIC( i3GICColorProperty);

private:
	i3GICShape *	m_pShape = nullptr;
	GIC_PROPERTY_UPDATE_COLOR	m_UpdateType = GIC_PROPERTY_UPDATE_COLOR_BK;

protected:
	void			_UpdateColorToShape( void);

public:
	void			setShape( i3GICShape * pShape)						{ m_pShape = pShape; }
	void			setUpdateType( GIC_PROPERTY_UPDATE_COLOR type)		{ m_UpdateType = type; }

public:
	i3GICColorProperty(const CString& strName, const COLORREF& color,
		CPalette* pPalette = nullptr, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0)
		: CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData) {}
	virtual ~i3GICColorProperty() {}

	virtual BOOL	OnUpdateValue( void);

};

#endif	// I3_DEBUG

#endif
