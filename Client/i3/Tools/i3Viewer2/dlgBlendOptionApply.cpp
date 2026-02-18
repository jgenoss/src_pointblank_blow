#include "stdafx.h"
#include "dlgBlendOptionApply.h"
#include "i3Base/string/ext/atoi.h"


BEGIN_MESSAGE_MAP(dlgBlendOptionApply, CDialog)
END_MESSAGE_MAP()

enum BLEND_TYPE
{
	BLEND_OPTION_NONE,
	BLEND_OPTION_ALPHA_TEST,
	BLEND_OPTION_MODULATE,
	BLEND_OPTION_ADD,
};

dlgBlendOptionApply::dlgBlendOptionApply() : CDialog(dlgBlendOptionApply::IDD)
{
}

dlgBlendOptionApply::~dlgBlendOptionApply()
{
	
}

void dlgBlendOptionApply::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_BLENDTYPE,	m_pBlendTypeCtrl);
	DDX_Control(pDX, ID_BTN_TWOSIDE,		m_pTwoSideCtrl);
	DDX_Control(pDX, ID_BTN_DEPTHWRITE,		m_pDepthWriteCtrl);
	DDX_Control(pDX, IDC_EDIT_REFVALUE,		m_pRefValCtrl);
}  

void dlgBlendOptionApply::OnOK()
{
	Apply();

	CDialog::OnOK();
}

BOOL dlgBlendOptionApply::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pBlendTypeCtrl.SetCurSel(0);
	m_pRefValCtrl.SetWindowText("64");

	return TRUE;
}

void dlgBlendOptionApply::Apply()
{
	I3ASSERT(m_pRoot != NULL);

	char conv[MAX_PATH];
	UINT8 val;
	BLEND_TYPE type = (BLEND_TYPE)m_pBlendTypeCtrl.GetCurSel();

	switch(type)
	{
	case BLEND_OPTION_NONE:
		{
			SetNoBlend();
		}
		break;
	case BLEND_OPTION_ALPHA_TEST:
		{
			m_pRefValCtrl.GetWindowText(conv, _countof(conv) - 1);
			val = (UINT8)i3::atoi(conv); 

			SetAlphaTest(val);
		}
		break;
	case BLEND_OPTION_MODULATE:
		{
			m_pRefValCtrl.GetWindowText(conv, _countof(conv) - 1);
			val = (UINT8)i3::atoi(conv); 

			SetBlendModulate(val);
		}
		break;
	case BLEND_OPTION_ADD:
		{
			m_pRefValCtrl.GetWindowText(conv, _countof(conv) - 1);
			val = (UINT8)i3::atoi(conv); 

			SetBlendAdd(val);
		}
		break;
	default:
		break;
	}

	BOOL enabled = (m_pTwoSideCtrl.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	SetFaceCull(enabled);

	enabled = (m_pDepthWriteCtrl.GetCheck() == BST_CHECKED) ? TRUE : FALSE;
	SetZWrite(enabled);
}

void dlgBlendOptionApply::Execute(i3Node * pNode)
{
	I3ASSERT(pNode != NULL);

	m_pRoot = pNode;

	DoModal();
}

void dlgBlendOptionApply::SetNoBlend()
{
	i3::vector<i3RenderAttr *> blendEnableList;
	i3::vector<i3RenderAttr *> alphaTestList;

	i3Scene::FindAttrByExactType(m_pRoot, i3BlendEnableAttr::static_meta(), blendEnableList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaTestEnableAttr::static_meta(), alphaTestList, true);

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendEnableAttr * pBlendEnableAttr = (i3BlendEnableAttr *)blendEnableList[i];
		pBlendEnableAttr->Set(FALSE);
	}

	for(INT32 i = 0; i < (INT32)alphaTestList.size(); i++)
	{
		i3AlphaTestEnableAttr * pAlphaAttr = (i3AlphaTestEnableAttr *)alphaTestList[i];
		pAlphaAttr->Set(FALSE);
	}
}

void dlgBlendOptionApply::SetAlphaTest(UINT8 val)
{
	i3::vector<i3RenderAttr *> blendEnableList;
	i3::vector<i3RenderAttr *> alphaTestList;
	i3::vector<i3RenderAttr *> alphaFuncList;

	i3Scene::FindAttrByExactType(m_pRoot, i3BlendEnableAttr::static_meta(), blendEnableList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaTestEnableAttr::static_meta(), alphaTestList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaFuncAttr::static_meta(), alphaFuncList, true);

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendEnableAttr * pBlendEnableAttr = (i3BlendEnableAttr *)blendEnableList[i];
		pBlendEnableAttr->Set(FALSE);
	}

	for(INT32 i = 0; i < (INT32)alphaTestList.size(); i++)
	{
		i3AlphaTestEnableAttr * pAlphaAttr = (i3AlphaTestEnableAttr *)alphaTestList[i];
		pAlphaAttr->Set(TRUE);
	}

	for(INT32 i = 0; i < (INT32)alphaFuncList.size(); i++)
	{
		i3AlphaFuncAttr * pFuncAttr = (i3AlphaFuncAttr *)alphaFuncList[i];
		pFuncAttr->SetFunction(I3G_COMP_GREATER);
		pFuncAttr->SetRefValue(val);
	}
}

void dlgBlendOptionApply::SetBlendModulate(UINT8 val)
{
	i3::vector<i3RenderAttr *> blendEnableList;
	i3::vector<i3RenderAttr *> blendModeList;
	i3::vector<i3RenderAttr *> alphaTestList;
	i3::vector<i3RenderAttr *> alphaFuncList;

	i3Scene::FindAttrByExactType(m_pRoot, i3BlendEnableAttr::static_meta(), blendEnableList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3BlendModeAttr::static_meta(), blendModeList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaTestEnableAttr::static_meta(), alphaTestList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaFuncAttr::static_meta(), alphaFuncList, true);

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendEnableAttr * pBlendEnableAttr = (i3BlendEnableAttr *)blendEnableList[i];
		pBlendEnableAttr->Set(TRUE);
	}

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendModeAttr * pBlendModeAttr = (i3BlendModeAttr *)blendModeList[i];
		pBlendModeAttr->SetSource(I3G_BLEND_SRCALPHA);
		pBlendModeAttr->SetDestination(I3G_BLEND_INVSRCALPHA);
	}

	for(INT32 i = 0; i < (INT32)alphaTestList.size(); i++)
	{
		i3AlphaTestEnableAttr * pAlphaAttr = (i3AlphaTestEnableAttr *)alphaTestList[i];
		pAlphaAttr->Set(TRUE);
	}

	for(INT32 i = 0; i < (INT32)alphaFuncList.size(); i++)
	{
		i3AlphaFuncAttr * pFuncAttr = (i3AlphaFuncAttr *)alphaFuncList[i];
		pFuncAttr->SetFunction(I3G_COMP_GREATER);
		pFuncAttr->SetRefValue(val);
	}
}

void dlgBlendOptionApply::SetBlendAdd(UINT8 val)
{
	i3::vector<i3RenderAttr *> blendEnableList;
	i3::vector<i3RenderAttr *> blendModeList;
	i3::vector<i3RenderAttr *> alphaTestList;
	i3::vector<i3RenderAttr *> alphaFuncList;

	i3Scene::FindAttrByExactType(m_pRoot, i3BlendEnableAttr::static_meta(), blendEnableList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3BlendModeAttr::static_meta(), blendModeList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaTestEnableAttr::static_meta(), alphaTestList, true);
	i3Scene::FindAttrByExactType(m_pRoot, i3AlphaFuncAttr::static_meta(), alphaFuncList, true);

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendEnableAttr * pBlendEnableAttr = (i3BlendEnableAttr *)blendEnableList[i];
		pBlendEnableAttr->Set(TRUE);
	}

	for(INT32 i = 0; i < (INT32)blendEnableList.size(); i++)
	{
		i3BlendModeAttr * pBlendModeAttr = (i3BlendModeAttr *)blendModeList[i];
		pBlendModeAttr->SetSource(I3G_BLEND_SRCALPHA);
		pBlendModeAttr->SetDestination(I3G_BLEND_ONE);
	}

	for(INT32 i = 0; i < (INT32)alphaTestList.size(); i++)
	{
		i3AlphaTestEnableAttr * pAlphaAttr = (i3AlphaTestEnableAttr *)alphaTestList[i];
		pAlphaAttr->Set(TRUE);
	}

	for(INT32 i = 0; i < (INT32)alphaFuncList.size(); i++)
	{
		i3AlphaFuncAttr * pFuncAttr = (i3AlphaFuncAttr *)alphaFuncList[i];
		pFuncAttr->SetFunction(I3G_COMP_GREATER);
		pFuncAttr->SetRefValue(val);
	}
}

void dlgBlendOptionApply::SetFaceCull(BOOL isEnableFaceCull)
{
	i3::vector<i3RenderAttr *> twoSideList;
	i3Scene::FindAttrByExactType(m_pRoot, i3FaceCullModeAttr::static_meta(), twoSideList, true);

	for(INT32 i = 0; i < (INT32)twoSideList.size(); i++)
	{
		i3FaceCullModeAttr * pAttr = (i3FaceCullModeAttr *)twoSideList[i];
		if(isEnableFaceCull)
		{
			pAttr->Set(I3G_FACE_CULL_NONE);
		}
		else
		{
			pAttr->Set(I3G_FACE_CULL_BACK);
		}
	}
}

void dlgBlendOptionApply::SetZWrite(BOOL isEnableZWrite)
{
	i3::vector<i3RenderAttr *> zWriteList;
	i3Scene::FindAttrByExactType(m_pRoot, i3ZWriteEnableAttr::static_meta(), zWriteList, true);

	for(INT32 i = 0; i < (INT32)zWriteList.size(); i++)
	{
		i3ZWriteEnableAttr * pAttr = (i3ZWriteEnableAttr *) zWriteList[i];
		pAttr->Set(isEnableZWrite);
	}
}