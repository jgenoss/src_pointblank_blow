#include "stdafx.h"
#include "i3Viewer2Axis.h"

I3_CLASS_INSTANCE( i3Viewer2Axis);

i3Viewer2Axis::i3Viewer2Axis()
{
	for(UINT8 i = 0; i < 3; i++)
	{
		m_pAxis[i] = NULL;
		m_pAxisAttrSet[i] = NULL;
		m_pAxis_ColorAttr[i] = NULL;
	}

	m_BoundMin = VEC3D( 0.0f, 0.0f, 0.0f);
	m_BoundMax = m_BoundMin;

	m_pTargetTransform = NULL;
}

i3Viewer2Axis::~i3Viewer2Axis()
{
	I3_SAFE_RELEASE(m_pTargetTransform);
}

void i3Viewer2Axis::CreateAxis()
{
	i3Node * pNode;
	i3AttrSet * pAxisRoot;	
	INT32 i;
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

	COLOR	s_AxisColor[3] =
	{
		COLOR(	255, 0, 0, 255 ),
		COLOR(	0, 255, 0, 255 ),
		COLOR(	0, 0, 255, 255 )
	};

	for( i = 0; i < 3; i++)
	{
		m_pAxis[i] = i3Transform::new_object_ref();
		pAxisRoot->AddChild( m_pAxis[i]);

		m_pAxisAttrSet[i] = i3AttrSet::new_object_ref();
		m_pAxis[i]->AddChild( m_pAxisAttrSet[i]);

		{
			m_pAxisAttrSet[i]->AddAttr( pShaderAttr);

			m_pAxis_ColorAttr[i] = i3ColorAttr::new_object_ref();
			m_pAxis_ColorAttr[i]->Set( &s_AxisColor[i]);
			m_pAxisAttrSet[i]->AddAttr( m_pAxis_ColorAttr[i]);
		}

		{
			pNode = i3SceneUtil::BuildAxisMesh( 20, 20.0f, 1.0f, TRUE, FALSE, NULL, NULL); 

			m_pAxisAttrSet[i]->AddChild( pNode);
			I3_MUST_RELEASE(pNode);
		}
	}

	i3Matrix::SetRotateZDeg( m_pAxis[1]->GetMatrix(), 90.0f);
	i3Matrix::SetRotateYDeg( m_pAxis[2]->GetMatrix(), 90.0f);

}

void i3Viewer2Axis::SetScale(REAL32 fScale)
{
	INT32 i = 0;
	for(i = 0; i < 3; i++)
	{
		i3Matrix::SetScale(m_pAxis[i]->GetMatrix(), fScale, fScale, fScale);
	}
}

void i3Viewer2Axis::Enable()
{
	RemoveFlag(I3_NODEFLAG_DISABLE);
	INT32 i;
	for(i = 0; i < 3; i++)
	{
		m_pAxis[i]->RemoveFlag(I3_NODEFLAG_DISABLE);
	}
}

void i3Viewer2Axis::Disable()
{
	AddFlag(I3_NODEFLAG_DISABLE);
	INT32 i;
	for(i = 0; i < 3; i++)
	{
		m_pAxis[i]->AddFlag(I3_NODEFLAG_DISABLE);
	}
}