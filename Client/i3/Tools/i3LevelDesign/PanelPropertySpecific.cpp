// PanelPropertySpecific.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelPropertySpecific.h"
#include "GlobalVariable.h"


// CPanelPropertySpecific dialog

IMPLEMENT_DYNAMIC(CPanelPropertySpecific, i3TDKDialogContainer)
CPanelPropertySpecific::CPanelPropertySpecific(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPanelPropertySpecific::IDD, pParent)
{
}

CPanelPropertySpecific::~CPanelPropertySpecific()
{
}

void CPanelPropertySpecific::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelPropertySpecific, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPanelPropertySpecific message handlers

BOOL CPanelPropertySpecific::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_PropLight.Create( CPropLight::IDD, this);
		AddSpecDialog( i3LevelLight::static_meta(), &m_PropLight);

		m_Prop_LvElm_Prim.Create( CProp_LvElm_Primitive::IDD, this);
		AddSpecDialog( i3LevelPrimitive::static_meta(), &m_Prop_LvElm_Prim);

		m_Prop_LvElm_World.Create( CProp_LvElm_World::IDD, this);
		AddSpecDialog( i3LevelWorld::static_meta(), &m_Prop_LvElm_World);

		m_Prop_LvresTexture.Create( CProp_LvRes_Texture::IDD, this);
		AddSpecDialog( i3LevelResTexture::static_meta(), &m_Prop_LvresTexture);

		m_Prop_LvresAnimation.Create( CProp_LvRes_Animation::IDD, this);
		AddSpecDialog( i3LevelResAnimation::static_meta(), &m_Prop_LvresAnimation);

		m_Prop_LvresSound.Create( CProp_LvRes_Sound::IDD, this);
		AddSpecDialog( i3LevelResSound::static_meta(), &m_Prop_LvresSound);

		m_Prop_LvresSceneGraph.Create( CProp_LvRes_SceneGraph::IDD, this);
		AddSpecDialog( i3LevelResSceneGraph::static_meta(), &m_Prop_LvresSceneGraph);

		m_Prop_LvresChara.Create( CProp_LvRes_Character::IDD, this);
		AddSpecDialog( i3LevelResChara::static_meta(), &m_Prop_LvresChara);

		m_Prop_LvElm_Prim_Respawn.Create( CProp_LvElm_Prim_Respawn::IDD, this);
		AddSpecDialog( i3LevelRespawn::static_meta(), &m_Prop_LvElm_Prim_Respawn);

		m_Prop_LvElm_Object.Create( Prop_LvElm_Object::IDD, this);
		AddSpecDialog( i3LevelObject::static_meta(), &m_Prop_LvElm_Object);

		m_Prop_LvElm_Camera.Create( CProp_LvElm_CameraObj::IDD, this);
		AddSpecDialog( i3LevelCameraObject::static_meta(), &m_Prop_LvElm_Camera);

		m_Prop_LvElm_PathPoint.Create( CProp_LvElm_PathPoint::IDD, this);
		AddSpecDialog( i3LevelPathPoint::static_meta(), &m_Prop_LvElm_PathPoint);

		m_Prop_LvElm_PathLink.Create( CProp_LvElm_PathLink::IDD, this);
		AddSpecDialog( i3LevelPathLink::static_meta(), &m_Prop_LvElm_PathLink);
	}

	if( g_pInterface != NULL)
	{
		INT32 i, cnt;
		i3ClassMeta * pMeta;
		i3GameObjPropertyDialog * pDlg;

		cnt = g_pInterface->OnQueryPropertyDlgCount();

		for( i = 0; i < cnt; i++)
		{
			pDlg = NULL;
			g_pInterface->OnQueryPropertyDlg( g_hInterface, i, &pMeta, &pDlg);

			if( pDlg != NULL)
			{
				AddSpecDialog( pMeta, pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
			}
		}
	}

	i3TDK::RegisterUpdate( m_hWnd, i3LevelScene::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

i3ClassMeta * CPanelPropertySpecific::OnQuerySourceObject( i3ElementBase * pObj)
{
	if( pObj == NULL)
		return NULL;

	if( i3::kind_of<i3LevelElement3D*>(pObj)) //->IsTypeOf( i3LevelElement3D::static_meta()))
	{
		i3LevelElement3D * pElm = (i3LevelElement3D *) pObj;

		if( pElm->getInstanceClassName()[0] != 0)
		{
			i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(pElm->getInstanceClassName());
								//i3ClassMeta::FindClassMetaByName( pElm->getInstanceClassName());

			if( pMeta != NULL)
				return pMeta;
		}
	}

	return i3TDKDialogContainer::OnQuerySourceObject( pObj);
}

void CPanelPropertySpecific::OnOK()
{
}

void CPanelPropertySpecific::OnCancel()
{
}

void CPanelPropertySpecific::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::kind_of<i3LevelScene *>( pInfo->m_pMeta))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_REMOVE :
				PopupDialog( NULL, false);
				break;
		}
	}
}

LRESULT CPanelPropertySpecific::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogContainer::WindowProc(message, wParam, lParam);
}

