#if !defined( __I3_GIC_SHAPE_LINE_H)
#define __I3_GIC_SHAPE_LINE_H

#include "i3GICControlPoint.h"
#include "i3GICDefine.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeLine : public i3GICShape
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeLine, i3GICShape);
protected:
	// Persist members
	I3GIC_POINT		m_StartPoint;
	I3GIC_POINT		m_EndPoint;

	GIC_LINE_STYLE	m_LineStyle = GIC_LINE_STYLE_SOLID;
	UINT32			m_LineWidth = 1;				///< Line 두께

	COLORREF		m_LineColor;
	COLORREF		m_OldLineColor = RGB(0, 0, 0);

	// Volatile Members
	HPEN			m_hpenLine = nullptr;

protected:
	bool			_HitTest( REAL32 x, REAL32 y);
	void			_CreateGDILineObject( void);
	void			_DeleteGDILineObject( void);

	void			_DragPoint( i3GICDiagramContext * pCtx, UINT32 sem, REAL32 dcx, REAL32 dcy);

public:
	I3GIC_POINT *	getStartPoint( void)			{ return &m_StartPoint; }

	void			setStartPoint( VEC2D * pVec)
	{
		i3Vector::Copy( &m_StartPoint.m_Point, pVec);
	}

	I3GIC_POINT *	getEndPoint( void)				{ return &m_EndPoint; }

	void			setEndPoint( VEC2D * pVec)
	{
		i3Vector::Copy( &m_EndPoint.m_Point, pVec);
	}

	GIC_LINE_STYLE	getLineStyle( void)							{ return m_LineStyle; }

	void			setLineStyle( GIC_LINE_STYLE style)
	{
		m_LineStyle = style;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDILineObject();
			_CreateGDILineObject();
		}
	}

	UINT32			getLineWidth( void)				{ return m_LineWidth; }

	void			setLineWidth( UINT32 width)
	{
		m_LineWidth = width;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDILineObject();
			_CreateGDILineObject();
		}
	}

	COLORREF		getLineColor( void)				{ return m_LineColor; }

	void			setLineColor( COLORREF color)
	{
		m_OldLineColor = m_LineColor;
		m_LineColor = color;

		if( isBindDiagram() && (isStyle( I3GIC_STYLE_GDIPLUS) == false) )
		{
			_DeleteGDILineObject();
			_CreateGDILineObject();
		}
	}

	void			setLineColor( COLOR * pCol)
	{
		COLORREF c;

		c = RGB( pCol->r, pCol->g, pCol->b);

		if( c == m_LineColor)	return;

		setLineColor( c);
	}

	//
public:
	i3GICShapeLine(void);
	virtual ~i3GICShapeLine(void);

	virtual void	OnCreate( void);

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List);

	/** \brief 다른 Linker Shape이 이 함수를 호출하여 Update시켜준다.
		\note 영향을 받는 경우 여기에 정의한다. */
	virtual void	OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pEffector, i3GICLinkPoint * pLinker);

	virtual bool	OnDragCP( i3GICDiagramContext * pCtx);

	virtual bool	OnPreDragMove( REAL32 x, REAL32 y);

	virtual	bool	OnBeginDrag(void);
	virtual	bool	OnCancelDrag(void);

	virtual bool	OnSelect( bool bFlag);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//								for Property
public:
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
