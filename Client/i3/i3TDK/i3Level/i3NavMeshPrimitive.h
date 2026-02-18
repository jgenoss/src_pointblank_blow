#pragma once

class I3_EXPORT_TDK i3NavMeshPrimitive : public i3GeometryAttr
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3NavMeshPrimitive, i3GeometryAttr );

public:
	bool	Init( i3Geometry* pParent );

public:
	virtual	void		OnBuildMesh()	= 0;
	virtual void		OnRender()		= 0;

public:
	virtual void		Apply( i3RenderContext* ctx ) override;
	virtual void		RenderElement();

protected:
	void		_BuildGeometry_Sphere( UINT32 hdiv = 10, UINT32 vdiv = 5 );
	void		_BuildGeometry_SelBox();

	void		_PrevRenderState();
	void		_ReSetRenderState();

protected:
	MATRIX		m_mat;
	
	COLORREAL		m_cr;
	COLORREAL		m_crDiffuse;

	I3G_BLEND			m_srcBlend;
	I3G_BLEND			m_destBlend;
	I3G_BLEND_OP		m_blendOp;
	I3G_RENDER_MODE		m_renderMode;

	bool			m_blendEnable;
	bool			m_zwrite;
	bool			m_ztest;

protected:
	i3RenderContext*		m_ctx = nullptr;
};