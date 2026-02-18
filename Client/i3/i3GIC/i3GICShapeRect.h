#if !defined( __I3_GIC_SHAPE_RECT_H)
#define __I3_GIC_SHAPE_RECT_H

#include "i3GICShapeFigure.h"

#if defined( I3_DEBUG)

class i3GICLinkPoint;

class I3_EXPORT_GIC i3GICShapeRect : public i3GICShapeFigure
{
	I3_EXPORT_CLASS_DEFINE( i3GICShapeRect, i3GICShapeFigure);
protected:
	REAL32		m_rCornerRadius = 0.0f;				///< 꼭지점의 라운드 반지름

	// Volcatile Members
	i3GICControlPoint *		m_pCP[I3GIC_CORNER_MAX];
	i3GICLinkPoint *		m_pLP[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };

public:
	void		setCornerRadius( REAL32 rVal)				{ m_rCornerRadius = rVal; }

public:
	i3GICShapeRect(void);
	virtual ~i3GICShapeRect(void) {}

	virtual void	OnCreate( void);

	/** \brief Size를 조절한다. */
	virtual	bool	OnPreSize( REAL32 cx, REAL32 cy);

	virtual bool	OnAllLinkerSnapToLink( i3GICDiagramContext * pCtx, REAL32 * pX, REAL32 * pY);

	/** \brief 이동 후의 Link Point의 이동량을 계산한다. */
	virtual	void	OnAfterDragMove( REAL32 x, REAL32 y);

	virtual void	OnAfterDraw( i3GICDiagramContext * pCtx) {}

	virtual	void	OnDraw( i3GICDiagramContext * pCtx);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List);

	/** \brief 다른 Linker Shape이 이 함수를 호출하여 Update시켜준다.
		\note 영향을 받는 경우 여기에 정의한다. */
	virtual void	OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pEffector, i3GICLinkPoint * pLinker);

	/** \brief 인스턴스 정보를 pDest로 복사합니다. 
		\sa i3TreeNode::CopyTo		*/	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

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
