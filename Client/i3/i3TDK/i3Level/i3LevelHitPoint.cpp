#include "stdafx.h"
#include "i3LevelHitPoint.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelHitPoint);

i3LevelHitPoint::i3LevelHitPoint(void)
{
	RemoveStyle( I3_LEVEL_STYLE_VOLATILE);

	if (m_pRoot != nullptr)
	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(FALSE);
		m_pRoot->AddAttr(pAttr);
	}
}

i3LevelHitPoint::~i3LevelHitPoint(void)
{
	I3_MUST_RELEASE(m_pTextNode);
}

void i3LevelHitPoint::Create(void)
{
	{
		i3LevelResSceneGraph * pRes = g_pScene->GetCommonHitMapResource();

		i3Node * pClone = i3Scene::CreateClone(pRes->GetSceneRoot(), I3_COPY_INSTANCE);
		SetSymbol(pClone);
	}
}

void i3LevelHitPoint::SetName(char* pchName, VEC3D* vec)
{
	m_pTextNode = i3TextNodeDX::new_object_ref();
	m_pTextNode->Create("Tahoma", 256, 8);
	m_pTextNode->setSize(256, 256);

	GetRoot()->AddChild(m_pTextNode);

	m_pTextNode->SetText(pchName);
}

void i3LevelHitPoint::UpdateNode()
{
	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();
	MATRIX viewproj;
	pFramework->GetViewProjectionMatrix(&viewproj);

	i3Frustum frustum;
	frustum.Set(&viewproj);

	i3BoundBox bb;
	VEC3D vecMin;
	VEC3D vecMax;

	GetTransformedBoundBox(&vecMin, &vecMax);
	bb.SetMinMax(&vecMin, &vecMax);
	if (0 == frustum.TestBox(&bb))
	{
		m_pTextNode->SetEnable(false);
		return;
	}
	else
	{
		m_pTextNode->SetEnable(true);
	}

	float viewX = getPos()->x * viewproj.m00 + getPos()->y * viewproj.m10 + getPos()->z * viewproj.m20 + viewproj.m30;
	float viewY = getPos()->x * viewproj.m01 + getPos()->y * viewproj.m11 + getPos()->z * viewproj.m21 + viewproj.m31;
	float viewZ = getPos()->x * viewproj.m02 +getPos()->y * viewproj.m12 + getPos()->z * viewproj.m22 + viewproj.m32;

	int screenX, screenY;
	screenX = (int)((viewX / viewZ + 1.0f) * pView->GetViewWidth() / 2.0f) - (int)((float)m_pTextNode->getTextWidth() / 2.0f);
	screenY = (int)((-viewY / viewZ + 1.0f) * pView->GetViewHeight() / 2.0f);

	if (m_pTextNode->getPosX() != screenX || m_pTextNode->getPosY() != screenY)
	{
		m_pTextNode->setPos(screenX, screenY);		//text update
	}
}

void i3LevelHitPoint::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelControl::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelHitPoint*>(pDest));
}
