#if !defined( __I3_GIC_PROPERTY_H__)
#define __I3_GIC_PROPERTY_H__

#if defined( I3_DEBUG)

class i3GICShape;

enum GIC_PROPERTY_UPDATE
{
	GIC_PROPERTY_UPDATE_NONE = 0,
	GIC_PROPERTY_UPDATE_NAME,
	GIC_PROPERTY_UPDATE_POSITION_2D,
	GIC_PROPERTY_UPDATE_POSITION_3D,
	GIC_PROPERTY_UPDATE_SIZE_2D,
	GIC_PROPERTY_UPDATE_SIZE_3D,
	GIC_PROPERTY_UPDATE_BORDERSIZE,
	GIC_PROPERTY_UPDATE_LINEWIDTH,
	GIC_PROPERTY_UPDATE_TEXT,
	GIC_PROPERTY_UPDATE_FILL,
	GIC_PROPERTY_UPDATE_ROUNDVALUE,
	GIC_PROPERTY_UPDATE_BORDERSTYLE,
	GIC_PROPERTY_UPDATE_LINESTYLE,
	GIC_PROPERTY_UPDATE_FONT,
	GIC_PROPERTY_UPDATE_FONTSIZE,
	GIC_PROPERTY_UPDATE_BARVALUE,
	GIC_PROPERTY_UPDATE_BARWIDTH,
	GIC_PROPERTY_UPDATE_SHAPESTYLE,
};

class I3_EXPORT_GIC i3GICProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC( i3GICProperty);

private:
	i3GICShape *	m_pShape = nullptr;
	GIC_PROPERTY_UPDATE	m_UpdateType = GIC_PROPERTY_UPDATE_NONE;

protected:
	void			_UpdateName( void);
	void			_Update2DPosition( void);
	void			_Update3DPosition( void);
	void			_Update2DSize( void);
	void			_Update3DSize( void);
	void			_UpdateBorderSize( void);			///< for i3GICShapeFigure
	void			_UpdateLineWidth( void);			///< for i3GICShapeLine
	void			_UpdateText( void);					///< for i3GICShapeTextRect
	void			_UpdateFill( void);					///< for i3GICShapeFigure
	void			_UpdateTexturePath( void);			///< for i3GICShapeRenderTex
	void			_UpdateRoundValue( void);			///< for i3GICShapeRect
	void			_UpdateBorderStyle( void);			///< for i3GICShapeFigure
	void			_UpdateLineStyle( void);			///< for i3GICShapeLine
	void			_UpdateFontStyle( void);			///< for i3GICShapeTextRect
	void			_UpdateFontSize( void);				///< for i3GICShapeTextRect
	void			_UpdateBarValue( void);				///< for i3GICShapeCircleBar, i3GICShapeRectBar
	void			_UpdateBarWidth( void);				///<
	void			_UpdateShapeStyle( void);

public:
	void			setUpdateType( GIC_PROPERTY_UPDATE type)		{ m_UpdateType = type; }
	void			setShape( i3GICShape * pShape)					{ m_pShape = pShape; }

public:
	i3GICProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE)
		: CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) {}
	i3GICProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = nullptr, LPCTSTR lpszEditTemplate = nullptr, LPCTSTR lpszValidChars = nullptr)
		: CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars) {}
	virtual ~i3GICProperty() {}

	void			UpdateChangeValue( void);

	virtual BOOL	OnUpdateValue( void);
};

#endif	// I3_DEBUG

#endif
