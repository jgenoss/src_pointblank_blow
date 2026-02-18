// DlgAddAIStateToChilds.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgAddAIStateToChilds.h"
#include ".\dlgaddaistatetochilds.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/substr.h"

// CDlgAddAIStateToChilds dialog

IMPLEMENT_DYNAMIC(CDlgAddAIStateToChilds, CDialog)
CDlgAddAIStateToChilds::CDlgAddAIStateToChilds(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddAIStateToChilds::IDD, pParent)
{
	m_pObj = NULL;
}

CDlgAddAIStateToChilds::~CDlgAddAIStateToChilds()
{
}

void CDlgAddAIStateToChilds::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_ADDCHILDS_NAME, m_Ed_AIStateNameCtrl);
	DDX_Control(pDX, IDC_COMBO_ADDCHILDS_ANIMNAME, m_Cmb_AnimPathCtrl);
	DDX_Control(pDX, IDC_ED_ADDCHILDS_BLENDTIME, m_Ed_BlendTimeCtrl);
	DDX_Control(pDX, IDC_EDIT_ADDCHILDS_FILTER, m_Ed_AnimFilterCtrl);
	DDX_Control(pDX, IDC_CHK_ADDCHILDS_LOOP, m_Chk_AnimLoopCtrl);
	DDX_Control(pDX, IDC_RADIO_ADDCHILDS_POS_AUTO, m_Radio_PosAuto);
	DDX_Control(pDX, IDC_CHK_USE_OWN_PATH, m_Chk_UseOwnPath);
	DDX_Control(pDX, IDC_ED_ADDCHILD_ROOTNODE, m_Ed_BasePathNodeName);
}


BEGIN_MESSAGE_MAP(CDlgAddAIStateToChilds, CDialog)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDCHILDS_FILTER, OnEnKillfocusEditAddchildsFilter)
	ON_BN_CLICKED(IDC_RADIO_ADDCHILDS_POS_AUTO, &CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosAuto)
	ON_BN_CLICKED(IDC_RADIO_ADDCHILDS_POS_BOTTOM, &CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosBottom)
	ON_BN_CLICKED(IDC_RADIO_ADDCHILDS_POS_RIGHT, &CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosRight)
	ON_BN_CLICKED(IDC_CHK_USE_OWN_PATH, &CDlgAddAIStateToChilds::OnBnClickedChkUseOwnPath)
END_MESSAGE_MAP()


// CDlgAddAIStateToChilds message handlers
BOOL CDlgAddAIStateToChilds::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Ed_AnimFilterCtrl.SetWindowText( "");
	m_Cmb_AnimPathCtrl.SetCurSel( -1);
	m_Ed_BlendTimeCtrl.SetWindowText( "0.0");

	UpdateAnimComboList();

	// 노드를 추가할 위치.
	{
		m_Radio_PosAuto.SetCheck(TRUE);
		m_eAddPos = ADD_POS_AUTO;
	}

	m_Ed_BasePathNodeName.SetWindowText("Input Base AI State Name");

	return TRUE;
}

void CDlgAddAIStateToChilds::AddAIState( i3GameNode * pNode)
{
	char conv[ 64];
//	I3ASSERT( pNode != NULL);
	I3ASSERT( i3::same_of<i3AIContext*>(pNode)); //->IsExactTypeOf( i3AIContext::static_meta()));

	INT32 idx = g_pChara->FindAIByAIContext( (i3AIContext*) pNode);
	AI * pAI = g_pChara->getAI( idx);
	I3ASSERT( pAI != NULL);
	
	i3AIState * pState = i3AIState::new_object_ref();
	I3ASSERT( pState != NULL);

	m_Ed_AIStateNameCtrl.GetWindowText( conv, sizeof(conv) -1);
	if( conv[ 0] != 0)
	{
		pState->SetName( conv);
	}

	i3Animation * pAnim = FindSelectAIState(pAI->getDiagram());
	if(pAnim != NULL)
		pState->SetAnim( pAnim);

	if( m_Chk_AnimLoopCtrl.GetCheck() == BST_CHECKED)
	{
		pState->addStyle( I3_AISTATE_STYLE_ANIM_LOOP );
	}

	m_Ed_BlendTimeCtrl.GetWindowText( conv, sizeof(conv) -1);
	if( conv[ 0] != 0)
	{
		REAL32 rBlendTime = (REAL32) atof( conv);
		pState->setBlendTime( rBlendTime);
	}

	pAI->getAI()->AddAIState( pState);
	I3ASSERT( pAI->getDiagram() != NULL);

	GICShapeAI * pNewShape = /*(GICShapeAI *)*/	GICShapeAI::new_object_ref();	// CREATEINSTANCE( GICShapeAI::static_meta());
	pAI->getDiagram()->AddShape( NULL, pNewShape);
	pNewShape->setAIState( pState);
	pNewShape->SetName(pState->GetName());
	
	RecalcPos( pAI->getDiagram(), pNewShape);
}

void CDlgAddAIStateToChilds::RecalcPos( GICDiagramAI * pDiagram, i3GICShape * pShape)
{
	I3ASSERT( pDiagram != NULL);
	I3ASSERT( pShape != NULL);

	REAL32 rMaxX = 0.f;
	REAL32 rMinX = 10000.f;

	REAL32 rShapeWidth = 10000.f;
	REAL32 rShapeHeight = 90.0f;

	// 전체적인 사이즈 계산
	for( INT32 i = 0; i < pDiagram->getShapeCount(); i++)
	{
		i3GICShape * pDestShape = pDiagram->getShape( i);

		if( pDestShape == pShape)
		{
			if(pDiagram->getShapeCount() <= 1 )
			{
				rMaxX = pDestShape->getPosX() + 300.0f;
				rShapeWidth = 300.0f;
			}

			continue;
		}

		if( rMaxX < pDestShape->getPosX() + pDestShape->getWidth())
			rMaxX = pDestShape->getPosX() + pDestShape->getWidth();

		if( rMinX > pDestShape->getPosX())
			rMinX = pDestShape->getPosX();

		if( rShapeWidth > pDestShape->getWidth() )
			rShapeWidth = pDestShape->getWidth();

		if( rShapeHeight < pDestShape->getHeight() )
			rShapeHeight = pDestShape->getHeight();
	}

	pShape->setWidth( rShapeWidth);
	pShape->setHeight( rShapeHeight);

	switch(m_eAddPos)
	{
	case ADD_POS_AUTO:
		RecalcPosToAuto(pDiagram, pShape, rMaxX, rMinX);	
		break;
	case ADD_POS_BOTTOM:
		RecalcPosToBottomOfLastNode(pDiagram, pShape);	
		break;
	case ADD_POS_RIGHT:
		ReCalcPosToRightOfLastNode(pDiagram, pShape);	
		break;
	default:
		RecalcPosToAuto(pDiagram, pShape, rMaxX, rMinX);	
		break;
	}
}

void CDlgAddAIStateToChilds::RecalcPosToAuto(GICDiagramAI * pDiagram, i3GICShape * pShape, REAL32 rMaxX, REAL32 rMinX)
{
	const REAL32 rPadding = 25.0f;

	REAL32 rShapePosX = pShape->getPosX();
	REAL32 rShapePosY = pShape->getPosY();

	for( INT32 i = 0; i < pDiagram->getShapeCount(); i++)
	{
		i3GICShape * pDestShape = pDiagram->getShape( i);
		
		if( pDestShape == pShape )
			continue;

		bool bShiftX = false;
		bool bShiftY = false;

		if( pDestShape->getPosX() + pDestShape->getWidth() > rShapePosX )
		{	
			if( pDestShape->getPosX() < rShapePosX + pShape->getWidth() )
			{	
				bShiftX = true;
			}
		}

		if( pDestShape->getPosY() + pDestShape->getHeight() > rShapePosY )
		{
			if( pDestShape->getPosY() < rShapePosY + pShape->getHeight() )
			{	
				bShiftY = true;
			}
		}

		if(bShiftX && bShiftY)
		{
			if( rMaxX < pDestShape->getPosX() + pDestShape->getWidth() + rPadding + pShape->getWidth() )
			{
				rShapePosX = rMinX;
				rShapePosY = pDestShape->getPosY() + pDestShape->getHeight() + rPadding;
			}
			else
			{
				rShapePosX = pDestShape->getPosX() + pDestShape->getWidth() + rPadding;
			}

			i = -1;
		}
	}

	pShape->setPosX( rShapePosX );
	pShape->setPosY( rShapePosY );
}

void CDlgAddAIStateToChilds::RecalcPosToBottomOfLastNode(GICDiagramAI * pDiagram, i3GICShape * pShape)
{
	REAL32 rPosX = 10000.0f;
	REAL32 rPosY = 0.0f;

	const REAL32 rPadding = 25.0f;

	for( INT32 i = 0; i < pDiagram->getShapeCount(); i++)
	{
		i3GICShape * pDestShape = pDiagram->getShape( i);

		if( pDestShape == pShape )
		{
			if(pDiagram->getShapeCount() <= 1 )
				rPosX = 0.0f;

			continue;
		}

		if( rPosX >= pDestShape->getPosX() )
			rPosX = pDestShape->getPosX();

		if(rPosY <= pDestShape->getPosY() + pDestShape->getHeight() )
			rPosY = pDestShape->getPosY() + pDestShape->getHeight() + rPadding; 

	}

	pShape->setPosX( rPosX);
	pShape->setPosY( rPosY);
}

void CDlgAddAIStateToChilds::ReCalcPosToRightOfLastNode(GICDiagramAI * pDiagram, i3GICShape * pShape)
{
	REAL32 rPosX = 10000.0f;
	REAL32 rPosY = 0.0f;

	const REAL32 rPadding = 25.0f;

	for( INT32 i = 0; i < pDiagram->getShapeCount(); i++)
	{
		i3GICShape * pDestShape = pDiagram->getShape( i);

		if( pDestShape == pShape )
		{
			if(pDiagram->getShapeCount() <= 1 )
				rPosX = 0.0f;

			continue;
		}

		// 제일 하단 노드의 우측으로 위치
		if(rPosY <= pDestShape->getPosY())
		{
			rPosX = pDestShape->getPosX() + pDestShape->getWidth() + rPadding;
			rPosY = pDestShape->getPosY();			
		}
	}

	pShape->setPosX( rPosX);
	pShape->setPosY( rPosY);
}

i3Animation * CDlgAddAIStateToChilds::FindSelectAIState(GICDiagramAI * pDiagram)
{
	i3Animation * pAnim = NULL;

	INT32 iAnimIdx = m_Cmb_AnimPathCtrl.GetCurSel();

	if( iAnimIdx != -1)
	{
		pAnim = (i3Animation*) m_Cmb_AnimPathCtrl.GetItemData( iAnimIdx);

		if(m_Chk_UseOwnPath.GetCheck() == BST_UNCHECKED)
			return pAnim;

		// 무기별 경로에서 찾는 경우 
		char szBaseAIState[MAX_PATH] = "";
		// Base Path를 제공해줄 AI State가 필요.
		m_Ed_BasePathNodeName.GetWindowText( szBaseAIState, sizeof(szBaseAIState) - 1);	

		if(i3::generic_string_size(szBaseAIState) <= 0)
			return pAnim;

		char szBasePath[MAX_PATH] = "";
		char szFileName[MAX_PATH] = "";
		char szFullPath[MAX_PATH] = "";

		// 적용할 Animation 파일명을 구합니다.
	//	i3String::SplitFileName(pAnim->GetName(), szFileName, TRUE);
		i3::extract_filename(pAnim->GetName(), szFileName);

		for(INT32 i = 0 ; i < pDiagram->getShapeCount(); ++i)
		{
			GICShapeAI *pShape = i3::same_cast<GICShapeAI*>(pDiagram->getShape(i));

			// Base Path를 구한 후, Full Path를 구합니다.
			if(i3::generic_is_iequal(pShape->getAIState()->GetName(), szBaseAIState) )
			{
				if(pShape->getAIState()->getAnim() == NULL)
					return NULL;

			//	i3String::SplitPath(pShape->getAIState()->getAnim()->GetName(), szBasePath);
				i3::extract_directoryname(pShape->getAIState()->getAnim()->GetName(), szBasePath);
				i3::snprintf(szFullPath, sizeof(szFullPath), "%s/%s", szBasePath, szFileName);
				break;
			}
		}

		if(i3::generic_string_size(szFullPath) <= 0)
			return pAnim;

		for(INT32 i = 0 ; i <g_pChara->getAnimCount(); ++i)
		{
			i3Animation * pFindAnim = g_pChara->getAnim( i);

			I3ASSERT( pFindAnim->hasName());

			char szFindKey[MAX_PATH] = {0};
			i3::safe_string_copy(szFindKey, szFullPath, MAX_PATH);

			// FindKey를 사용하여, 문자열이 존재하는지 확인.
			while(i3::icontain_string(pFindAnim->GetName(), szFindKey) == -1)
			{
				INT32 i32Pos = i3::contain_char(szFindKey, '/');

				if(i32Pos < 0)	
					break;
				else
					i3::substr(szFindKey, szFindKey, i32Pos + 1);
			}

			if(i3::generic_is_iequal(szFullPath, szFindKey) )
			{
				return pFindAnim;
			}
			else if(i3::generic_string_size(szFindKey) > 0)
			{
				// Find Key가 너무 하위단으로 내려간 경우에는 적용하지 않는다.
				// Select Anim : Root/Folder1/FileName1.i3a, Find Anim : Root/Folder2/FileName1.i3a 인 경우,
				// Find Key가 FileName1.i3a로 남았을 때, 루프간 Root/Folder3/FileName1.i3a 이 선택되는 경우를 방지함.
				if(i3::icontain_string(pAnim->GetName(), szFindKey) == -1)
				{
					// 경로 구분을 위해 '/' 추가하여 다시한번 검사.
					// TempFolder/Animation.i3a , Folder/Animation.i3a를 확실히 구분하기 위함.
				//	i3String::InsertStr(szFindKey, "/", 0, 1);		
					i3::insert_string(szFindKey, 0, "/", 1);

					if(i3::icontain_string(pFindAnim->GetName(), szFindKey) >= 0)
					{
						return pFindAnim;
					}
				}
			}
		}
	}

	return pAnim;
}

void CDlgAddAIStateToChilds::ProcessAddAIState( void)
{
//	I3ASSERT( m_pObj != NULL);
	I3ASSERT( i3::kind_of<AI*>(m_pObj)); //->IsTypeOf( AI::static_meta()) );
	
	i3AIContext * pAICtx = ((AI*) m_pObj)->getAIContext();
	I3ASSERT( pAICtx != NULL);

	Rec_FindLastChild( pAICtx);
}

void CDlgAddAIStateToChilds::Rec_FindLastChild( i3GameNode * pNode)
{
	i3GameNode * pChild = pNode->getFirstChild();
	if( pChild == NULL)
	{
		// 마지막 노드이다.
		// 이 노드에 AIState를 추가
		AddAIState( pNode);
	}

	while( pChild != NULL)
	{
		Rec_FindLastChild( pChild);

		pChild = pChild->getNext();
	}
}


void CDlgAddAIStateToChilds::UpdateAnimComboList()
{
	char szFilter[MAX_PATH];
	char conv[MAX_PATH];
//	char	conv2[MAX_PATH];
	INT32 i, idx;

	m_Ed_AnimFilterCtrl.GetWindowText( szFilter, sizeof(szFilter) - 1);


	// Chara가 소유하고 있는 모든 Animation들을 등록..
	{
		i3Animation * pAnim;

		m_Cmb_AnimPathCtrl.ResetContent();

		for( i = 0; i < g_pChara->getAnimCount(); i++)
		{
			pAnim = g_pChara->getAnim( i);
			I3ASSERT( pAnim->hasName());

			if( szFilter[0] != 0)
			{
				if( i3::contain_string( pAnim->GetName(), szFilter) == -1)
					continue;
			}

			{
				i3::stack_string dir, title;
				// 최하단 디렉토리명과 파일타이틀만 얻어온다..
				i3::extract_directoryname_filename(pAnim->GetName(), dir, title);
				i3::extract_filename(dir, dir);	
				i3::remove_ext(title);
				i3::sprintf(conv, "%s\\%s", dir, title );

				/*
				char * pName = i3String::SplitFileName( pAnim->GetName(), conv, FALSE);
				if( pName != NULL )
				{
					INT32 nLength = i3::generic_string_size( pName );
					if( nLength > 0 )
					{
						char temp[MAX_PATH] = {0,};
					//	i3String::InsertStr( temp, (char*)pAnim->GetName(), 0, i3::generic_string_size(pAnim->GetName()) - (nLength+1) );
					//	i3::generic_string_copy(temp, pAnim->GetName());
						i3::generic_string_ncopy(temp, (char*)pAnim->GetName(), i3::generic_string_size(pAnim->GetName()) - (nLength+1) );
						i3String::SplitFileName( temp, conv2, FALSE );
						i3::safe_string_copy( temp, conv, MAX_PATH );
						sprintf( conv, "%s\\%s", conv2, temp );
					}
				}
				*/
			}

			idx = m_Cmb_AnimPathCtrl.AddString( conv);
			m_Cmb_AnimPathCtrl.SetItemData( idx, (DWORD_PTR) pAnim);
		}
	}
}

void CDlgAddAIStateToChilds::OnEnKillfocusEditAddchildsFilter()
{
	// TODO: Add your control notification handler code here
	UpdateAnimComboList();
}

void CDlgAddAIStateToChilds::OnOK()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();

	ProcessAddAIState();
}

void CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosAuto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_eAddPos = ADD_POS_AUTO;
}

void CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosBottom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_eAddPos = ADD_POS_BOTTOM;
}

void CDlgAddAIStateToChilds::OnBnClickedRadioAddchildsPosRight()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_eAddPos = ADD_POS_RIGHT;
}

void CDlgAddAIStateToChilds::OnBnClickedChkUseOwnPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_Chk_UseOwnPath.GetCheck() == BST_CHECKED)
	{
		m_Ed_BasePathNodeName.SetWindowText("");
		m_Ed_BasePathNodeName.EnableWindow(TRUE);
	}
	else
	{
		m_Ed_BasePathNodeName.EnableWindow(FALSE);
		m_Ed_BasePathNodeName.SetWindowText("Input Base AI State Name");
	}

}
