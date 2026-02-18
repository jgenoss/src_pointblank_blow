#if !defined( __I3_MATERIAL_ATTR_H)
#define __I3_MATERIAL_ATTR_H

#include "i3RenderAttr.h"
#include "i3Math.h"

class I3_EXPORT_SCENE i3MaterialAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3MaterialAttr, i3RenderAttr);

protected:
	UINT32			m_ValidFlag = I3G_MATERIAL_DIFFUSE | I3G_MATERIAL_SPECULAR | I3G_MATERIAL_EMISSIVE | I3G_MATERIAL_SHININESS;
	COLORREAL		m_Diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	COLORREAL		m_Specular = { 0.4f, 0.4f, 0.4f, 1.0f };
	COLORREAL		m_Emissive = { 1.0f, 1.0f, 1.0f, 1.0f };
	REAL32			m_Shininess = 128.0f;

public:
	i3MaterialAttr(void);

	void		SetDiffuse( COLORREAL * pColor)		{ i3Color::Set( &m_Diffuse, pColor); }
	void		SetSpecular( COLORREAL * pColor)	{ i3Color::Set( &m_Specular, pColor); }
	void		SetEmissive( COLORREAL * pColor)	{ i3Color::Set( &m_Emissive, pColor); }
	void		SetShininess( REAL32 val)			{ m_Shininess = val; }

	void		SetDiffuse( REAL32 r, REAL32 g, REAL32 b, REAL32 a )	{ i3Color::Set( &m_Diffuse, r, g, b, a ); }
	void		SetSpecular( REAL32 r, REAL32 g, REAL32 b, REAL32 a )	{ i3Color::Set( &m_Specular, r, g, b, a ); }
	void		SetEmissive( REAL32 r, REAL32 g, REAL32 b, REAL32 a )	{ i3Color::Set( &m_Emissive, r, g, b, a ); }

	COLORREAL *	GetDiffuse(void)					{ return &m_Diffuse; }
	COLORREAL *	GetSpecular(void)					{ return &m_Specular; }
	COLORREAL * GetEmissive(void)					{ return &m_Emissive; }
	REAL32		GetShininess(void)					{ return m_Shininess; }

	bool		isValidDiffuse(void)				{ return (m_ValidFlag & I3G_MATERIAL_DIFFUSE) != 0; }
	void		setValidDiffuse( bool bFlag)
	{
		if( bFlag)		m_ValidFlag |= I3G_MATERIAL_DIFFUSE;
		else			m_ValidFlag &= ~I3G_MATERIAL_DIFFUSE;
	}

	bool		isValidSpecular(void)				{ return (m_ValidFlag & I3G_MATERIAL_SPECULAR) != 0; }
	void		setValidSpecular( bool bFlag)
	{
		if( bFlag)		m_ValidFlag |= I3G_MATERIAL_SPECULAR;
		else			m_ValidFlag &= ~I3G_MATERIAL_SPECULAR;
	}

	bool		isValidEmissive(void)				{ return (m_ValidFlag & I3G_MATERIAL_EMISSIVE) != 0; }
	void		setValidEmissive( bool bFlag)
	{
		if( bFlag)		m_ValidFlag |= I3G_MATERIAL_EMISSIVE;
		else			m_ValidFlag &= ~I3G_MATERIAL_EMISSIVE;
	}

	bool		isValidShininess(void)				{ return (m_ValidFlag & I3G_MATERIAL_SHININESS) != 0; }
	void		setValidShininess( bool bFlag)
	{
		if( bFlag)		m_ValidFlag |= I3G_MATERIAL_SHININESS;
		else			m_ValidFlag &= ~I3G_MATERIAL_SHININESS;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
