#if !defined( __I3_GIC_SHAPE_TRIANGLE_H__)
#define __I3_GIC_SHAPE_TRIANGLE_H__

#include "i3GICShapeFigure.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeTriangle : public i3GICShapeFigure
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeTriangle, i3GICShapeFigure);

private:
	I3GIC_POINT		m_Point[3];


public:
	i3GICShapeTriangle();
	virtual ~i3GICShapeTriangle();

	virtual void	OnCreate( void);

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List);

	virtual	bool	OnBeginDrag(void);
	virtual	bool	OnCancelDrag(void);
	
	virtual bool	OnDragCP( i3GICDiagramContext * pCtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//								for Property
public:
	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif

