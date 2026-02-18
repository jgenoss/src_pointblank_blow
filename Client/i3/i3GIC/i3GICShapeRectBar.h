#if !defined( __I3_GIC_SHAPE_RECT_BAR_H__)
#define __I3_GIC_SHAPE_RECT_BAR_H__

#include "i3GICShapeRect.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeRectBar : public i3GICShapeRect
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeRectBar, i3GICShapeRect);

private:
	// Persistant members
	REAL32		m_rValue = 0.0f;					///< Percent 0 ~ 100.f
	COLORREF	m_BarColor = RGB(0, 0, 0);
	UINT32		m_uBarWidth = 1;

	// Volatile member
	HPEN		m_hpenBar = nullptr;

protected:
	void		_CreateGDIBarObject( void);
	void		_DeleteGDIBarObject( void);

public:
	REAL32		getBarValue( void)					{ return m_rValue; }
	void		setBarValue( REAL32 rVal)			{ m_rValue = rVal; }

	COLORREF	getBarColor( void)				{ return m_BarColor; }
	void		setBarColor( COLORREF color)
	{
		m_BarColor = color;

		if( isBindDiagram())
		{
			_DeleteGDIBarObject();
			_CreateGDIBarObject();
		}
	}

	UINT32		getBarWidth( void)				{ return m_uBarWidth; }
	void		setBarWidth( UINT32 width)
	{
		m_uBarWidth = width;
		
		if( isBindDiagram())
		{
			_DeleteGDIBarObject();
			_CreateGDIBarObject();
		}
	}

public:
	i3GICShapeRectBar();
	virtual ~i3GICShapeRectBar();

	virtual	void	OnAfterDraw( i3GICDiagramContext * pCtx);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//								for Property
public:
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif // I3_DEBUG

#endif
