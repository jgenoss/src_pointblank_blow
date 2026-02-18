#if !defined( __I3_Level_AXIS_H)
#define __I3_Level_AXIS_H

#include "i3LevelElement3D.h"
#include "i3LevelControl.h"

class i3LevelViewport;
class i3LevelFramework;

struct I3_LEVEL_EDIT_CONTEXT
{
	MATRIX					m_DragStartMatrix;
	POINT					m_DragStartPt = { 0, 0 };
	VEC3D					m_DragStartPos;
	POINT					m_OldDragPt = { 0, 0 };
	VEC3D					m_BasePos;
	bool					m_bDragging = false;
	bool					m_bOpenPopup = true;
	VEC3D					m_AxisMask = { 1.0f, 0.0f, 1.0f };

	INT32					m_idxGrip = 0;

	i3LevelViewport	*		m_pViewport = nullptr;
	i3LevelFramework *		m_pFramework = nullptr;

};

class I3_EXPORT_TDK i3LevelAxis : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelAxis, i3LevelControl);
protected:
	i3Transform *	m_pAxis[3] = { nullptr, nullptr, nullptr };
	i3AttrSet *		m_pAxisAttrSet[3] = { nullptr, nullptr, nullptr };
	i3ColorAttr *	m_pAxis_ColorAttr[3] = { nullptr, nullptr, nullptr };

	VEC3D			m_BoundMin;
	VEC3D			m_BoundMax;

	i3LevelElement3D *	m_pTargetObj = nullptr;

protected:
	bool			CreateAxis( INT32 AxisDiv = 20, REAL32 = 1.0f, REAL32 rad = 0.05f);

public:
	virtual ~i3LevelAxis(void);

	virtual BOOL	Create(void);

	const VEC3D &	getBoundMin(void)	const				{ return m_BoundMin; }
	const VEC3D &	getBoundMax(void)	const				{ return m_BoundMax; }
	void			setBound( const VEC3D & vmin, const VEC3D & vmax)
	{
		m_BoundMin = vmin;
		m_BoundMax = vmax;
	}

	i3LevelElement3D *	getTargetObject(void)				{ return m_pTargetObj; }
	void				setTargetObject( i3LevelElement3D * pObj)		{ I3_REF_CHANGE( m_pTargetObj, pObj); }

	i3Node * BuildRectangle( BOOL bTri = TRUE, BOOL bLine = TRUE, REAL32 fWidth = 1.f, REAL32 fHeight = 1.f, COLORREAL * pLineColor = nullptr, COLORREAL * pDiffuse = nullptr, COLORREAL * pAmbient = nullptr );
	void	SetZFunc( i3Node * pNode, I3G_COMP_FUNC func );
	void	SetAlphaFunc( i3Node * pNode, I3G_COMP_FUNC func );
	void	SetBlendMode( i3Node * pNode, I3G_BLEND src, I3G_BLEND dest );

	void	ChangeColor( i3Node * pNode, COLORREAL * pColor );

	virtual bool	OnCollision(  i3LevelFramework * pFramework, I3_LEVEL_EDIT_CONTEXT * pCtx, VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx);
	virtual void	OnUpdate( REAL32 tm)		{};
	virtual void	OnUpdateTransform(void)		{};
};

#endif