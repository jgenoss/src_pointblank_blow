#pragma once

#include "PropLight.h"

//res
#include "Prop_LvRes_Texture.h"
#include "Prop_LvRes_Animation.h"
#include "Prop_LvRes_Sound.h"
#include "Prop_LvRes_SceneGraph.h"
#include "Prop_LvRes_Character.h"

#include "Prop_LvElm_Primitive.h"
#include "Prop_LvElm_World.h"
#include "Prop_LvElm_Prim_Respawn.h"
#include "Prop_LvElm_Object.h"

#include "Prop_LvElm_CameraObj.h"
#include "Prop_LvElm_PathPoint.h"
#include "Prop_LvElm_PathLink.h"

// CPanelPropertySpecific dialog

class CPanelPropertySpecific : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPanelPropertySpecific)
protected:
	CPropLight	m_PropLight;

	//Res
	CProp_LvRes_Texture			m_Prop_LvresTexture;
	CProp_LvRes_Animation		m_Prop_LvresAnimation;
	CProp_LvRes_Sound			m_Prop_LvresSound;
	CProp_LvRes_SceneGraph		m_Prop_LvresSceneGraph;
	CProp_LvRes_Character		m_Prop_LvresChara;

	// Elements
	CProp_LvElm_Primitive		m_Prop_LvElm_Prim;
	CProp_LvElm_World			m_Prop_LvElm_World;
	CProp_LvElm_Prim_Respawn	m_Prop_LvElm_Prim_Respawn;
	Prop_LvElm_Object			m_Prop_LvElm_Object;

	CProp_LvElm_CameraObj		m_Prop_LvElm_Camera;
	CProp_LvElm_PathPoint		m_Prop_LvElm_PathPoint;
	CProp_LvElm_PathLink		m_Prop_LvElm_PathLink;

protected:

public:
	CPanelPropertySpecific(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelPropertySpecific();

// Dialog Data
	enum { IDD = IDD_PROP_SPECIFIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual i3ClassMeta *	OnQuerySourceObject( i3ElementBase * pElm) override;
};
