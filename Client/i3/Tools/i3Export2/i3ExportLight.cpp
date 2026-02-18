#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"

i3Light * i3Export::CreateLightObject( INode * pNode)
{
	LightObject * pLightObj = (LightObject *) pNode->GetObjOrWSMRef();
	i3Light * pLight;
	TimeValue time = m_timeStart;
	Interface * ip = GetCOREInterface();
	Interval interval = FOREVER;
	LightState lightState;

	pLightObj->EvalLightState( time, interval, &lightState);

	pLight = i3Light::new_object_ref();

	switch( lightState.type)
	{
		case OMNI_LGT :		pLight->SetLightType( I3G_LIGHT_POINT);	break;
		case SPOT_LGT :		pLight->SetLightType( I3G_LIGHT_SPOT);	break;
		case DIRECT_LGT :	pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);	break;
		default :			pLight->SetLightType( I3G_LIGHT_DIRECTIONAL);	break;
	}

	{
		COLORREAL col;
		REAL32 intense = lightState.intens;

		i3Color::Set( &col, lightState.color.r * intense, lightState.color.g * intense, lightState.color.b * intense, 1.0f);
		pLight->SetDiffuse( &col);

		if( lightState.affectSpecular)
		{
			i3Color::Set( &col, lightState.color.r * intense, lightState.color.g * intense, lightState.color.b * intense, 1.0f);
		}
		else
		{
			i3Color::Set( &col, 0.0f, 0.0f, 0.0f, 0.0f);
		}

        pLight->SetSpecular( &col);

		pLight->setOuterAngle( I3_DEG2RAD( lightState.fallsize));
		pLight->setInnerAngle( I3_DEG2RAD( lightState.hotsize));

		pLight->setRange( lightState.attenEnd);
	}

	return pLight;
}

void i3Export::_OnExportLight( INode * pNode, ObjectState * pState)
{
	i3Light * pLight;

	pLight = CreateLightObject( pNode);

	if( pLight != NULL)
	{
		i3LightAttr * pAttr;

		pAttr = i3LightAttr::new_object_ref();
		pAttr->SetLightObject( pLight);

		i3LightSet * pLightSet = i3LightSet::new_object_ref();

		pLightSet->AddLightAttr( pAttr);

		m_pCurProcessInfo->pushi3Node( pLightSet, true);

		m_LightCount++;
	}

}
