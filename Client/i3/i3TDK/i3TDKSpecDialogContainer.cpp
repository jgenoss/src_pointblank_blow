// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSpecDialogContainer.h"
#include "resource.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKEditCtrl.h"

#include "i3TDKSpec_Node.h"
#include "i3TDKSpec_AttrSet.h"
#include "i3TDKSpec_Transform.h"
#include "i3TDKSpec_BoneRef.h"
#include "i3TDKSpec_Body.h"
#include "i3TDKSpec_Transform2.h"

#include "i3TDKSpec_Attr_Bool.h"
#include "i3TDKSpec_Attr_AlphaFunc.h"
#include "i3TDKSpec_Attr_BlendMode.h"
#include "i3TDKSpec_Attr_Color.h"
#include "i3TDKSpec_Attr_FaceCull.h"
#include "i3TDKSpec_Attr_FogColor.h"
#include "i3TDKSpec_Attr_FogDensity.h"
#include "i3TDKSpec_Attr_FogMode.h"
#include "i3TDKSpec_Attr_FogNearFar.h"
#include "i3TDKSpec_Attr_Fresnel.h"
#include "i3TDKSpec_Attr_Geometry.h"
#include "i3TDKSpec_Attr_TexBind.h"
#include "i3TDKSpec_Attr_TexMat.h"
#include "i3TDKSpec_Attr_TexWrap.h"
#include "i3TDKSpec_Attr_Material.h"
#include "i3TDKSpec_Attr_RenderMode.h"
#include "i3TDKSpec_Attr_ShadeMode.h"
#include "i3TDKSpec_Attr_Shader.h"

#define		BTN_BACK_CY			17

// i3TDKSpecDialogContainer dialog

IMPLEMENT_DYNAMIC(i3TDKSpecDialogContainer, i3TDKDialogBase)

i3TDKSpecDialogContainer::~i3TDKSpecDialogContainer()
{
	RemoveAllSpecDialog();
}

void	i3TDKSpecDialogContainer::AddSpecDialog( i3ClassMeta * pMeta, i3GameObjPropertyDialog * pSpecDlg, UINT32 mask, bool bInternal)
{
	I3TDK_SPECDLG_INFO * pInfo;

	pInfo = (I3TDK_SPECDLG_INFO *) i3MemAlloc( sizeof(I3TDK_SPECDLG_INFO));
	I3ASSERT( pInfo != nullptr);

	pInfo->m_bInternal = bInternal;
	pInfo->m_pMeta = pMeta;
	pInfo->m_pDlg = pSpecDlg;
	pInfo->m_PopupMask = mask;

	{
		CRect rt;

		pSpecDlg->GetWindowRect( &rt);

		pInfo->m_DefWidth = rt.Width();
		pInfo->m_DefHeight = rt.Height();
	}

	pSpecDlg->ShowWindow( SW_HIDE);
	pSpecDlg->SetParent( &m_FoldCtrl);

	i3TDK::RegisterUpdate( m_hWnd, pMeta, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC | I3_TDK_UPDATE_REMOVE);

	m_List.push_back( pInfo);
}

void	i3TDKSpecDialogContainer::RemoveAllSpecDialog(void)
{
	I3TDK_SPECDLG_INFO * pInfo;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pInfo = m_List[i];

		if((pInfo->m_bInternal == true) && (pInfo->m_pDlg != nullptr))
		{
			delete pInfo->m_pDlg;
		}

		i3MemFree( pInfo);
	}

	m_List.clear();
}

i3ClassMeta * i3TDKSpecDialogContainer::OnQuerySourceObject( i3ElementBase * pElm)
{
	if( pElm == nullptr)
		return nullptr;

	return pElm->meta();
}

void	i3TDKSpecDialogContainer::PopupDialog( i3ElementBase * pObj)
{
	CRect rect;
	i3ClassMeta * pMeta;
	i3::vector<I3TDK_SPECDLG_INFO*> list;

	m_pObj = pObj;

	pMeta	= OnQuerySourceObject( pObj);
	if((pMeta == nullptr) && (pObj != nullptr))
		pMeta = pObj->meta();

	if( pMeta != nullptr)
	{
		FindSpecDialog( pMeta, 0xFFFFFFFF, list);

		if( list.size() == 0)
		{
			FindSpecDialog( pObj->meta(), 0xFFFFFFFF, list);
		}
	}

	// 기존의 Spec들을 닫는다.
	for( size_t i = 0; i < m_PopList.size(); i++)
	{
		I3TDK_SPECDLG_INFO * pInfo = m_PopList[i];

		if( i3::vu::index_of(list, pInfo) == list.size() )
		{
			if( pInfo->m_pDlg != nullptr)
				pInfo->m_pDlg->ShowWindow( SW_HIDE);
		}
	}
	m_PopList.clear();

	if( list.size() > 0)
	{
		for(size_t i = 0; i < list.size(); i++)
		{
			I3TDK_SPECDLG_INFO * pInfo = list[i];
			i3GameObjPropertyDialog * pSpec = (i3GameObjPropertyDialog *) pInfo->m_pDlg;

			pSpec->SetObject( pObj);

			m_PopList.push_back( pInfo);
		}

		_SetPlaceSpecs();
	}
}

void i3TDKSpecDialogContainer::_SetPlaceSpecs(void)
{
	// 새 Spec들을 순서대로 배치한다.
	
	m_FoldCtrl.RemoveAllPane();

	for(size_t i = 0; i < m_PopList.size(); i++)
	{
		I3TDK_SPECDLG_INFO * pInfo = m_PopList[i];
		i3GameObjPropertyDialog * pSpec = (i3GameObjPropertyDialog *) pInfo->m_pDlg;

		pSpec->SetWindowPos( nullptr, 0, 0, pInfo->m_DefWidth, pInfo->m_DefHeight, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

#ifdef _UNICODE
		i3::stack_wstring wstrMeta;		i3::mb_to_utf16(pInfo->m_pMeta->class_name(), wstrMeta);
		m_FoldCtrl.AddPane( pSpec, wstrMeta.c_str());
#else
		m_FoldCtrl.AddPane( pSpec, pInfo->m_pMeta->class_name().c_str());
#endif
		pSpec->ShowWindow( SW_SHOW);
	}
}

INT32 i3TDKSpecDialogContainer::FindSpecDialog( i3ClassMeta * pMeta, UINT32 mask, i3::vector<I3TDK_SPECDLG_INFO*>& List)
{
	I3TDK_SPECDLG_INFO * pInfo;
	INT32 cnt = 0;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pInfo = m_List[i];

		if( pMeta->kind_of( pInfo->m_pMeta) && (pInfo->m_PopupMask & mask))
		{
			List.push_back( pInfo);
			cnt++;
		}
	}

	return cnt;
}

void i3TDKSpecDialogContainer::AddBuiltInSpecDialogs(void)
{
	i3TDK::SetResInstance();

	{	// i3Node
		i3TDKSpec_Node * pDlg = new i3TDKSpec_Node;

		pDlg->Create( i3TDKSpec_Node::IDD, this);

		AddSpecDialog( i3Node::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3AttrSet
		i3TDKSpec_AttrSet * pDlg = new i3TDKSpec_AttrSet;

		pDlg->Create( i3TDKSpec_AttrSet::IDD, this);

		AddSpecDialog( i3AttrSet::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3Transform
		i3TDKSpec_Transform * pDlg = new i3TDKSpec_Transform;

		pDlg->Create( i3TDKSpec_Transform::IDD, this);

		AddSpecDialog( i3Transform::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{
		// i3Transform
		i3TDKSpec_Transform2 * pDlg = new i3TDKSpec_Transform2;

		pDlg->Create( i3TDKSpec_Transform2::IDD, this);

		AddSpecDialog( i3Transform2::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3BoneRef
		i3TDKSpec_BoneRef * pDlg = new i3TDKSpec_BoneRef;

		pDlg->Create( i3TDKSpec_BoneRef::IDD, this);

		AddSpecDialog( i3BoneRef::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3BoneRef
		i3TDKSpec_Body * pDlg = new i3TDKSpec_Body;

		pDlg->Create( i3TDKSpec_Body::IDD, this);

		AddSpecDialog( i3Body::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3AbstractBoolAttr
		i3TDKSpec_Attr_Bool * pDlg = new i3TDKSpec_Attr_Bool;

		pDlg->Create( i3TDKSpec_Attr_Bool::IDD, this);

		AddSpecDialog( i3AbstractBoolAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3AlphaFuncAttr
		i3TDKSpec_Attr_AlphaFunc * pDlg = new i3TDKSpec_Attr_AlphaFunc;

		pDlg->Create( i3TDKSpec_Attr_AlphaFunc::IDD, this);

		AddSpecDialog( i3AlphaFuncAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3BlendModeAttr
		i3TDKSpec_Attr_BlendMode * pDlg = new i3TDKSpec_Attr_BlendMode;

		pDlg->Create( i3TDKSpec_Attr_BlendMode::IDD, this);

		AddSpecDialog( i3BlendModeAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3ColorAttr
		i3TDKSpec_Attr_Color * pDlg = new i3TDKSpec_Attr_Color;

		pDlg->Create( i3TDKSpec_Attr_Color::IDD, this);

		AddSpecDialog( i3ColorAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FaceCullModeAttr
		i3TDKSpec_Attr_FaceCull * pDlg = new i3TDKSpec_Attr_FaceCull;

		pDlg->Create( i3TDKSpec_Attr_FaceCull::IDD, this);

		AddSpecDialog( i3FaceCullModeAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FogColorAttr
		i3TDKSpec_Attr_FogColor * pDlg = new i3TDKSpec_Attr_FogColor;

		pDlg->Create( i3TDKSpec_Attr_FogColor::IDD, this);

		AddSpecDialog( i3FogColorAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FogDensityAttr
		i3TDKSpec_Attr_FogDensity * pDlg = new i3TDKSpec_Attr_FogDensity;

		pDlg->Create( i3TDKSpec_Attr_FogDensity::IDD, this);

		AddSpecDialog( i3FogDensityAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FogModeAttr
		i3TDKSpec_Attr_FogMode * pDlg = new i3TDKSpec_Attr_FogMode;

		pDlg->Create( i3TDKSpec_Attr_FogMode::IDD, this);

		AddSpecDialog( i3FogModeAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FogNearFarAttr
		i3TDKSpec_Attr_FogNearFar * pDlg = new i3TDKSpec_Attr_FogNearFar;

		pDlg->Create( i3TDKSpec_Attr_FogNearFar::IDD, this);

		AddSpecDialog( i3FogNearFarAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3FresnelAttr
		i3TDKSpec_Attr_Fresnel * pDlg = new i3TDKSpec_Attr_Fresnel;

		pDlg->Create( i3TDKSpec_Attr_Fresnel::IDD, this);

		AddSpecDialog( i3FresnelAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3GeometryAttr
		i3TDKSpec_Attr_Geometry * pDlg = new i3TDKSpec_Attr_Geometry;

		pDlg->Create( i3TDKSpec_Attr_Geometry::IDD, this);

		AddSpecDialog( i3GeometryAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3TextureBindAttr
		i3TDKSpec_Attr_TexBind * pDlg = new i3TDKSpec_Attr_TexBind;

		pDlg->Create( i3TDKSpec_Attr_TexBind::IDD, this);

		AddSpecDialog( i3TextureBindAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3TextureMatrixAttr
		i3TDKSpec_Attr_TexMat * pDlg = new i3TDKSpec_Attr_TexMat;
	
		pDlg->Create( i3TDKSpec_Attr_TexMat::IDD, this);

		AddSpecDialog( i3TextureMatrixAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3TextureWrapAttr
		i3TDKSpec_Attr_TexWrap * pDlg = new i3TDKSpec_Attr_TexWrap;

		pDlg->Create( i3TDKSpec_Attr_TexWrap::IDD, this);

		AddSpecDialog( i3TextureWrapAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3MaterialAttr
		i3TDKSpec_Attr_Material * pDlg = new i3TDKSpec_Attr_Material;

		pDlg->Create( i3TDKSpec_Attr_Material::IDD, this);

		AddSpecDialog( i3MaterialAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3RenderModeAttr
		i3TDKSpec_Attr_RenderMode * pDlg = new i3TDKSpec_Attr_RenderMode;

		pDlg->Create( i3TDKSpec_Attr_RenderMode::IDD, this);

		AddSpecDialog( i3RenderModeAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{	// i3ShadeModeAttr
		i3TDKSpec_Attr_ShadeMode * pDlg = new i3TDKSpec_Attr_ShadeMode;

		pDlg->Create( i3TDKSpec_Attr_ShadeMode::IDD, this);

		AddSpecDialog( i3ShadeModeAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}

	{
		i3TDKSpec_Attr_Shader * pDlg = new i3TDKSpec_Attr_Shader;

		pDlg->Create( i3TDKSpec_Attr_Shader::IDD, this);

		AddSpecDialog( i3ShaderAttr::static_meta(), pDlg, I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, true);
	}


	i3TDK::RestoreResInstance();
}

BEGIN_MESSAGE_MAP(i3TDKSpecDialogContainer, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// i3TDKSpecDialogContainer message handlers
void i3TDKSpecDialogContainer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	m_Width = cx;
	m_Height = cy;

	if( ::IsWindow( m_FoldCtrl.m_hWnd))
	{
		m_FoldCtrl.SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void i3TDKSpecDialogContainer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & (I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC))
	{
		PopupDialog( pInfo->m_pObject);
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		PopupDialog(nullptr);
// 뭔가 다르게 하고 싶었던 모냥인데, 다른게 없으므로 위 한줄 추가하고 아랫쪽 구현은 모두 주석처리. (16.08.01.수빈)
//		if( pInfo->m_pObject == m_pObj)
//		{
//			PopupDialog( nullptr);
//		}
//		else
//		{
//			PopupDialog( nullptr);
//		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		PopupDialog( nullptr);
	}
}

LRESULT i3TDKSpecDialogContainer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL i3TDKSpecDialogContainer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_FoldCtrl.Create( _T("Specification"), WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	i3TDK::RegisterUpdate( m_hWnd, nullptr, I3_TDK_UPDATE_SCENE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL i3TDKSpecDialogContainer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//UINT id = LOWORD( wParam);
	//UINT code = HIWORD( wParam);
	//HWND hwndCtrl = (HWND) lParam;

	return i3TDKDialogBase::OnCommand(wParam, lParam);
}

void i3TDKSpecDialogContainer::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
