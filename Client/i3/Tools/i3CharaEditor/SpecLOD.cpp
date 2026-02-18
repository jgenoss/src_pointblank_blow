// SpecLOD.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecLOD.h"
#include "LOD.h"
#include "GlobalVar.h"
#include ".\speclod.h"
#include "i3Base/string/ext/extract_filename.h"

// CSpecLOD dialog

IMPLEMENT_DYNAMIC(CSpecLOD, i3TDKDialogBase)
CSpecLOD::CSpecLOD(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecLOD::IDD, pParent)
{
}

CSpecLOD::~CSpecLOD()
{ 
}

void CSpecLOD::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_SG, m_SgCtrl);
	DDX_Control(pDX, IDC_ED_TRICOUNT, m_TriCountCtrl);
	DDX_Control(pDX, IDC_ED_BONECOUNT, m_BoneCountCtrl);
	DDX_Control(pDX, IDC_CHK_CAST_SHADOW, m_CHK_DisableShadowCast);
}

void CSpecLOD::SetObject( i3ElementBase * pObj)
{
	INT32 i, idx, selIdx;
	char	szName[MAX_PATH];
	CLOD * pLOD;

	I3ASSERT( i3::kind_of<CLOD*>(pObj)); //->IsTypeOf( CLOD::static_meta()));

	pLOD = (CLOD *) pObj;
	I3_REF_CHANGE(m_pObject, pObj);

	// Scene-Graph
	{
		i3SceneGraphInfo * pInfo;

		m_SgCtrl.ResetContent();

		selIdx = -1;

		for( i = 0; i < g_pChara->getSceneGraphCount(); i++)
		{
			pInfo = g_pChara->getSceneGraph( i);

		//	i3String::SplitFileName( pInfo->GetName(), szName, FALSE);
			i3::extract_filetitle( pInfo->GetName(), szName);

			idx = m_SgCtrl.AddString( szName);
			m_SgCtrl.SetItemData( idx, (DWORD_PTR) pInfo);

			if( pInfo == pLOD->getSceneGraph())
				selIdx = idx;
		}

		m_SgCtrl.SetCurSel( selIdx);
	}

	// Shadow Casting
	{
		if( pLOD->getLOD()->getShapeNode()->IsFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER))
		{
			m_CHK_DisableShadowCast.SetCheck( BST_CHECKED);
		}
		else
		{
			m_CHK_DisableShadowCast.SetCheck( BST_UNCHECKED);
		}
	}

	_Update();
}

void CSpecLOD::_Update(void)
{
	CLOD * pLOD = (CLOD *) m_pObject;
	char conv[128];

	// Triangle Count
	{
		sprintf( conv, "%d", pLOD->getTriCount());
		m_TriCountCtrl.SetWindowText( conv);
	}

	// Bone Count
	{
		sprintf( conv, "%d", pLOD->getBoneCount());
		m_BoneCountCtrl.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(CSpecLOD, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_SG, OnCbnSelchangeCbSg)
	ON_BN_CLICKED(IDC_CHK_CAST_SHADOW, OnBnClickedChkCastShadow)
END_MESSAGE_MAP()


// CSpecLOD message handlers

void CSpecLOD::OnCbnSelchangeCbSg()
{
	INT32 idx;
	i3SceneGraphInfo * pInfo;
	CLOD * pLOD = (CLOD *) m_pObject;

	idx = m_SgCtrl.GetCurSel();
	if( idx == -1)
	{
		return;
	}

	pInfo = (i3SceneGraphInfo *) m_SgCtrl.GetItemData( idx);

	if( pInfo != pLOD->getSceneGraph())
	{
		pLOD->SetSceneGraph( pInfo, 0, true);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pLOD);
	}
}

void CSpecLOD::OnCancel()
{
}

void CSpecLOD::OnOK()
{
}

void CSpecLOD::OnBnClickedChkCastShadow()
{
	CLOD * pLOD = (CLOD *) m_pObject;

	if( pLOD == NULL)
		return;

	if( m_CHK_DisableShadowCast.GetCheck() == BST_CHECKED)
	{
		pLOD->getLOD()->getShapeNode()->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}
	else
	{
		pLOD->getLOD()->getShapeNode()->RemoveFlag( I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}
}
