// PanelSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "PanelSpec.h"

#include "RSCDialog.h"
#include "i3Base/string/ext/generic_string_cat.h"
// PanelSpec dialog
static char strTemp[512];

//	아이템 클래스
i3ClassMeta * g_PropertyItemClassMeta[ PROPERTY_ITEM_MAX] =			
{
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_FRAMEWORK_NAME = 0,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_FRAMEWORK_STARTSTAGE,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_FRAMEWORK_STARTSTAGE,
													
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_NAME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_CLASSNAME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_FADEINCOLOR,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_FADEINTIME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_FADEOUTCOLOR,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_FADEOUTTIME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_NEXTSTAGE,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_JUMPTYPE,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_STAGE_RSC,
													
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_LAYER_NAME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_LAYER_STYLE_2D,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_LAYER_STYLE_TRANS,

	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_INSTANCE_CLASSNAME,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_INSTANCE_COUNT,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_INSTANCE_UPDATEINTERVAL,
	i3TDKPropertySubItemStatic::static_meta(),		//	PROPERTY_STATIC_INSTANCE_FIXEDCOUNT,
													
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_FRAMEWORK_NAME,
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_FRAMEWORK_STARTSTAGE	
	i3TDKPropertySubItemUserCtrl::static_meta(),	//	PROPERTY_EDIT_STAGE_RSC,
													
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_STAGE_NAME,	
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_STAGE_CLASSNAME,	
	i3TDKPropertySubItemColor::static_meta(),		//	PROPERTY_EDIT_STAGE_FADEINCOLOR,	
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_STAGE_FADEINTIME,	
	i3TDKPropertySubItemColor::static_meta(),		//	PROPERTY_EDIT_STAGE_FADEOUTCOLOR,	
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_STAGE_FADEOUTTIME,	
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_STAGE_NEXTSTAGE,	
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_STAGE_JUMPTYPE,	
	i3TDKPropertySubItemUserCtrl::static_meta(),	//	PROPERTY_EDIT_STAGE_RSC,

	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_LAYER_NAME,	
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_LAYER_STYLE_2D,		
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_LAYER_STYLE_TRANS,		

	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_INSTANCE_CLASSNAME,	
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_INSTANCE_COUNT,	
	i3TDKPropertySubItemEdit::static_meta(),		//	PROPERTY_EDIT_INSTANCE_UPDATEINTERVAL,	
	i3TDKPropertySubItemComboBox::static_meta(),	//	PROPERTY_EDIT_INSTANCE_FIXEDCOUNT,		
};												

//	아이템 라벨 텍스트
char * g_PropertyItemLabel[ PROPERTY_ITEM_MAX] =			
{
	{ "Name"			},
	{ "Start Stage"		},
	{ "CommonRes RSC"	},

	{ "Name"			},
	{ "Class Name"		},
	{ "FadeIn Color"	},
	{ "FadeIn Time"		},
	{ "FadeOut Color"	},
	{ "FadeOut Time"	},
	{ "Next Stage"		},
	{ "Jump Type"		},
	{ "RSC File"		},

	{ "Name"			},
	{ "Style 2D"			},
	{ "Style Transparent"	},

	{ "Class Name"		},
	{ "Instance Count"	},
	{ "Update Interval"	},
	{ "Fixed Count" },

	{ ""				},
	{ ""				},

	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},

	{ ""				},
	{ ""				},
	{ ""				},

	{ ""				},
	{ ""				},
	{ ""				},
	{ ""				},
};

IMPLEMENT_DYNAMIC(PanelSpec, i3TDKDialogBase)
PanelSpec::PanelSpec(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(PanelSpec::IDD, pParent)
{
}

PanelSpec::~PanelSpec()
{
}

void PanelSpec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void PanelSpec::SelectPage( i3ElementBase * pData)
{
//	INT32 i = 0;
	i3TDKPropertySubItem * pSubItem = NULL;

	__CHECKW_RT( g_pGameFramework == NULL, "Warning! Invalid Data on select");

	m_ListCtrl.ClearItem();

	if( pData == NULL)	return;		//	NULL일 경우 빈 페이지다.
	
	//	현재 선택된 아이템 데이터
	i3ElementBase * pBaseData = (i3ElementBase *) theApp.GetMainFrm()->m_PanelGame.GetSelectedItemData();
	__CHECKW_RT( pBaseData == NULL, "Waring! Invalid Data.");
		
	if( i3::kind_of<i3Framework*>(pData)) //->IsTypeOf( i3Framework::static_meta()))		//	GameFramework Property
	{
		m_ListCtrl.NewSubItem( 3);

		//	Subject name
		INT32 startIndex = PROPERTY_STATIC_FRAMEWORK_NAME;
		INT32 endIndex = PROPERTY_STATIC_FRAMEWORK_CMNRES;
		for( INT32 i = startIndex; i <= endIndex; i++)
		{			
			pSubItem = m_pPropertyItem[ i];
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);				
		}

		//	Name	
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_FRAMEWORK_NAME];
			pSubItem->SetID( PROPERTY_VALUE_FRAMEWORK_NAME);
			pSubItem->SetUserData( (void *) g_pGameFramework);
			pSubItem->SetLabel( g_pGameFramework->GetName());			

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	StartStage
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_FRAMEWORK_STARTSTAGE];
			pSubItem->SetID( PROPERTY_VALUE_FRAMEWORK_STARTSTAGE);
			pSubItem->SetUserData( (void *) g_pGameFramework);
	
			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			
			i3StageInfo * pStartStage = g_pGameFramework->GetStartStageInfo();
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement("(null)");		//	Element(0) = NULL

			//	ComboBox에 Element로 StageInfo의 Name 추가
			for( INT32 i = 0; i < g_pGameFramework->getStageInfoCount(); i++)
			{
				i3StageInfo * pStageInfo = g_pGameFramework->getStageInfo( i);
				pComboBoxItem->AddElement( (char *) pStageInfo->GetName());

				//	현재 선택된 Element
				if( pStartStage == pStageInfo)
					pComboBoxItem->SelectElement( i + 1);
			}			
			
			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			char * pszString = (char *) pComboBoxItem->GetElement( nSelect);
			pSubItem->SetLabel( pszString);

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	RSC File
		{
			i3::vector<i3::rc_string>& CmnResList = g_pGameFramework->getCmnResList();

			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_FRAMEWORK_CMNRES];
			pSubItem->SetID( PROPERTY_VALUE_FRAMEWORK_CMNRES);
			pSubItem->SetUserData( &CmnResList);

			i3TDKPropertySubItemUserCtrl * pRSCItem = (i3TDKPropertySubItemUserCtrl *) pSubItem;	
			
			INT32 count = CmnResList.size(); 
			if( count > 0)
				pRSCItem->SetLabel( CmnResList[count - 1].c_str());	//	하나 이상일 경우 마지막 RSC가 Label에 보인다.
			else
				pRSCItem->SetLabel( "");
			
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);

			//	Set ToolTip
			{
//				INT32 nToolTipSize = GetStringListBufferSize(CmnResList);	// pCmnResList->GetBufferSize();

				memset( strTemp, 0, sizeof( strTemp));
				for (size_t i = 0; i < CmnResList.size(); i++)
				{
					i3::generic_string_cat( strTemp, CmnResList[i].c_str() );

					if( i < CmnResList.size() - 1)
						i3::generic_string_cat( strTemp, "\r\n");
				}

				pRSCItem->SetToolTipText( strTemp);
			}
		}
	}
	else if( i3::kind_of<i3StageInfo*>(pData)) //->IsTypeOf( i3StageInfo::static_meta()))				//	StageInfo Property
	{
		m_ListCtrl.NewSubItem(9);

		i3StageInfo * pStageInfo = (i3StageInfo *) pData;
		__CHECKW_RT( pStageInfo == NULL, "Warning! Invalid Data on select");

		//	Subject name
		INT32 startIndex = PROPERTY_STATIC_STAGE_NAME;
		INT32 endIndex = PROPERTY_STATIC_STAGE_RSC;
		for( INT32 i = startIndex; i <= endIndex; i++)
		{			
			pSubItem = m_pPropertyItem[ i];
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Name
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_NAME];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_NAME);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->GetName());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Stage Class Name	
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_CLASSNAME];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_CLASSNAME);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->getStageClassName());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	FadeInColor
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_FADEINCOLOR];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_FADEINCOLOR);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->getFadeInColor());

			i3TDKPropertySubItemColor * pColorItem = (i3TDKPropertySubItemColor *) pSubItem;
			pColorItem->SetColor( pStageInfo->getFadeInColor());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	FadeInTime
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_FADEINTIME];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_FADEINTIME);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->getFadeInTime());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	FadeOutColor
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_FADEOUTCOLOR];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_FADEOUTCOLOR);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->getFadeOutColor());

			i3TDKPropertySubItemColor * pColorItem = (i3TDKPropertySubItemColor *) pSubItem;
			pColorItem->SetColor( pStageInfo->getFadeOutColor());	//	설정된 색상

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	FadeOutTime
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_FADEOUTTIME];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_FADEOUTTIME);
			pSubItem->SetUserData( (void *) pStageInfo);
			pSubItem->SetLabel( pStageInfo->getFadeOutTime());		

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	NextStage
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_NEXTSTAGE];		
			pSubItem->SetID( PROPERTY_VALUE_STAGE_NEXTSTAGE);
			pSubItem->SetUserData( (void *) pStageInfo);

			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			
			i3StageInfo * pNextStage = pStageInfo->getNextStage();
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement("(null)");		//	Element(0) = NULL

			//	ComboBox에 Element로 StageInfo의 Name 추가
			for( INT32 i = 0; i < g_pGameFramework->getStageInfoCount(); i++)
			{
				i3StageInfo * pStage = g_pGameFramework->getStageInfo( i);
				pComboBoxItem->AddElement( (char *) pStage->GetName());

				//	현재 선택된 Element
				if( pNextStage == pStage)
					pComboBoxItem->SelectElement( i + 1);
			}			
			
			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			char * pszString = (char *) pComboBoxItem->GetElement( nSelect);
			pSubItem->SetLabel( pszString);

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	JumpType
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_JUMPTYPE];		
			pSubItem->SetID( PROPERTY_VALUE_STAGE_JUMPTYPE);
			pSubItem->SetUserData( (void *) pStageInfo);

			//	ComboBox에 Element로 StageInfo의 JumpType 추가
			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement( "I3FRM_STAGE_JUMP_SET");		//	0
			pComboBoxItem->AddElement( "I3FRM_STAGE_JUMP_PUSH");	//	1
			pComboBoxItem->AddElement( "I3FRM_STAGE_JUMP_POP");		//	2
			
			//	현재 선택된 Element
			switch( pStageInfo->getJumpType())
			{
			case I3FRM_STAGE_JUMP_SET:		pComboBoxItem->SelectElement( 0);	break;
			case I3FRM_STAGE_JUMP_PUSH:		pComboBoxItem->SelectElement( 1);	break;
			case I3FRM_STAGE_JUMP_POP:		pComboBoxItem->SelectElement( 2);	break;
			}

			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			pSubItem->SetLabel( (char *) pComboBoxItem->GetElement( nSelect));

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	RSC File
		{
			i3::vector<i3::rc_string>& RscList = pStageInfo->getRSCPath();

		//	I3ASSERT( pRscList != NULL);

			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_STAGE_RSC];
			pSubItem->SetID( PROPERTY_VALUE_STAGE_RSC);
			pSubItem->SetUserData( &RscList);

			i3TDKPropertySubItemUserCtrl * pRSCItem = (i3TDKPropertySubItemUserCtrl *) pSubItem;	
			
			INT32 count = RscList.size();
			if( count > 0)
				pRSCItem->SetLabel( RscList[count - 1].c_str() );	//	하나 이상일 경우 마지막 RSC가 Label에 보인다.
			else
				pRSCItem->SetLabel( "");
			
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);

			//	Set ToolTip
			{
//				INT32 nToolTipSize = GetStringListBufferSize(RscList); // 	pRscList->GetBufferSize();

				memset( strTemp, 0, sizeof( strTemp));
				for (size_t i = 0; i < RscList.size(); i++)
				{
					i3::generic_string_cat( strTemp, RscList[i].c_str());

					if( i < RscList.size() - 1)
						i3::generic_string_cat( strTemp, "\r\n");
				}

				pRSCItem->SetToolTipText( strTemp);
			}
		}
	}
	else if( i3::kind_of<i3SgLayer*>(pData)) //->IsTypeOf( i3SgLayer::static_meta()))		//	Layer Property
	{
		m_ListCtrl.NewSubItem(3);

		i3SgLayer * pLayer = (i3SgLayer *) pData;
		__CHECKW_RT( pLayer == NULL, "Warning! Invalid Data on select");

		//	Subject name
		INT32 startIndex = PROPERTY_STATIC_LAYER_NAME;
		INT32 endIndex = PROPERTY_STATIC_LAYER_STYLE_TRANS;
		for( INT32 i = startIndex; i <= endIndex; i++)
		{			
			pSubItem = m_pPropertyItem[ i];
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}
		
		//	Name
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_LAYER_NAME];
			pSubItem->SetID( PROPERTY_VALUE_LAYER_NAME);
			pSubItem->SetUserData( (void *) pLayer);
			pSubItem->SetLabel( pLayer->GetName());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Style 2D
		{				
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_LAYER_STYLE_2D];
			pSubItem->SetID( PROPERTY_VALUE_LAYER_STYLE_2D);
			pSubItem->SetUserData( (void *) pLayer);
			
			//	ComboBox에 Element로 Layer의 Style 추가
			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement( "false");		//	0
			pComboBoxItem->AddElement( "true");			//	1
			
			if( pLayer->getStyle() & I3_SG_LAYER_STYLE_2D)
			{
				pComboBoxItem->SelectElement( 1);	//	true
			}
			else
			{
				pComboBoxItem->SelectElement( 0);	//	false
			}

			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			pSubItem->SetLabel( (char *) pComboBoxItem->GetElement( nSelect));

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Style Transparent
		{				
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_LAYER_STYLE_TRANS];
			pSubItem->SetID( PROPERTY_VALUE_LAYER_STYLE_TRANS);
			pSubItem->SetUserData( (void *) pLayer);
			
			//	ComboBox에 Element로 Layer의 Style 추가
			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement( "false");		//	0	
			pComboBoxItem->AddElement( "true");			//	1
			
			if( pLayer->getStyle() & I3_SG_LAYER_STYLE_TRANSPARENT)
			{
				pComboBoxItem->SelectElement( 1);	//	True
			}
			else
			{
				pComboBoxItem->SelectElement( 0);	//	False
			}

			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			pSubItem->SetLabel( (char *) pComboBoxItem->GetElement( nSelect));

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}
	}
	else if( i3::kind_of<i3StageInstanceInfo*>(pData)) //->IsTypeOf( i3StageInstanceInfo::static_meta()))		//	StageInstance Property
	{
		m_ListCtrl.NewSubItem(4);

		i3StageInstanceInfo * pInfo = (i3StageInstanceInfo *) pData;
		__CHECKW_RT( pInfo == NULL, "Warning! Invalid Data on select");

		//	Subject name
		INT32 startIndex = PROPERTY_STATIC_INSTANCE_CLASSNAME;
		INT32 endIndex = PROPERTY_STATIC_INSTANCE_FIXEDCOUNT;
		for( INT32 i = startIndex; i <= endIndex; i++)
		{			
			pSubItem = m_pPropertyItem[ i];
			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}
		
		//	ClassName
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_INSTANCE_CLASSNAME];
			pSubItem->SetID( PROPERTY_VALUE_INSTANCE_CLASSNAME);
			pSubItem->SetUserData( (void *) pInfo);
			pSubItem->SetLabel( pInfo->GetName());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	InstanceCount
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_INSTANCE_COUNT];
			pSubItem->SetID( PROPERTY_VALUE_INSTANCE_COUNT);
			pSubItem->SetUserData( (void *) pInfo);
			pSubItem->SetLabel( pInfo->getInstanceCount());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Instance Manager UpdateInterval
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_INSTANCE_UPDATEINTERVAL];
			pSubItem->SetID( PROPERTY_VALUE_INSTANCE_UPDATEINTERVAL);
			pSubItem->SetUserData( (void *) pInfo);
			pSubItem->SetLabel( pInfo->getUpdateInterval());

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}

		//	Instance Manager is Fixed Count
		{
			pSubItem = m_pPropertyItem[ PROPERTY_EDIT_INSTANCE_FIXEDCOUNT];
			pSubItem->SetID( PROPERTY_VALUE_INSTANCE_FIXEDCOUNT);
			pSubItem->SetUserData( (void *) pInfo);
			
			//	ComboBox에 Element로 Layer의 Style 추가
			i3TDKPropertySubItemComboBox * pComboBoxItem = (i3TDKPropertySubItemComboBox *) pSubItem;
			pComboBoxItem->ClearElement();
			pComboBoxItem->AddElement( "false");		//	0	
			pComboBoxItem->AddElement( "true");			//	1
			
			if( pInfo->isAllocCountFixed())
			{
				pComboBoxItem->SelectElement( 1);	//	True
			}
			else
			{
				pComboBoxItem->SelectElement( 0);	//	False
			}

			INT32 nSelect = pComboBoxItem->GetSelectIdx();
			pSubItem->SetLabel( (char *) pComboBoxItem->GetElement( nSelect));

			m_ListCtrl.SetSubItem( pSubItem->GetRowIndex(), pSubItem->GetColumnIndex(), pSubItem);
		}
	}
	else		//	NULL Property
	{
	}
}

void PanelSpec::ResetGame( void)
{
	SelectPage( NULL);
}


void PanelSpec::_releaseProperty( void)
{
	for( INT32 i = 0; i < PROPERTY_ITEM_MAX; i++)
	{
		I3_SAFE_RELEASE( m_pPropertyItem[ i]);
	}
}

void PanelSpec::_createProperty( void)
{			
	INT32 row = 0;
	INT32 col = 0;
	for( INT32 i = 0; i < PROPERTY_ITEM_MAX; i++)
	{
		switch( (PROPERTY_ITEM_ID) i)
		{
		case PROPERTY_STATIC_FRAMEWORK_NAME:
		case PROPERTY_STATIC_STAGE_NAME:
		case PROPERTY_STATIC_LAYER_NAME:
		case PROPERTY_EDIT_FRAMEWORK_NAME:
		case PROPERTY_EDIT_STAGE_NAME:
		case PROPERTY_EDIT_LAYER_NAME:
		case PROPERTY_STATIC_INSTANCE_CLASSNAME:
		case PROPERTY_EDIT_INSTANCE_CLASSNAME:
			{
				row = 0;
				break;
			}		
		}

		if( i <= PROPERTY_STATIC_INSTANCE_FIXEDCOUNT)
			col = 0;
		else
			col = 1;

		//	SubItem 메모리 생성
		m_pPropertyItem[ i] = (i3TDKPropertySubItem *) g_PropertyItemClassMeta[ i]->create_instance();
			//CREATEINSTANCE( g_PropertyItemClassMeta[ i]);	
		m_pPropertyItem[ i]->AddRef();

		//	i3TDKPropertySubItemStatic
		if( i3::kind_of<i3TDKPropertySubItemStatic*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemStatic::static_meta()))
		{
			SubItemStaticInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];
			itemInit._style = SUBITEM_STYLE_LABELTOOLTIP;

			i3TDKPropertySubItemStatic * pItem = (i3TDKPropertySubItemStatic *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}
		//	i3TDKPropertySubItemEdit
		else if( i3::kind_of<i3TDKPropertySubItemEdit*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemEdit::static_meta()))
		{
			SubItemEditInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];
			itemInit._style = SUBITEM_STYLE_LABELTOOLTIP;
			itemInit._WndStyle = EDIT_WNDSTYLE_LEFT;

			//	실수 입력만 받는다.
			if( (i == PROPERTY_EDIT_STAGE_FADEINTIME) || (i == PROPERTY_EDIT_STAGE_FADEOUTTIME))
				itemInit._style |= EDIT_STYLE_FLOATING_POINT;
		
			i3TDKPropertySubItemEdit * pItem = (i3TDKPropertySubItemEdit *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}
		//	i3TDKPropertySubItemComboBox
		else if( i3::kind_of<i3TDKPropertySubItemComboBox*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemComboBox::static_meta()))
		{
			SubItemComboBoxInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];
			itemInit._WndStyle = CB_WNDSTYLE_DROPDOWNLIST;
			itemInit._style = SUBITEM_STYLE_LABELTOOLTIP;

			i3TDKPropertySubItemComboBox * pItem = (i3TDKPropertySubItemComboBox *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}	
		//	i3TDKPropertySubItemColor
		else if( i3::kind_of<i3TDKPropertySubItemColor*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemColor::static_meta()))
		{
			SubItemColorInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];

			i3TDKPropertySubItemColor * pItem = (i3TDKPropertySubItemColor *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}	
		//	i3TDKPropertySubItemFilePath
		else if( i3::kind_of<i3TDKPropertySubItemFilePath*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemFilePath::static_meta()))
		{
			SubItemFilePathInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];
			itemInit._style = FILEPATH_STYLE_RELATIVE_PATH;
			itemInit._style = SUBITEM_STYLE_LABELTOOLTIP;

			i3TDKPropertySubItemFilePath * pItem = (i3TDKPropertySubItemFilePath *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}	
		//	i3TDKPropertySubItemUserCtrl
		else if( i3::kind_of<i3TDKPropertySubItemUserCtrl*>(m_pPropertyItem[ i])) //->IsTypeOf( i3TDKPropertySubItemUserCtrl::static_meta()))
		{
			SubItemUserCtrlInitStruct itemInit;
			memset( &itemInit, 0, sizeof( itemInit));
			itemInit._row = row;
			itemInit._column = col;
			itemInit._id = i;
			itemInit._label = g_PropertyItemLabel[ i];
			
			if( i == PROPERTY_EDIT_STAGE_RSC || i == PROPERTY_EDIT_FRAMEWORK_CMNRES)//	Stage의 RSC Item 처리 함수
			{
				itemInit._pCallOnHit = callRSCDialogOnHit;		//	SubItem이 Hit되면 callRSCDialogOnHit 함수가 호출된다.
			}

			i3TDKPropertySubItemUserCtrl * pItem = (i3TDKPropertySubItemUserCtrl *) m_pPropertyItem[ i];
			pItem->Create( &itemInit);
		}	

		row++;
	}
}

void PanelSpec::callRSCDialogOnHit( CallOnHitRef * pRef)
{
	//	i3TDKPropertySubItemUserCtrl의 SubItem이 Hit되면 callRSCDialogOnHit 함수가 호출된다.
	PanelSpec * pPanelSpec = &theApp.GetMainFrm()->m_PanelSpec;
	__CHECKF_RT( pPanelSpec == NULL, "Invalid PanelSpec");
	
	i3TDKPropertySubItemUserCtrl * pRSCDialogCtrl = pRef->pUserCtrl;
	__CHECKW_RT( pRSCDialogCtrl == NULL, "UserCtrl is NULL");

	RSCDialog	Dlg;

	i3::vector<i3::rc_string>* pRscList = static_cast< i3::vector<i3::rc_string>* >(pRSCDialogCtrl->GetUserData());
	__CHECKW_RT( pRscList == NULL, "pRscList is NULL");

	//	Dialog Initialize
	Dlg.SetCopyList( pRscList);	

	if( Dlg.DoModal() == IDOK)
	{
		INT32 nToolTipSize = 0;

		pRscList->clear();

		//	Update 메시지를 보낸다.
		for (INT32 i = 0; i < Dlg.GetRSCCount(); i++)
		{
			pRSCDialogCtrl->UpdateMessage( NULL, Dlg.GetRSC( i));
			nToolTipSize += strlen( Dlg.GetRSC( i)) + 2;
		}

		//char * strTemp = (char *) i3MemAlloc( nToolTipSize + 1);
		memset( strTemp, 0, sizeof( strTemp));
		for (INT32 i = 0; i < Dlg.GetRSCCount(); i++)
		{
			i3::generic_string_cat( strTemp, Dlg.GetRSC( i));

			if( i < Dlg.GetRSCCount() - 1)
				i3::generic_string_cat( strTemp, "\r\n");
		}

		pRSCDialogCtrl->SetToolTipText( strTemp);

		//I3MEM_SAFE_FREE( strTemp);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(PanelSpec, i3TDKDialogBase)
	ON_WM_DESTROY()
	ON_WM_CREATE()	
	ON_WM_SIZE()
END_MESSAGE_MAP()

// PanelSpec message handlers

LRESULT PanelSpec::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//	값이 변경된 경우
	if( message == WM_TDK_UPDATE)
	{
		__CHECKW_RTV( g_pGameFramework == NULL, "Warning! Invalid GameFramework", 0);

		I3_TDK_UPDATE_INFO * pInfo = (I3_TDK_UPDATE_INFO *) wParam;

		i3ElementBase * pObj = pInfo->m_pObject;
		if( pObj)
		{
			//	EditBox 에디트 종료
			if( i3::kind_of<i3TDKPropertySubItemEdit*>(pObj)) //->IsTypeOf( i3TDKPropertySubItemEdit::static_meta()) )
			{
				i3TDKPropertySubItemEdit * pSubItem = (i3TDKPropertySubItemEdit *) pObj;

				switch( pSubItem->GetID())
				{
				case PROPERTY_VALUE_FRAMEWORK_NAME:		//	Framework Name 변경
					{
						//i3Framework * g_pGameFramework = (i3Framework *) pSubItem->GetUserData();
						g_pGameFramework->SetName( pSubItem->GetLabel());

						//	PanelGame에 현재 선택된 Item 갱신
						theApp.GetMainFrm()->m_PanelGame.UpdateSelectedItem();
					}
					break;		
				case PROPERTY_VALUE_STAGE_NAME:			//	Stage Name 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->SetName( pSubItem->GetLabel());

						//	PanelGame에 현재 선택된 Item 갱신
						theApp.GetMainFrm()->m_PanelGame.UpdateSelectedItem();
					}
					break;	
				case PROPERTY_VALUE_STAGE_CLASSNAME:	//	Stage Class Name 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->setStageClassName( (char *) pSubItem->GetLabel());
					}
					break;	
				case PROPERTY_VALUE_STAGE_FADEINTIME:	//	Stage FadeIn Time 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->setFadeInTime( (REAL32) atof( pSubItem->GetLabel()));					
					}
					break;	
				case PROPERTY_VALUE_STAGE_FADEOUTTIME:	//	Stage FadeOut Time 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->setFadeOutTime( (REAL32) atof( pSubItem->GetLabel()));
					}
					break;	
				case PROPERTY_VALUE_LAYER_NAME:			//	Layer Name 변경
					{
						i3SgLayer * pLayer = (i3SgLayer *) pSubItem->GetUserData();
						pLayer->SetName( pSubItem->GetLabel());

						//	PanelGame에 현재 선택된 Item 갱신
						theApp.GetMainFrm()->m_PanelGame.UpdateSelectedItem();
					}
					break;	
				case PROPERTY_VALUE_INSTANCE_CLASSNAME:
					{
						i3StageInstanceInfo * pStageInstanceInfo = (i3StageInstanceInfo *) pSubItem->GetUserData();
						pStageInstanceInfo->SetName( pSubItem->GetLabel());

						//	PanelGame에 현재 선택된 Item 갱신
						theApp.GetMainFrm()->m_PanelGame.UpdateSelectedItem();
					}
					break;	
				case PROPERTY_VALUE_INSTANCE_COUNT:
					{
						i3StageInstanceInfo * pStageInstanceInfo = (i3StageInstanceInfo *) pSubItem->GetUserData();
						pStageInstanceInfo->setInstanceCount( (INT32)atoi( pSubItem->GetLabel()));
					}
					break;	
				case PROPERTY_VALUE_INSTANCE_UPDATEINTERVAL:
					{
						i3StageInstanceInfo * pStageInstanceInfo = (i3StageInstanceInfo *) pSubItem->GetUserData();
						pStageInstanceInfo->setUpdateInterval( (REAL32)atof( pSubItem->GetLabel()));
					}
					break;	
				}
			}
			//	ComboBox 에디트 종료
			else if( i3::kind_of<i3TDKPropertySubItemComboBox*>(pObj)) //->IsTypeOf( i3TDKPropertySubItemComboBox::static_meta()) )
			{
				i3TDKPropertySubItemComboBox * pSubItem = (i3TDKPropertySubItemComboBox *) pObj;

				switch( pSubItem->GetID())
				{
				case PROPERTY_VALUE_FRAMEWORK_STARTSTAGE:	//	Framework Start Stage 변경
					{						
						i3StageInfo * pStageInfo = NULL;
						INT32 idx = pSubItem->GetSelectIdx() - 1;	// Idx(-1) = NULL
						if( idx >= 0)
							pStageInfo = g_pGameFramework->getStageInfo( idx);	

						//	Start Stage 설정
						g_pGameFramework->SetStartStageInfo( pStageInfo);
					}
					break;
				case PROPERTY_VALUE_STAGE_NEXTSTAGE:		//	Stage Next Stage 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						
						i3StageInfo * pNextStage = NULL;
						INT32 idx = pSubItem->GetSelectIdx() - 1; // Idx(-1) = NULL
						if( idx >= 0)
							pNextStage = g_pGameFramework->getStageInfo( idx);	

						//	Next Stage 설정
						pStageInfo->setNextStage( pNextStage);
					}
					break;
				case PROPERTY_VALUE_STAGE_JUMPTYPE:			//	Stage Jump Type 변경
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
												
						INT32 idx = pSubItem->GetSelectIdx();						
						switch( idx)
						{
						case 0:		pStageInfo->setJumpType( I3FRM_STAGE_JUMP_SET);		break;
						case 1:		pStageInfo->setJumpType( I3FRM_STAGE_JUMP_PUSH);	break;
						case 2:		pStageInfo->setJumpType( I3FRM_STAGE_JUMP_POP);		break;
						}
					}
					break;
				case PROPERTY_VALUE_LAYER_STYLE_2D:				//	Layer Style 2D
					{
						i3SgLayer * pLayer = (i3SgLayer *) pSubItem->GetUserData();
						UINT32 style = I3_SG_LAYER_STYLE_2D;

						INT32 idx = pSubItem->GetSelectIdx();
						switch( idx)
						{
						case 0:		pLayer->removeStyle( style);						break;		//	False
						case 1:		pLayer->addStyle( style);							break;		//	True
						}
					}
					break;
				case PROPERTY_VALUE_LAYER_STYLE_TRANS:			//	Layer Style Tranparent
					{
						i3SgLayer * pLayer = (i3SgLayer *) pSubItem->GetUserData();
						UINT32 style = I3_SG_LAYER_STYLE_TRANSPARENT;

						INT32 idx = pSubItem->GetSelectIdx();
						switch( idx)
						{
						case 0:		pLayer->removeStyle( style);						break;		//	False
						case 1:		pLayer->addStyle( style);							break;		//	True
						}
					}
					break;
				case PROPERTY_VALUE_INSTANCE_FIXEDCOUNT:
					{
						i3StageInstanceInfo * pStageInstanceInfo = (i3StageInstanceInfo *) pSubItem->GetUserData();

						INT32 idx = pSubItem->GetSelectIdx();
						if( idx == 0)
							pStageInstanceInfo->setAllocCountFixed( FALSE);
						else
							pStageInstanceInfo->setAllocCountFixed( TRUE);
					}
					break;	
				}
			}
			//	색상 에디트 종료
			else if( i3::kind_of<i3TDKPropertySubItemColor*>(pObj)) //->IsTypeOf( i3TDKPropertySubItemColor::static_meta()) )
			{
				i3TDKPropertySubItemColor * pSubItem = (i3TDKPropertySubItemColor *) pObj;

				switch( pSubItem->GetID())
				{
				case PROPERTY_VALUE_STAGE_FADEINCOLOR:
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->setFadeInColor(  pSubItem->GetColor());		//	색상 설정
					}
					break;
				case PROPERTY_VALUE_STAGE_FADEOUTCOLOR:
					{
						i3StageInfo * pStageInfo = (i3StageInfo *) pSubItem->GetUserData();
						pStageInfo->setFadeOutColor(  pSubItem->GetColor());	//	색상 설정	
					}
					break;
				}
			}
			//	UserCtrl 에디트 종료
			else if( i3::kind_of<i3TDKPropertySubItemUserCtrl*>(pObj)) //->IsTypeOf( i3TDKPropertySubItemUserCtrl::static_meta()) )
			{
				i3TDKPropertySubItemUserCtrl * pSubItem = (i3TDKPropertySubItemUserCtrl *) pObj;

				switch( pSubItem->GetID())
				{
				case PROPERTY_VALUE_STAGE_RSC:
				case PROPERTY_VALUE_FRAMEWORK_CMNRES:
					{
						i3::vector<i3::rc_string>* pRscList = (i3::vector<i3::rc_string>*)pSubItem->GetUserData();
						pRscList->push_back(pSubItem->GetLabel());
					}
					break;
				}
			}
		}
	}	//	 end of message

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void PanelSpec::OnDestroy()
{
	i3TDKDialogBase::OnDestroy();

	_releaseProperty();
}

int PanelSpec::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	{   	
		CRect rt;
		GetClientRect( &rt); 
          
		rt.left += 3;
		rt.top += 3;
		rt.right -= 3; 
		rt.bottom -= 3;    

		m_ListCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
		//m_ListCtrl.AddExtenedStyle( LVS_EX_FULLROWSELECT);
		m_ListCtrl.ClearAll();

		m_ListCtrl.AddColumnTitle( "Item", 40);
		m_ListCtrl.AddColumnTitle( "Value", 60); 

		m_ListCtrl.ShowWindow( SW_SHOW);	

		_createProperty();

		ResetGame();

	}	

	return 0;
}

void PanelSpec::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	{
		if( ::IsWindow( m_ListCtrl.m_hWnd))
		{
			m_ListCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
}
