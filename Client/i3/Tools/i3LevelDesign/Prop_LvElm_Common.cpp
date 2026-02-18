// Prop_LvElm_Common.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_Common.h"
#include ".\prop_lvelm_common.h"


// CProp_LvElm_Common dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_Common, i3TDKDialogBase)
CProp_LvElm_Common::CProp_LvElm_Common(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_Common::IDD, pParent)
{
	m_pCurElm = NULL;
	m_pMemStream = NULL;
}

CProp_LvElm_Common::~CProp_LvElm_Common()
{
	I3_SAFE_RELEASE( m_pMemStream);
}

void CProp_LvElm_Common::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_RES_PATH, m_ResPathCtrl);
	DDX_Control(pDX, IDC_ED_RES_PATH_HASH, m_ResHashCtrl);
	DDX_Control(pDX, IDC_CB_INSTANCE_CLASS, m_InstanceClassCtrl);
	DDX_Control(pDX, IDC_ED_INSTANCE_CLASS_HASH, m_InstanceClassHashCtrl);
	DDX_Control(pDX, IDC_CHK_INITSTATE, m_InitStateCtrl);
	DDX_Control(pDX, IDC_CB_DYNAMIC_STATE, m_DynamicStateCtrl);
	DDX_Control(pDX, IDC_CB_NETWORK_SYNC, m_NetworkSyncCtrl);
	DDX_Control(pDX, IDC_ED_NAME, m_ED_Name);
}

void CProp_LvElm_Common::_UpdateResPath( const char * pszPath)
{
	UINT32 crc;
	char conv[128];

	m_ResPathCtrl.SetWindowText( pszPath);

	crc = CRC32( 0xFFFFFFFF, (UINT8 *) pszPath, strlen( pszPath));
	sprintf( conv, "0x%08X", crc);
	m_ResHashCtrl.SetWindowText( conv);
}

void CProp_LvElm_Common::_UpdateInstanceClass( const char * pszName)
{
	UINT32 crc;
	char conv[128];

	m_InstanceClassCtrl.SetWindowText( pszName);

	crc = CRC32( 0xFFFFFFFF, (UINT8 *) pszName, strlen( pszName));
	sprintf( conv, "0x%08X", crc);
	m_InstanceClassHashCtrl.SetWindowText( conv);
}

void CProp_LvElm_Common::_AssignValue( i3LevelElement3D * pElm)
{
	char	szResPath[MAX_PATH], szInstName[256];
	bool	bInitState;
	INT32	dynState;
	INT32	netSync;
	
	szResPath[0] = 0;
	szInstName[0] = 0;
	bInitState = false;
	dynState = -1;
	netSync = -1;
	m_pCurElm = pElm;

	if( pElm != NULL)
	{
		// 선택된 Elm가 없거나, Multi-Select 된 경우...
		// 모두 동일한 값을 가진 경우에만 해당 값을 보여주고,
		// 다르다면 빈 상태로 그냥 둔다.
		i3::vector<i3LevelElement3D*> list;
		i3LevelElement3D * pObj;
		i3LevelRes * pRes;
		bool bCheckResPath, bCheckInstName,  bCheckInitState, bCheckDynamicState, bCheckNetworkSync;

		g_pScene->GetSelectedElements( list);
		bCheckResPath = true;
		bCheckInstName = true;
		bCheckInitState = true;
		bCheckDynamicState = true;
		bCheckNetworkSync = true;

		if( list.size() == 1)
		{
			m_ED_Name.SetWindowText( pElm->GetName());
			m_ED_Name.EnableWindow( TRUE);
		}
		else
		{
			m_ED_Name.SetWindowText( "");
			m_ED_Name.EnableWindow( FALSE);
		}

		for(size_t i = 0; i < list.size(); i++)
		{
			pObj = list[i];
			pRes = pObj->getRes();

			if( i == 0)
			{
				if( pRes != NULL)
				{
					strcpy( szResPath, pRes->getPath());
				}
				else
				{
					szResPath[0] = 0;
				}

				strcpy( szInstName, pObj->getInstanceClassName());
				bInitState = pObj->IsState( I3_LEVEL_STYLE_INIT_DISABLE) != FALSE;
				dynState = pObj->getDynamicState();
				netSync = (INT32) pObj->getNetworkSyncMode();
			}
			else
			{
				if( bCheckResPath)
				{
					if( pRes != NULL)
					{
						if( i3::generic_is_iequal( szResPath, pRes->getPath()) == false)
						{
							// 다르다.
							strcpy( szResPath, "(More than one elements are selected.)");
							bCheckResPath = false;
						}
					}
				}

				if( bCheckInstName)
				{
					if( i3::generic_is_iequal( szInstName, pObj->getInstanceClassName()) == false)
					{
						// 다르다
						strcpy( szInstName, "");
						bCheckInstName = false;
					}
				}

				if( bCheckInitState)
				{
					if( bInitState != (pObj->IsState( I3_LEVEL_STYLE_INIT_DISABLE) == FALSE))
					{
						bInitState = false;
						bCheckInitState = false;
					}
				}

				if( bCheckDynamicState)
				{
					if( dynState != pObj->getDynamicState())
					{
						dynState = -1;
						bCheckDynamicState = false;
					}
				}

				if( bCheckNetworkSync)
				{
					if( netSync != pObj->getNetworkSyncMode())
					{
						netSync = -1;
						bCheckDynamicState = false;
					}
				}
			}
		}
	}
	else
	{
		i3LevelRes * pRes = pElm->getRes();

		if( pRes == NULL)
		{
			strcpy( szResPath, "Not available");
		}
		else
		{
			strcpy( szResPath, pRes->getPath());
		}

		strcpy( szInstName, pElm->getInstanceClassName());
		bInitState = (pElm->IsState( I3_LEVEL_STYLE_INIT_DISABLE) == FALSE);
		dynState = pElm->getDynamicState();

		m_ED_Name.SetWindowText( "");
		m_ED_Name.EnableWindow( FALSE);
	}

	_UpdateResPath( szResPath);

	if( szInstName[0] != 0)
	{
		_UpdateInstanceClassList( pElm);
	}
	else
	{
		_UpdateInstanceClassList( NULL);
	}

	_UpdateInstanceClass( szInstName);

	if( bInitState)
		m_InitStateCtrl.SetCheck( BST_CHECKED);
	else
		m_InitStateCtrl.SetCheck( BST_UNCHECKED);

	m_DynamicStateCtrl.SetCurSel( dynState);

	m_NetworkSyncCtrl.SetCurSel( netSync);
}

void CProp_LvElm_Common::_UpdateInstanceClassList( i3LevelElement3D * pElm)
{
	i3ClassMeta * pMeta;

	m_InstanceClassCtrl.ResetContent();

	if( pElm == NULL)
		return;

	i3GameObj * pObj = pElm->CreateGameInstance();

	if( pObj == NULL)
	{
		pMeta = i3GameObj::static_meta();
	}
	else
	{
		pMeta = pObj->meta();
		pObj->Release();
	}

	i3ClassMeta * pBaseMeta;

	// i3GameObj ClassMeta가 발견될 때까지 거슬러 올라간다.
	pBaseMeta = pMeta;

	while( !i3::same_of<i3GameObj*>(pBaseMeta)) //->IsExactTypeOf( i3GameObj::static_meta()) == FALSE)
	{
		pMeta = pBaseMeta;
		pBaseMeta = pMeta->get_parent();
	}

	m_InstanceClassCtrl.AddString( pMeta->class_name().c_str() );

	// pMeta에서 파생된 모든 Class를 구한다.
	i3::vector<i3ClassMeta*> List;
	i3ClassMeta * pTempMeta;


	pMeta->get_all_derived_metas( List);

	for(size_t i = 0; i < List.size(); i++)
	{
		pTempMeta = List[i];

		if( pTempMeta != pMeta)
			m_InstanceClassCtrl.AddString( pTempMeta->class_name().c_str());
	}
}

void	CProp_LvElm_Common::SetObject( i3ElementBase * pObj)
{
	if( i3::kind_of<i3LevelElement3D*>(pObj)) //->IsTypeOf( i3LevelElement3D::static_meta()))
	{
		_AssignValue( (i3LevelElement3D *) pObj);
	}
}

BEGIN_MESSAGE_MAP(CProp_LvElm_Common, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_CB_DYNAMIC_STATE, OnCbnSelchangeCbDynamicState)
	ON_CBN_KILLFOCUS(IDC_CB_INSTANCE_CLASS, OnCbnKillfocusCbInstanceClass)
	ON_CBN_SELCHANGE(IDC_CB_INSTANCE_CLASS, OnCbnSelchangeCbInstanceClass)
	ON_BN_CLICKED(IDC_CHK_INITSTATE, OnBnClickedChkInitstate)
	ON_CBN_SELCHANGE(IDC_CB_NETWORK_SYNC, OnCbnSelchangeCbNetworkSync)
	ON_EN_KILLFOCUS(IDC_ED_NAME, OnEnKillfocusEdName)
END_MESSAGE_MAP()


// CProp_LvElm_Common message handlers

BOOL CProp_LvElm_Common::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_Common::OnOK()
{
	OnEnKillfocusEdName();
}

void CProp_LvElm_Common::OnCancel()
{
}

void CProp_LvElm_Common::OnCbnSelchangeCbDynamicState()
{
	 i3::vector<i3LevelElement3D*> list;
	
	I3LV_DYNAMIC_STATE state;
	i3LevelElement3D * pObj = NULL;

	if( m_DynamicStateCtrl.GetCurSel() == -1)
		return;

	state = (I3LV_DYNAMIC_STATE) m_DynamicStateCtrl.GetCurSel();

	g_pScene->GetSelectedElements( list);

	i3LevelUndo::ADD_Edit( g_pScene);

	for(size_t i = 0; i < list.size(); i++)
	{
		pObj = list[i];

		pObj->setDynamicState( state);
	}

	if( list.size() == 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pObj);
	}
	else if( list.size() > 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, NULL, i3LevelElement3D::static_meta());
	}
}

void CProp_LvElm_Common::OnCbnKillfocusCbInstanceClass()
{
	char szName[256];
	 i3::vector<i3LevelElement3D*> list;

	i3LevelElement3D * pObj = NULL;

	m_InstanceClassCtrl.GetWindowText( szName, sizeof(szName) - 1);

	if( szName[0] == 0)
		return;

	g_pScene->GetSelectedElements( list);

	for(size_t i = 0; i < list.size(); i++)
	{
		pObj = list[i];

		pObj->setInstanceClassName( szName);
	}

	if( list.size() == 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pObj);
	}
	else if( list.size() > 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, NULL, i3LevelElement3D::static_meta());
	}
}

void CProp_LvElm_Common::OnCbnSelchangeCbInstanceClass()
{
	INT32 idx = m_InstanceClassCtrl.GetCurSel();
	char conv[256];

	m_InstanceClassCtrl.GetLBText( idx, conv);

	m_InstanceClassCtrl.SetWindowText( conv);

	i3LevelUndo::ADD_Edit( g_pScene);

	OnCbnKillfocusCbInstanceClass();
}

void CProp_LvElm_Common::OnBnClickedChkInitstate()
{
	 i3::vector<i3LevelElement3D*> list;
	
	i3LevelElement3D * pObj = NULL;
	I3LV_DYNAMIC_STATE state;

	state = (I3LV_DYNAMIC_STATE) m_DynamicStateCtrl.GetCurSel();
	if( state == -1)
		return;
	
	g_pScene->GetSelectedElements( list);

	i3LevelUndo::ADD_Edit( g_pScene);

	for(size_t i = 0; i < list.size(); i++)
	{
		pObj = list[i];

		pObj->setDynamicState( state);
	}

	if( list.size() == 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pObj);
	}
	else if( list.size() > 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, NULL, i3LevelElement3D::static_meta());
	}
}

void CProp_LvElm_Common::OnCbnSelchangeCbNetworkSync()
{

	I3_NETWORK_GAMEOBJECT_TYPE type;
	i3LevelElement3D * pObj = NULL;
	 i3::vector<i3LevelElement3D*> list;

	type = (I3_NETWORK_GAMEOBJECT_TYPE) m_NetworkSyncCtrl.GetCurSel();

	g_pScene->GetSelectedElements( list);

	i3LevelUndo::ADD_Edit( g_pScene);
	
	for(size_t i = 0; i < list.size(); i++)
	{
		pObj = list[i];

		pObj->setNetworkSyncMode( type);
	}

	if( list.size() == 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pObj);
	}
	else if( list.size() > 1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, NULL, i3LevelElement3D::static_meta());
	}
}

void CProp_LvElm_Common::OnEnKillfocusEdName()
{
	char conv[256];
	i3LevelElement3D * pElm;
	 i3::vector<i3LevelElement3D*> list;

	if( m_ED_Name.GetModify() == FALSE)
		return;

	m_ED_Name.SetModify( FALSE);

	m_ED_Name.GetWindowText( conv, sizeof(conv));

	if( conv[0] == 0)
		return;

	g_pScene->GetSelectedElements( list);
	
	if( list.size() <= 0)
		return;

	i3LevelUndo::ADD_Edit( g_pScene);

	pElm = list[0];

	INT32 idx = g_pScene->FindElementByName( conv);

	if( idx != -1)
	{
		NotifyBox( m_hWnd, "이미 동일 이름의 Element가 존재합니다. 잘 못된 이름입니다.");

		m_ED_Name.SetWindowText( pElm->GetName());
		return;
	}

	pElm->SetName( conv);

	{
		i3LevelRes * pRes = pElm->getRes();

		if( pRes != NULL)
		{
			_UpdateResPath( pRes->getPath());
		}
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pElm);
}
