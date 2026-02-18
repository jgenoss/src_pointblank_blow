#include "stdafx.h"
#include "i3Viewer2LightAxis.h"

I3_CLASS_INSTANCE(i3Viewer2LightAxis);

i3Viewer2LightAxis::i3Viewer2LightAxis()
{
	m_pAxis = NULL;
	m_pAxisAttrSet = NULL;
	m_pAxis_ColorAttr = NULL;
	m_pLightSphere = NULL;
}

i3Viewer2LightAxis::~i3Viewer2LightAxis()
{
}

void i3Viewer2LightAxis::CreateLightAxis()
{
	i3Node * pNode;
	i3AttrSet * pAxisRoot;	
	i3ShaderAttr * pShaderAttr;

	i3Matrix::Identity(GetMatrix());

	pAxisRoot = i3AttrSet::new_object_ref();
	pAxisRoot->SetName("Axis");
	pAxisRoot->SetPriority(0);

	this->AddChild(pAxisRoot);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		pAxisRoot->AddAttr( pAttr);
	}

	{
		pShaderAttr = i3ShaderAttr::new_object_ref();
		pShaderAttr->setAddCacheState( false);
		pShaderAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_2);
		pShaderAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_2);
		pShaderAttr->setSource( "i3Gizmo.hlsl");
	}

	COLOR	s_AxisColor = COLOR(255, 255, 0, 255);

	m_pAxis = i3Transform::new_object_ref();
	pAxisRoot->AddChild( m_pAxis);

	m_pAxisAttrSet = i3AttrSet::new_object_ref();
	m_pAxis->AddChild( m_pAxisAttrSet);

	{
		m_pAxisAttrSet->AddAttr( pShaderAttr);

		m_pAxis_ColorAttr = i3ColorAttr::new_object_ref();
		m_pAxis_ColorAttr->Set( &s_AxisColor);
		m_pAxisAttrSet->AddAttr( m_pAxis_ColorAttr);
	}

	{
		pNode = i3SceneUtil::BuildAxisMesh( 20, 15.0f, 0.75f, TRUE, FALSE, NULL, NULL); 

		m_pAxisAttrSet->AddChild( pNode);
		I3_MUST_RELEASE(pNode);
	}

	{
		COLORREAL lineColor, aColor, dColor;

		i3Color::Set( &lineColor, 1.0f, 0.8f, 0.0f, 1.0f);
		i3Color::Set( &aColor, 0.2f, 0.2f, 0.2f, 1.0f);
		i3Color::Set( &dColor, 1.0f, 0.8f, 0.0f, 1.0f);

		m_pLightSphere = i3SceneUtil::BuildSphereMesh(1.5f, TRUE, TRUE, 20, 10, &lineColor, &aColor, &dColor);
		this->AddChild(m_pLightSphere);
		I3_MUST_RELEASE(m_pLightSphere);
	}

	i3Matrix::SetRotateYDeg( m_pAxis->GetMatrix(), 90.0f);

}

void i3Viewer2LightAxis::SetVisible(bool bVisible)
{
	if(bVisible == TRUE)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

void i3Viewer2LightAxis::Enable()
{
	RemoveFlag(I3_NODEFLAG_DISABLE);
	m_pAxis->RemoveFlag(I3_NODEFLAG_DISABLE);
	m_pLightSphere->RemoveFlag(I3_NODEFLAG_DISABLE);
}

void i3Viewer2LightAxis::Disable()
{
	AddFlag(I3_NODEFLAG_DISABLE);
	m_pAxis->AddFlag(I3_NODEFLAG_DISABLE);
	m_pLightSphere->AddFlag(I3_NODEFLAG_DISABLE);
}