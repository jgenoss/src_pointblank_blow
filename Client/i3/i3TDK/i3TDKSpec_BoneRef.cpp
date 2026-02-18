// i3TDKSpec_BoneRef.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_BoneRef.h"
#include ".\i3tdkspec_boneref.h"


// i3TDKSpec_BoneRef dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_BoneRef, i3TDKDialogBase)

i3TDKSpec_BoneRef::~i3TDKSpec_BoneRef()
{
	I3_SAFE_RELEASE( m_pBoneRef);
}

void i3TDKSpec_BoneRef::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_BONE, m_CB_Bones);
}

i3Body * i3TDKSpec_BoneRef::_FindBody(void)
{
	if( m_pBoneRef == nullptr)
		return nullptr;

	i3Node * pTemp = m_pBoneRef;

	while( (pTemp != nullptr) &&	!i3::same_of<i3Body*>(pTemp))
	{
		if( pTemp->GetParent() == nullptr && pTemp->GetOwner() != nullptr)
		{
			i3ElementBase * pObj = pTemp->GetOwner();
			if( i3::same_of<i3LOD*>( pObj))
			{
				pTemp = ((i3LOD*)pObj)->getBody();
			}
		}
		else
			pTemp = pTemp->GetParent();
	}

	return (i3Body *) pTemp;
}

void i3TDKSpec_BoneRef::_UpdateBoneList( i3Body * pBody)
{
	i3LOD * pLOD = pBody->getFrameLOD();
	
	if( pLOD == nullptr)
		return;

	INT32 i;
	i3Skeleton * pSkel = pLOD->getSkeleton();

	for( i = 0; i < pSkel->getBoneCount(); i++)
	{
		TCHAR conv[512];

		i3::snprintf( conv, _countof(conv), _T("[%d] - %s"), i, pSkel->getBone(i)->getName());
		m_CB_Bones.AddString( conv);
	}
}

void i3TDKSpec_BoneRef::SetObject( i3ElementBase * pObj)
{
	INT32 idx;
	i3Body * pBody = nullptr;

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pBoneRef, ((i3BoneRef *) pObj));

	if( m_pBoneRef != nullptr)
	{
		pBody = _FindBody();

		if( pBody != nullptr)
		{
			_UpdateBoneList( pBody);

			idx = m_pBoneRef->getBoneIndex();
		}
		else
		{
			idx = -1;
		}

		m_CB_Bones.SetCurSel( idx);
	}

	m_CB_Bones.EnableWindow( m_pBoneRef != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_BoneRef, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_BONE, OnCbnSelchangeTdkCbBone)
END_MESSAGE_MAP()


// i3TDKSpec_BoneRef message handlers

BOOL i3TDKSpec_BoneRef::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_BoneRef::OnOK()
{
}

void i3TDKSpec_BoneRef::OnCancel()
{
}

LRESULT i3TDKSpec_BoneRef::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_BoneRef::OnCbnSelchangeTdkCbBone()
{
	if( m_pBoneRef == nullptr)
		return;

	INT32 idx = m_CB_Bones.GetCurSel();

	if( idx != -1)
		m_pBoneRef->setBoneIndex( idx);
}
