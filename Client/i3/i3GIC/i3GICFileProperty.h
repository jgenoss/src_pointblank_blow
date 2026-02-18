#if !defined( __I3_GIC_FILE_PROPERTY_H__)
#define __I3_GIC_FILE_PROPERTY_H__

#if defined( I3_DEBUG)

enum GIC_PROPERTY_UPDATE_PATH
{
	GIC_PROPERTY_UPDATE_PATH_TEXTURE = 0,
};

class i3GICShape;

class I3_EXPORT_GIC i3GICFileProperty : public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC( i3GICFileProperty);

private:
	i3GICShape *				m_pShape = nullptr;
	GIC_PROPERTY_UPDATE_PATH	m_UpdateType = GIC_PROPERTY_UPDATE_PATH_TEXTURE;

public:
	void			setShape( i3GICShape * pShape)					{ m_pShape = pShape; }
	void			setUpdateType( GIC_PROPERTY_UPDATE_PATH type)	{ m_UpdateType = type; }

public:
	i3GICFileProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = nullptr,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = nullptr, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0)
		: CMFCPropertyGridFileProperty(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData) {}
	virtual ~i3GICFileProperty() {}

	void			ChangeValue( void);

	virtual BOOL	OnUpdateValue( void);
	virtual void	OnClickButton( CPoint point);
	virtual BOOL	OnEndEdit( void);
};

#endif	// I3_DEBUG

#endif
