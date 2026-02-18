#include "stdafx.h"
#include "i3LevelCommon.h"

//I3_CLASS_INSTANCE( i3LevelCommon, i3AttrSet );
I3_CLASS_INSTANCE( i3LevelCommon);

i3LevelCommon::i3LevelCommon()
{
}

i3LevelCommon::~i3LevelCommon()
{
}

void	i3LevelCommon::InitAttrSet()
{
	SetName( "Unit Root" );

	m_pRenderModeAttr		= i3RenderModeAttr::new_object_ref();
	m_pFogEnableAttr		= i3FogEnableAttr::new_object_ref();
	m_pFogColorAttr			= i3FogColorAttr::new_object_ref();
	m_pFogModeAttr			= i3FogModeAttr::new_object_ref();
	m_pFogDensityAttr		= i3FogDensityAttr::new_object_ref();
	m_pFogNearFarAttr		= i3FogNearFarAttr::new_object_ref();
	m_pLightingEnableAttr	= i3LightingEnableAttr::new_object_ref();

	m_pLightingEnableAttr->Set( TRUE);
		
	AddAttr( m_pRenderModeAttr );
	AddAttr( m_pFogEnableAttr );
	AddAttr( m_pFogColorAttr );
	AddAttr( m_pFogModeAttr );
	AddAttr( m_pFogDensityAttr );
	AddAttr( m_pFogNearFarAttr );
	AddAttr( m_pLightingEnableAttr );

	{
		i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

		pAttr->SetFunction( I3G_COMP_GREATER);
		pAttr->SetRefValue( 0);		
		AddAttr( pAttr);
	}

	{
		i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

		pAttr->SetSource( I3G_BLEND_SRCALPHA);
		pAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
		AddAttr( pAttr);
	}
}