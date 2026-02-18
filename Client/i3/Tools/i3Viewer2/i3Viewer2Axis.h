#if !defined(__I3VIEWER2_AXIS_H)
#define __I3VIEWER2_AXIS_H

class i3Viewer2Axis : public i3Transform
{
	I3_CLASS_DEFINE(i3Viewer2Axis, i3Transform);

protected:
	i3Transform *	m_pAxis[3];
	i3AttrSet *		m_pAxisAttrSet[3];
	i3ColorAttr *	m_pAxis_ColorAttr[3];

	VEC3D			m_BoundMin;
	VEC3D			m_BoundMax;

	i3Transform *	m_pTargetTransform; 

public :
	i3Viewer2Axis();
	virtual ~i3Viewer2Axis();

	void			CreateAxis();

	i3Transform *	GetTargetTransform()							{ return m_pTargetTransform; }
	void			SetTargetTransform(i3Transform * pTrans)		{ I3_REF_CHANGE(m_pTargetTransform, pTrans); }

	void			SetScale(REAL32 fScale);

	void			Enable();
	void			Disable();
};

#endif