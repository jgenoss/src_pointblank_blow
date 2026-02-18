#if !defined( __GIZMO_H_)
#define __GIZMO_H_

class CGizmo : public i3GameObj
{
	I3_CLASS_DEFINE( CGizmo);
protected:
	i3AttrSet	*	m_pAxisRoot;

	i3Transform *	m_pTransformRoot;

	i3Transform *	m_pAxis[3];

	VEC3D			m_vAxisMask;

public:
	CGizmo(void);
	virtual ~CGizmo(void);

	virtual BOOL	Create();

	bool	isCollided()	{	return (bool)(i3Vector::Length(&m_vAxisMask) > I3_EPS);}
public:

	i3Node * BuildRectangle( BOOL bTri = TRUE, BOOL bLine = TRUE, REAL32 fWidth = 1.f, REAL32 fHeight = 1.f, COLORREAL * pLineColor = NULL, COLORREAL * pDiffuse = NULL, COLORREAL * pAmbient = NULL );
	void	SetZFunc( i3Node * pNode, I3G_COMP_FUNC func );
	void	SetAlphaFunc( i3Node * pNode, I3G_COMP_FUNC func );
	void	SetBlendMode( i3Node * pNode, I3G_BLEND src, I3G_BLEND dest );

	void	ChangeColor( i3Node * pNode, COLORREAL * pColor );

	virtual bool	OnCollision(  VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx);
	void			ScreenToWorld( POINT pos, VEC3D * pStart, VEC3D * pEnd, MATRIX * pmatInvViewProj);
};

#endif