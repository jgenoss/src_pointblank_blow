#include "stdafx.h"
#include "SgBuilder.h"
#include "i3Export.h"

void SgBuilder::_OnJobLight( JOB_INFO * pJob)
{
	i3Light * pLight;
	i3LightSet * pLightSet;

	pJob->m_bProcessed = true;

	pLight = m_pExport->CreateLightObject( pJob->m_pINode);

	{
		pLightSet = i3LightSet::new_object_ref();

		pLightSet->SetName( pJob->m_pINode->GetName());
	}

	{
		i3LightAttr * pAttr = i3LightAttr::new_object_ref();

		pAttr->SetLightObject( pLight);

		pLightSet->AddLightAttr( pAttr);
	}

	// Parent
	ATTACH_NODE( pLightSet, pJob->m_pINode);
}

