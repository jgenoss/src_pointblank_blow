#if !defined (__I3VIEWER2_LIGHTAXIS_G)
#define __I3VIEWER2_LIGHTAXIS_G


class i3Viewer2LightAxis : public i3Transform
{
	I3_CLASS_DEFINE(i3Viewer2LightAxis, i3Transform);

protected:

	i3Transform *	m_pAxis;
	i3AttrSet *		m_pAxisAttrSet;
	i3ColorAttr *	m_pAxis_ColorAttr;
	i3Node *		m_pLightSphere;

public:
	i3Viewer2LightAxis();
	virtual ~i3Viewer2LightAxis();

	void	CreateLightAxis();

	void	SetVisible(bool bVisible);
	void	Enable();
	void	Disable();
};

#endif