#if !defined( __I3_GIC_FONT_PROPERTY_H__)
#define __I3_GIC_FONT_PROPERTY_H__

#if defined( I3_DEBUG)

class i3GICShape;

class I3_EXPORT_GIC i3GICFontProperty : public CMFCPropertyGridFontProperty
{
	DECLARE_DYNAMIC( i3GICFontProperty);

private:
	i3GICShape *	m_pShape;

public:
	i3GICFontProperty( const CString& strName, LOGFONT& lf, DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS,
				LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, COLORREF color = (COLORREF)-1 );

	virtual ~i3GICFontProperty();

	void			UpdateChangeValue( void);

	virtual BOOL	OnUpdateValue( void);

};

#endif	// I3_DEBUG

#endif
