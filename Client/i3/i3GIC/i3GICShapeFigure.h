#if !defined( __I3_GIC_SHAPE_FIGURE_H__)
#define __I3_GIC_SHAPE_FIGURE_H__

#include "i3GICShape.h"
#include "i3GICDefine.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeFigure : public i3GICShape
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3GICShapeFigure, i3GICShape);
private:
	// persist member
	GIC_SHAPE_BORDER_STYLE	m_BorderStyle = GIC_SHAPE_BORDER_STYLE_SOLID;
	COLORREF	m_BorderColor;
	COLORREF	m_FillColor;
	UINT32		m_uBorderSize = 1;

protected:
	// Volatile member
	HBRUSH		m_hbrFill = nullptr;
	HPEN		m_hpenBorder = nullptr;

protected:
	/** \breif OnDraw에서 반드시 호출해서 Pen과 Brush를 선택해야한다.
		*/
	void		_SelectPenAndBrush( i3GICDiagramContext * pCtx);

	void		_CreateGDIFillObject( void);
	void		_DeleteGDIFillObject( void);

	void		_CreateGDIBorderObject( void);
	void		_DeleteGDIBorderObject( void);

public:
	GIC_SHAPE_BORDER_STYLE	getBorderStyle( void)					{ return m_BorderStyle; }
	void			setBorderStyle( GIC_SHAPE_BORDER_STYLE style)
	{
		m_BorderStyle = style;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDIBorderObject();
			_CreateGDIBorderObject();
		}
	}

	/** \brief 외곽선의 색을 반환합니다. 
		\return 외곽선의 색		*/
	COLORREF		getBorderColor(void)			{ return m_BorderColor; }

	/** \brief 외곽선의 색을 설정합니다. 
		\param color 외곽선의 색
		\return none		*/
	void			setBorderColor( COLORREF color)
	{
		m_BorderColor = color;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDIBorderObject();
			_CreateGDIBorderObject();
		}
	}

	void			setBorderColor( COLOR * pCol)
	{
		COLORREF c;

		c = RGB( pCol->r, pCol->g, pCol->b);

		setBorderColor( c);
	}

	UINT32			getBorderSize( void)					{ return m_uBorderSize; }
	void			setBorderSize( UINT32 uSize)
	{
		m_uBorderSize = uSize;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDIBorderObject();
			_CreateGDIBorderObject();
		}
	}

	/** \brief Shape의 색을 반환합니다. 
		\return 색	*/
	COLORREF 	getFillColor(void)				{ return m_FillColor; }

	void		setFillColor( COLORREF color)
	{
		m_FillColor = color;

		if( isBindDiagram())
		{
			_DeleteGDIFillObject();
			_CreateGDIFillObject();
		}
	}

	void		setFillColor( COLOR * pCol)
	{
		COLORREF c;

		c = RGB( pCol->r, pCol->g, pCol->b);

		setFillColor( c);
	}

public:
	i3GICShapeFigure();
	virtual ~i3GICShapeFigure();

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
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
