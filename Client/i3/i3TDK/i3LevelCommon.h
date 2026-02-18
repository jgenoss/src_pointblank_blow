#pragma once

class I3_EXPORT_TDK i3LevelCommon : public i3AttrSet
{
	I3_EXPORT_CLASS_DEFINE( i3LevelCommon, i3AttrSet);

public:
	i3LevelCommon();
	virtual ~i3LevelCommon();

public:
	void	InitAttrSet();

public:
	///////////////////////////////
	// Render State
	void			SetFogEnable( BOOL bFlag)					{ m_pFogEnableAttr->Set( bFlag); }
	BOOL			GetFogEnable()								{ return m_pFogEnableAttr->Get(); }

	void			SetFogColor( COLOR * pColor)				{ m_pFogColorAttr->Set( pColor); }
	COLOR *			GetFogColor()								{ return m_pFogColorAttr->Get(); }

	void			SetFogMode( I3G_FOG_MODE mode)				{ m_pFogModeAttr->Set( mode); }
	I3G_FOG_MODE	GetFogMode()								{ return m_pFogModeAttr->Get(); }

	void			SetFogDensity( REAL32 val)					{ m_pFogDensityAttr->Set( val); }
	REAL32			GetFogDensity()								{ return m_pFogDensityAttr->Get(); }

	void			SetFogNear( REAL32 fNear)					{ m_pFogNearFarAttr->SetNear( fNear); }
	REAL32			GetFogNear()								{ return m_pFogNearFarAttr->GetNear(); }

	void			SetFogFar( REAL32 fFar)						{ m_pFogNearFarAttr->SetFar( fFar); }
	REAL32			GetFogFar()									{ return m_pFogNearFarAttr->GetFar(); }

protected:
	i3RenderModeAttr *			m_pRenderModeAttr;
	i3FogEnableAttr *			m_pFogEnableAttr;
	i3FogColorAttr *			m_pFogColorAttr;
	i3FogModeAttr *				m_pFogModeAttr;
	i3FogDensityAttr *			m_pFogDensityAttr;
	i3FogNearFarAttr *			m_pFogNearFarAttr;
	i3LightingEnableAttr *		m_pLightingEnableAttr;
};