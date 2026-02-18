#if !defined( __I3_GIC_SHAPE_CIRCLE_BAR_H__)
#define __I3_GIC_SHAPE_CIRCLE_BAR_H__

#include "i3GICShapeCircle.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICShapeCircleBar : public i3GICShapeCircle
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeCircleBar, i3GICShapeCircle);
private:

	// persistant member
	REAL32			m_rBarAngle = 0.0f;

	UINT32			m_uBarWidth = 1;
	COLORREF		m_BarColor;

	// Volatile member
	HPEN			m_hpenBar = nullptr;
	i3GICControlPoint *	m_pBarCP[2] = { nullptr, nullptr };

protected:
	void			_CreateGDIBarObject( void);
	void			_DeleteGDIBarObject( void);

public:
	REAL32			getAngle( void)						{ return m_rBarAngle; }
	void			setAngle( REAL32 rRad)				{ m_rBarAngle = rRad; }

	UINT32			getBarWidth( void)					{ return m_uBarWidth; }
	void			setBarWidth( UINT32 width)
	{
		m_uBarWidth = width;
		if( isBindDiagram())
		{
			_DeleteGDIBarObject();
			_CreateGDIBarObject();
		}
	}

	COLORREF		getBarColor( void)					{ return m_BarColor; }
	void			setBarColor( COLORREF color)
	{
		m_BarColor = color;

		if( isBindDiagram())
		{
			_DeleteGDIBarObject();
			_CreateGDIBarObject();
		}
	}

public:
	i3GICShapeCircleBar();
	virtual ~i3GICShapeCircleBar();

	virtual void	OnAfterDraw( i3GICDiagramContext * pCtx);

	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);

	/** \brief Diagram을 Bind/Unbind할시에 호출된다.
		\note Diagram이 보여지면 GDI Object를 생성하고
			그렇지 않으면 삭제한다.
		\param[in] bBind Bind 유무
			*/
	virtual void	OnBindDiagram( bool bBind);

	/** \brief 인스턴스 정보를 pDest로 복사합니다. 
		\sa i3TreeNode::CopyTo		*/	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void	OnProperty( CMFCPropertyGridCtrl * pCtrl);
};

#endif	// I3_DEBUG

#endif
