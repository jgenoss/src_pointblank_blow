#if !defined( __CONFIG_H)
#define __CONFIG_H

class CConfig
{
protected:
	COLORREAL		m_LightDiffuse;
	COLORREAL		m_LightSpecular;
	INT32			m_ShadowMapSize;
	COLORREAL		m_Ambient;
	COLORREAL		m_BackColor;

protected:
	void			ParseColor( const char * pszStr, COLORREAL * pColor);

public:
	CConfig(void);
	virtual ~CConfig(void);

	COLORREAL *	getAmbient(void)					{ return &m_Ambient; }
	void		setAmbient( COLORREAL * pCol)		{ i3Color::Set( &m_Ambient, pCol); }

	COLORREAL *	getBackColor(void)					{ return &m_BackColor; }
	void		setBackColor( COLORREAL * pCol)		{ i3Color::Set( &m_BackColor, pCol); }

	COLORREAL *	getLightDiffuse(void)				{ return &m_LightDiffuse; }
	void		setLightDiffuse( COLORREAL * pCol)	{ i3Color::Set( &m_LightDiffuse, pCol); }

	COLORREAL *	getLightSpecular(void)				{ return &m_LightSpecular; }
	void		setLightSpecular( COLORREAL * pCol)	{ i3Color::Set( &m_LightSpecular, pCol); }

	INT32		getShadowMapSize(void)				{ return m_ShadowMapSize; }
	void		setShadowMapSize( INT32 val)		{ m_ShadowMapSize = val; }

	void		Load(void);
	void		Save(void);
};

#endif
